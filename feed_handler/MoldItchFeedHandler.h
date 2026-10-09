#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>

#include "FeedHandlerState.h"
#include "types/SequenceCheckResult.h"
#include "market_data/moldudp64/MoldDatagramQueue.h"

namespace llt::moldudp64
{

    class IMoldMarketDataSource;
    class IMoldItchSequenceRecovery;

    class MoldUdpReceiver;

    struct ReceivedMoldDatagram;
    struct MessageView;
    struct SequencedItchMessage;

} // namespace llt::moldudp64

namespace llt::itch
{

    class ItchMarketState;

    class MoldItchFeedHandler
    {
    public:
        MoldItchFeedHandler(
            llt::moldudp64::IMoldMarketDataSource &source,
            llt::moldudp64::IMoldItchSequenceRecovery &recovery,
            ItchMarketState &marketState) noexcept;

        //
        // Deterministic mode.
        //
        // datagramCount counts successfully processed
        // LIVE MoldUDP64 datagrams.
        //
        // Recovered messages do not count toward
        // datagramCount.
        //
        void start(
            std::size_t datagramCount);

        //
        // Continuous live mode.
        //
        void run();

        // Live mode using a dedicated UDP receiver.
        // The receiver is stopped and joined by the caller.
        // Pending datagrams are drained after the receiver stops.
        void runQueued(
            llt::moldudp64::MoldDatagramQueue &queue,
            const llt::moldudp64::MoldUdpReceiver &receiver);

        void stop() noexcept;

        [[nodiscard]]
        llt::FeedHandlerState state() const noexcept;

        [[nodiscard]]
        std::uint64_t processedDatagrams() const noexcept
        {
            return processedDatagrams_;
        }

        [[nodiscard]]
        std::uint64_t processedMessages() const noexcept
        {
            return processedMessages_;
        }

        [[nodiscard]]
        std::uint64_t recoveredMessages() const noexcept
        {
            return recoveredMessages_;
        }

        [[nodiscard]]
        std::uint64_t ignoredMessages() const noexcept
        {
            return ignoredMessages_;
        }

        [[nodiscard]]
        std::uint64_t gapsDetected() const noexcept
        {
            return gapsDetected_;
        }

        [[nodiscard]]
        std::uint64_t malformedDatagrams() const noexcept
        {
            return malformedDatagrams_;
        }

        [[nodiscard]]
        std::uint64_t expectedSequence() const noexcept
        {
            return expectedSequence_;
        }

    private:
        [[nodiscard]]
        llt::SequenceCheckResult checkSequence(
            std::uint64_t sequence);

        [[nodiscard]]
        bool processDatagram(
            const llt::moldudp64::ReceivedMoldDatagram &datagram);

        [[nodiscard]]
        bool processMessage(
            const llt::moldudp64::MessageView &message);

        [[nodiscard]]
        bool processRecoveredMessage(
            const llt::moldudp64::SequencedItchMessage &message);

        void startLifecycle() noexcept;

        void finishLifecycle() noexcept;

    private:
        llt::moldudp64::IMoldMarketDataSource &source_;

        llt::moldudp64::IMoldItchSequenceRecovery &recovery_;

        ItchMarketState &marketState_;

        std::uint64_t expectedSequence_{0};

        bool hasSequence_{false};

        std::uint64_t processedDatagrams_{0};

        std::uint64_t processedMessages_{0};

        std::uint64_t recoveredMessages_{0};

        std::uint64_t ignoredMessages_{0};

        std::uint64_t gapsDetected_{0};

        std::uint64_t malformedDatagrams_{0};

        std::atomic<bool> running_{
            false};

        std::atomic<llt::FeedHandlerState> state_{
            llt::FeedHandlerState::Stopped};
    };

} // namespace llt::itch