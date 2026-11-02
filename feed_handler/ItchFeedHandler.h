#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "FeedHandlerState.h"
#include "types/SequenceCheckResult.h"
#include "market_data/itch/ItchMarketState.h"
#include "market_data/itch/IItchMarketDataSource.h"
#include "market_data/itch/IItchSequenceRecovery.h"
#include "market_data/itch/ItchDispatcher.h"
#include "market_data/itch/ItchUdpPacket.h"

namespace llt::itch
{
    template <typename BboHandler = NullBboHandler, 
              typename TradeHandler = NullTradeHandler>
    class ItchFeedHandler
    {
    public:
        // Constructor takes marketState with matching template parameters
        ItchFeedHandler(
            IItchMarketDataSource &source,
            IItchSequenceRecovery &recovery,
            ItchMarketState<BboHandler, TradeHandler> &marketState) noexcept
            : source_(source),
              recovery_(recovery),
              marketState_(marketState)
        {
        }

        void start(std::size_t packetCount)
        {
            startLifecycle();

            std::size_t acceptedLivePackets{0};
            ItchUdpPacket packet{};

            while (
                running_.load(std::memory_order_acquire) &&
                acceptedLivePackets < packetCount)
            {
                if (!source_.receive(packet))
                {
                    continue;
                }

                const auto result = checkSequence(packet.sequence);

                if (result == llt::SequenceCheckResult::Stop)
                {
                    state_.store(llt::FeedHandlerState::Failed, std::memory_order_release);
                    break;
                }

                if (result == llt::SequenceCheckResult::Ignore)
                {
                    continue;
                }

                if (!processPacket(packet))
                {
                    state_.store(llt::FeedHandlerState::Failed, std::memory_order_release);
                    break;
                }

                ++processedPackets_;
                ++acceptedLivePackets;
            }

            finishLifecycle();
        }

        void run()
        {
            startLifecycle();

            ItchUdpPacket packet{};

            while (running_.load(std::memory_order_acquire))
            {
                if (!source_.receive(packet))
                {
                    continue;
                }

                const auto result = checkSequence(packet.sequence);

                if (result == llt::SequenceCheckResult::Stop)
                {
                    state_.store(llt::FeedHandlerState::Failed, std::memory_order_release);
                    break;
                }

                if (result == llt::SequenceCheckResult::Ignore)
                {
                    continue;
                }

                if (!processPacket(packet))
                {
                    state_.store(llt::FeedHandlerState::Failed, std::memory_order_release);
                    break;
                }

                ++processedPackets_;
            }

            finishLifecycle();
        }

        void stop() noexcept
        {
            running_.store(false, std::memory_order_release);
        }

        [[nodiscard]]
        llt::FeedHandlerState state() const noexcept
        {
            return state_.load(std::memory_order_acquire);
        }

        [[nodiscard]]
        std::uint64_t processedPackets() const noexcept { return processedPackets_; }

        [[nodiscard]]
        std::uint64_t recoveredPackets() const noexcept { return recoveredPackets_; }

        [[nodiscard]]
        std::uint64_t ignoredPackets() const noexcept { return ignoredPackets_; }

        [[nodiscard]]
        std::uint64_t gapsDetected() const noexcept { return gapsDetected_; }

    private:
        [[nodiscard]]
        llt::SequenceCheckResult checkSequence(std::uint64_t sequence)
        {
            if (!hasSequence_)
            {
                expectedSequence_ = sequence + 1;
                hasSequence_ = true;
                return llt::SequenceCheckResult::Process;
            }

            if (sequence == expectedSequence_)
            {
                expectedSequence_ = sequence + 1;
                return llt::SequenceCheckResult::Process;
            }

            if (sequence < expectedSequence_)
            {
                ++ignoredPackets_;
                return llt::SequenceCheckResult::Ignore;
            }

            ++gapsDetected_;

            std::vector<ItchUdpPacket> recoveredPackets;
            const auto recoveryResult = recovery_.recover(
                expectedSequence_,
                sequence,
                recoveredPackets);

            if (recoveryResult != llt::SequenceCheckResult::Process)
            {
                return recoveryResult;
            }

            for (const auto& recoveredPacket : recoveredPackets)
            {
                if (!processPacket(recoveredPacket))
                {
                    return llt::SequenceCheckResult::Stop;
                }
                ++recoveredPackets_;
            }

            expectedSequence_ = sequence + 1;
            return llt::SequenceCheckResult::Process;
        }

        [[nodiscard]]
        bool processPacket(const ItchUdpPacket &packet)
        {
            if (packet.payloadSize == 0 || packet.payloadSize > ItchUdpPacket::MaxPayloadSize)
            {
                return false;
            }

            const auto message = ItchDispatcher::dispatch(
                packet.payload.data(),
                static_cast<std::size_t>(packet.payloadSize));

            if (!message.has_value())
            {
                return false;
            }

            marketState_.onMessage(*message);
            return true;
        }

        void startLifecycle() noexcept
        {
            expectedSequence_ = 0;
            hasSequence_ = false;
            processedPackets_ = 0;
            recoveredPackets_ = 0;
            ignoredPackets_ = 0;
            gapsDetected_ = 0;

            state_.store(llt::FeedHandlerState::Running, std::memory_order_release);
            running_.store(true, std::memory_order_release);
        }

        void finishLifecycle() noexcept
        {
            running_.store(false, std::memory_order_release);

            if (state_.load(std::memory_order_acquire) != llt::FeedHandlerState::Failed)
            {
                state_.store(llt::FeedHandlerState::Stopped, std::memory_order_release);
            }
        }

    private:
        IItchMarketDataSource &source_;
        IItchSequenceRecovery &recovery_;
        ItchMarketState<BboHandler, TradeHandler> &marketState_;

        std::uint64_t expectedSequence_{0};
        bool hasSequence_{false};

        std::uint64_t processedPackets_{0};
        std::uint64_t recoveredPackets_{0};
        std::uint64_t ignoredPackets_{0};
        std::uint64_t gapsDetected_{0};

        std::atomic<bool> running_{false};
        std::atomic<llt::FeedHandlerState> state_{llt::FeedHandlerState::Stopped};
    };

} // namespace llt::itch