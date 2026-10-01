#include "ItchFeedHandler.h"

#include <vector>

#include "market_data/itch/IItchSequenceRecovery.h"
#include "market_data/itch/ItchDispatcher.h"
#include "market_data/itch/ItchMarketState.h"
#include "market_data/itch/ItchUdpPacket.h"
#include "market_data/itch/UdpItchMarketDataSource.h"


namespace llt::itch
{

ItchFeedHandler::ItchFeedHandler(
    IItchMarketDataSource& source,
    IItchSequenceRecovery& recovery,
    ItchMarketState& marketState
) noexcept
    : source_(source),
      recovery_(recovery),
      marketState_(marketState)
{
}


void ItchFeedHandler::startLifecycle() noexcept
{
    expectedSequence_ = 0;

    hasSequence_ = false;

    processedPackets_ = 0;

    recoveredPackets_ = 0;

    ignoredPackets_ = 0;

    gapsDetected_ = 0;

    state_.store(
        llt::FeedHandlerState::Running,
        std::memory_order_release);

    running_.store(
        true,
        std::memory_order_release);
}


void ItchFeedHandler::finishLifecycle() noexcept
{
    running_.store(
        false,
        std::memory_order_release);

    if (
        state_.load(
            std::memory_order_acquire)
        != llt::FeedHandlerState::Failed)
    {
        state_.store(
            llt::FeedHandlerState::Stopped,
            std::memory_order_release);
    }
}


bool ItchFeedHandler::processPacket(
    const ItchUdpPacket& packet)
{
    if (
        packet.payloadSize == 0 ||
        packet.payloadSize >
            ItchUdpPacket::MaxPayloadSize)
    {
        return false;
    }

    const auto message =
        ItchDispatcher::dispatch(
            packet.payload.data(),
            static_cast<std::size_t>(
                packet.payloadSize));

    if (!message.has_value())
    {
        return false;
    }

    marketState_.onMessage(
        *message);

    return true;
}


llt::SequenceCheckResult
ItchFeedHandler::checkSequence(
    std::uint64_t sequence)
{
    //
    // First packet establishes the sequence.
    //
    if (!hasSequence_)
    {
        expectedSequence_ =
            sequence + 1;

        hasSequence_ = true;

        return
            llt::SequenceCheckResult::Process;
    }

    //
    // Expected packet.
    //
    if (sequence == expectedSequence_)
    {
        expectedSequence_ =
            sequence + 1;

        return
            llt::SequenceCheckResult::Process;
    }

    //
    // Old / duplicate / late packet.
    //
    if (sequence < expectedSequence_)
    {
        ++ignoredPackets_;

        return
            llt::SequenceCheckResult::Ignore;
    }

    //
    // Forward gap.
    //
    ++gapsDetected_;

    std::vector<ItchUdpPacket>
        recoveredPackets;

    const auto recoveryResult =
        recovery_.recover(
            expectedSequence_,
            sequence,
            recoveredPackets);

    if (
        recoveryResult !=
        llt::SequenceCheckResult::Process)
    {
        return recoveryResult;
    }

    //
    // ItchSequenceRecovery has already verified
    // that this vector contains exactly:
    //
    // [expectedSequence_, sequence)
    //
    // in contiguous sequence order.
    //
    for (
        const auto& recoveredPacket :
        recoveredPackets)
    {
        if (!processPacket(
                recoveredPacket))
        {
            return
                llt::SequenceCheckResult::Stop;
        }

        ++recoveredPackets_;
    }

    //
    // All missing packets have now entered
    // ItchDispatcher -> ItchMarketState.
    //
    // The current live packet will be processed
    // by the caller immediately afterward.
    //
    expectedSequence_ =
        sequence + 1;

    return
        llt::SequenceCheckResult::Process;
}


void ItchFeedHandler::start(
    std::size_t packetCount)
{
    startLifecycle();

    std::size_t acceptedLivePackets{0};

    ItchUdpPacket packet{};

    while (
        running_.load(
            std::memory_order_acquire) &&
        acceptedLivePackets < packetCount)
    {
        if (!source_.receive(packet))
        {
            continue;
        }

        const auto result =
            checkSequence(
                packet.sequence);

        if (
            result ==
            llt::SequenceCheckResult::Stop)
        {
            state_.store(
                llt::FeedHandlerState::Failed,
                std::memory_order_release);

            break;
        }

        if (
            result ==
            llt::SequenceCheckResult::Ignore)
        {
            continue;
        }

        if (!processPacket(packet))
        {
            state_.store(
                llt::FeedHandlerState::Failed,
                std::memory_order_release);

            break;
        }

        ++processedPackets_;

        ++acceptedLivePackets;
    }

    finishLifecycle();
}


void ItchFeedHandler::run()
{
    startLifecycle();

    ItchUdpPacket packet{};

    while (
        running_.load(
            std::memory_order_acquire))
    {
        if (!source_.receive(packet))
        {
            continue;
        }

        const auto result =
            checkSequence(
                packet.sequence);

        if (
            result ==
            llt::SequenceCheckResult::Stop)
        {
            state_.store(
                llt::FeedHandlerState::Failed,
                std::memory_order_release);

            break;
        }

        if (
            result ==
            llt::SequenceCheckResult::Ignore)
        {
            continue;
        }

        if (!processPacket(packet))
        {
            state_.store(
                llt::FeedHandlerState::Failed,
                std::memory_order_release);

            break;
        }

        ++processedPackets_;
    }

    finishLifecycle();
}


void ItchFeedHandler::stop() noexcept
{
    running_.store(
        false,
        std::memory_order_release);
}


llt::FeedHandlerState
ItchFeedHandler::state() const noexcept
{
    return state_.load(
        std::memory_order_acquire);
}

} // namespace llt::itch