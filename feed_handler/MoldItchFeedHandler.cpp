#include "MoldItchFeedHandler.h"

#include <vector>

#include "market_data/itch/ItchDispatcher.h"
#include "market_data/itch/ItchMarketState.h"

#include "market_data/moldudp64/IMoldItchSequenceRecovery.h"
#include "market_data/moldudp64/IMoldMarketDataSource.h"
#include "market_data/moldudp64/MoldUdp64.h"
#include "market_data/moldudp64/MoldUdp64Codec.h"
#include "market_data/moldudp64/SequencedItchMessage.h"


namespace llt::itch
{


MoldItchFeedHandler::MoldItchFeedHandler(
    llt::moldudp64::IMoldMarketDataSource& source,
    llt::moldudp64::IMoldItchSequenceRecovery& recovery,
    ItchMarketState& marketState
) noexcept
    : source_(source),
      recovery_(recovery),
      marketState_(marketState)
{
}


void MoldItchFeedHandler::startLifecycle() noexcept
{
    expectedSequence_ = 0;

    hasSequence_ = false;

    processedDatagrams_ = 0;

    processedMessages_ = 0;

    recoveredMessages_ = 0;

    ignoredMessages_ = 0;

    gapsDetected_ = 0;

    malformedDatagrams_ = 0;

    state_.store(
        llt::FeedHandlerState::Running,
        std::memory_order_release);

    running_.store(
        true,
        std::memory_order_release);
}


void MoldItchFeedHandler::finishLifecycle() noexcept
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


bool MoldItchFeedHandler::processMessage(
    const llt::moldudp64::MessageView& message)
{
    if (
        message.data == nullptr ||
        message.size == 0)
    {
        return false;
    }

    const auto decoded =
        ItchDispatcher::dispatch(
            message.data,
            message.size);

    if (!decoded.has_value())
    {
        return false;
    }

    marketState_.onMessage(
        *decoded);

    ++processedMessages_;

    return true;
}


bool MoldItchFeedHandler::processRecoveredMessage(
    const llt::moldudp64::SequencedItchMessage& message)
{
    if (
        message.payloadSize == 0 ||
        message.payloadSize >
            llt::moldudp64::SequencedItchMessage::
                MaxPayloadSize)
    {
        return false;
    }

    const auto decoded =
        ItchDispatcher::dispatch(
            message.payload.data(),
            static_cast<std::size_t>(
                message.payloadSize));

    if (!decoded.has_value())
    {
        return false;
    }

    marketState_.onMessage(
        *decoded);

    ++recoveredMessages_;

    return true;
}


llt::SequenceCheckResult
MoldItchFeedHandler::checkSequence(
    std::uint64_t sequence)
{
    //
    // First message establishes the sequence.
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
    // Expected live message.
    //
    if (sequence == expectedSequence_)
    {
        expectedSequence_ =
            sequence + 1;

        return
            llt::SequenceCheckResult::Process;
    }

    //
    // Old / duplicate / overlapping message.
    //
    if (sequence < expectedSequence_)
    {
        ++ignoredMessages_;

        return
            llt::SequenceCheckResult::Ignore;
    }

    //
    // Forward gap.
    //
    ++gapsDetected_;

    std::vector<
        llt::moldudp64::SequencedItchMessage>
        recoveredMessages;

    const auto recoveryResult =
        recovery_.recover(
            expectedSequence_,
            sequence,
            recoveredMessages);

    if (
        recoveryResult !=
        llt::SequenceCheckResult::Process)
    {
        return recoveryResult;
    }

    //
    // MoldItchSequenceRecovery has already verified
    // that this vector contains exactly:
    //
    // [expectedSequence_, sequence)
    //
    // in contiguous sequence order.
    //
    for (
        const auto& recoveredMessage :
        recoveredMessages)
    {
        if (!processRecoveredMessage(
                recoveredMessage))
        {
            return
                llt::SequenceCheckResult::Stop;
        }
    }

    //
    // All missing messages have now entered:
    //
    // ItchDispatcher -> ItchMarketState.
    //
    // The current live message will be processed
    // by the caller immediately afterward.
    //
    expectedSequence_ =
        sequence + 1;

    return
        llt::SequenceCheckResult::Process;
}


bool MoldItchFeedHandler::processDatagram(
    const llt::moldudp64::ReceivedMoldDatagram& datagram)
{
    llt::moldudp64::Header header{};

    if (
        !llt::moldudp64::MoldUdp64Codec::decodeHeader(
            datagram.bytes.data(),
            datagram.size,
            header))
    {
        return false;
    }

    //
    // For this first handler version we do not yet
    // process MoldUDP64 heartbeat packets.
    //
    // Heartbeat support will be added explicitly
    // rather than allowing it to affect sequence
    // establishment accidentally.
    //
    if (header.messageCount == 0)
    {
        return false;
    }

    //
    // =====================================================
    // Pass 1: validate the complete MoldUDP64 framing
    // =====================================================
    //
    // Do this BEFORE mutating ItchMarketState.
    //
    // Otherwise a malformed datagram could contain two
    // valid messages followed by broken framing, causing
    // partial state mutation before failure is discovered.
    //
    std::size_t validationOffset =
        llt::moldudp64::HeaderSize;

    std::uint64_t validationSequence =
        header.sequenceNumber;

    for (
        std::uint16_t i = 0;
        i < header.messageCount;
        ++i)
    {
        llt::moldudp64::MessageView
            validationMessage{};

        if (
            !llt::moldudp64::MoldUdp64Codec::nextMessage(
                datagram.bytes.data(),
                datagram.size,
                validationOffset,
                validationSequence,
                validationMessage))
        {
            return false;
        }

        ++validationSequence;
    }

    //
    // messageCount must describe the entire payload.
    //
    // Reject unexpected trailing bytes.
    //
    if (
        validationOffset !=
        datagram.size)
    {
        return false;
    }

    //
    // =====================================================
    // Pass 2: sequence-check and process each ITCH message
    // =====================================================
    //
    std::size_t offset =
        llt::moldudp64::HeaderSize;

    std::uint64_t sequence =
        header.sequenceNumber;

    for (
        std::uint16_t i = 0;
        i < header.messageCount;
        ++i)
    {
        llt::moldudp64::MessageView
            message{};

        if (
            !llt::moldudp64::MoldUdp64Codec::nextMessage(
                datagram.bytes.data(),
                datagram.size,
                offset,
                sequence,
                message))
        {
            //
            // Pass 1 already validated this framing.
            // Reaching here should therefore not normally
            // be possible.
            //
            return false;
        }

        const auto sequenceResult =
            checkSequence(
                message.sequenceNumber);

        if (
            sequenceResult ==
            llt::SequenceCheckResult::Stop)
        {
            return false;
        }

        if (
            sequenceResult ==
            llt::SequenceCheckResult::Ignore)
        {
            ++sequence;

            continue;
        }

        if (!processMessage(
                message))
        {
            return false;
        }

        ++sequence;
    }

    return true;
}


void MoldItchFeedHandler::start(
    std::size_t datagramCount)
{
    startLifecycle();

    std::size_t
        acceptedLiveDatagrams{0};

    llt::moldudp64::ReceivedMoldDatagram
        datagram{};

    while (
        running_.load(
            std::memory_order_acquire) &&
        acceptedLiveDatagrams <
            datagramCount)
    {
        if (!source_.receive(
                datagram))
        {
            continue;
        }

        if (!processDatagram(
                datagram))
        {
            ++malformedDatagrams_;

            state_.store(
                llt::FeedHandlerState::Failed,
                std::memory_order_release);

            break;
        }

        ++processedDatagrams_;

        ++acceptedLiveDatagrams;
    }

    finishLifecycle();
}


void MoldItchFeedHandler::run()
{
    startLifecycle();

    llt::moldudp64::ReceivedMoldDatagram
        datagram{};

    while (
        running_.load(
            std::memory_order_acquire))
    {
        if (!source_.receive(
                datagram))
        {
            continue;
        }

        if (!processDatagram(
                datagram))
        {
            ++malformedDatagrams_;

            state_.store(
                llt::FeedHandlerState::Failed,
                std::memory_order_release);

            break;
        }

        ++processedDatagrams_;
    }

    finishLifecycle();
}


void MoldItchFeedHandler::stop() noexcept
{
    running_.store(
        false,
        std::memory_order_release);
}


llt::FeedHandlerState
MoldItchFeedHandler::state() const noexcept
{
    return state_.load(
        std::memory_order_acquire);
}


} // namespace llt::itch