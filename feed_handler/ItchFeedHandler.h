#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>

#include "FeedHandlerState.h"
#include "types/SequenceCheckResult.h"

namespace llt::itch
{

    class IItchMarketDataSource;
    class IItchSequenceRecovery;
    class ItchMarketState;
    struct ItchUdpPacket;

    class ItchFeedHandler
    {
    public:
        ItchFeedHandler(
            IItchMarketDataSource &source,
            IItchSequenceRecovery &recovery,
            ItchMarketState &marketState) noexcept;

        //
        // Deterministic mode.
        //
        // packetCount counts successfully processed
        // LIVE packets.
        //
        // Recovered packets do not count toward
        // packetCount.
        //
        void start(
            std::size_t packetCount);

        //
        // Continuous live mode.
        //
        void run();

        void stop() noexcept;

        [[nodiscard]]
        llt::FeedHandlerState state() const noexcept;

        [[nodiscard]]
        std::uint64_t processedPackets() const noexcept
        {
            return processedPackets_;
        }

        [[nodiscard]]
        std::uint64_t recoveredPackets() const noexcept
        {
            return recoveredPackets_;
        }

        [[nodiscard]]
        std::uint64_t ignoredPackets() const noexcept
        {
            return ignoredPackets_;
        }

        [[nodiscard]]
        std::uint64_t gapsDetected() const noexcept
        {
            return gapsDetected_;
        }

    private:
        [[nodiscard]]
        llt::SequenceCheckResult checkSequence(
            std::uint64_t sequence);

        [[nodiscard]]
        bool processPacket(
            const ItchUdpPacket &packet);

        void startLifecycle() noexcept;

        void finishLifecycle() noexcept;

    private:
        IItchMarketDataSource &source_;

        IItchSequenceRecovery &recovery_;

        ItchMarketState &marketState_;

        std::uint64_t expectedSequence_{0};

        bool hasSequence_{false};

        std::uint64_t processedPackets_{0};

        std::uint64_t recoveredPackets_{0};

        std::uint64_t ignoredPackets_{0};

        std::uint64_t gapsDetected_{0};

        std::atomic<bool> running_{
            false};

        std::atomic<llt::FeedHandlerState> state_{
            llt::FeedHandlerState::Stopped};
    };

} // namespace llt::itch