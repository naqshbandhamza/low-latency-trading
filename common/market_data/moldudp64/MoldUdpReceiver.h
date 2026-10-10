
#pragma once

#include <atomic>
#include <cstdint>
#include <thread>

#include "market_data/moldudp64/IMoldMarketDataSource.h"
#include "market_data/moldudp64/MoldDatagramQueue.h"

namespace llt::moldudp64
{

    // temp
    struct ReceiverTimingStats
    {
        std::uint64_t maxReceiveCallNs = 0;
        std::uint64_t maxQueuePushNs = 0;
        std::uint64_t maxLoopGapNs = 0;

        std::uint64_t loopGapsOver100us = 0;
        std::uint64_t loopGapsOver500us = 0;
        std::uint64_t loopGapsOver1ms = 0;

        // Time between a successful receive and the next receive call.
        std::uint64_t maxPostReceiveWorkNs = 0;
        std::uint64_t postWorkOver10us = 0;
        std::uint64_t postWorkOver100us = 0;
        std::uint64_t postWorkOver1ms = 0;

        // SPSC push latency distribution.
        std::uint64_t pushesOver10us = 0;
        std::uint64_t pushesOver100us = 0;
        std::uint64_t pushesOver1ms = 0;

        // Successful receive-call latency distribution.
        std::uint64_t successfulReceivesOver1ms = 0;

        // Receive failures, including timeouts.
        std::uint64_t unsuccessfulReceiveCalls = 0;
    };
    //

    class MoldUdpReceiver final
    {
    public:
        MoldUdpReceiver(
            IMoldMarketDataSource &source,
            MoldDatagramQueue &queue) noexcept;

        ~MoldUdpReceiver();

        MoldUdpReceiver(const MoldUdpReceiver &) = delete;
        MoldUdpReceiver &operator=(const MoldUdpReceiver &) = delete;

        void start();
        void stop() noexcept;
        void join() noexcept;

        [[nodiscard]]
        bool running() const noexcept;

        [[nodiscard]]
        std::uint64_t receivedDatagrams() const noexcept;

        [[nodiscard]]
        std::uint64_t queuedDatagrams() const noexcept;

        [[nodiscard]]
        std::uint64_t queueFullDrops() const noexcept;

        [[nodiscard]]
        std::uint64_t maxQueueOccupancy() const noexcept;

        // temp
        [[nodiscard]]
        ReceiverTimingStats timingStats() const noexcept
        {
            return timingStats_;
        }
        //

    private:
        void receiveLoop() noexcept;

        IMoldMarketDataSource &source_;
        MoldDatagramQueue &queue_;

        std::thread worker_;

        std::atomic<bool> running_{false};

        std::atomic<std::uint64_t> receivedDatagrams_{0};
        std::atomic<std::uint64_t> queuedDatagrams_{0};
        std::atomic<std::uint64_t> queueFullDrops_{0};
        std::atomic<std::uint64_t> maxQueueOccupancy_{0};

        // temp
        ReceiverTimingStats timingStats_{};
        //
    };

} // namespace llt::moldudp64
