
#include "market_data/moldudp64/MoldUdpReceiver.h"
#include <chrono>
#include <cstddef>
#include <stdexcept>

#include <algorithm>
#include <cstdint>

namespace llt::moldudp64
{

    MoldUdpReceiver::MoldUdpReceiver(
        IMoldMarketDataSource &source,
        MoldDatagramQueue &queue) noexcept
        : source_(source),
          queue_(queue)
    {
    }

    MoldUdpReceiver::~MoldUdpReceiver()
    {
        stop();
        join();
    }

    void MoldUdpReceiver::start()
    {
        if (worker_.joinable())
        {
            throw std::logic_error(
                "MoldUdpReceiver thread must be joined before restart");
        }

        bool expected = false;

        if (!running_.compare_exchange_strong(
                expected,
                true,
                std::memory_order_acq_rel))
        {
            throw std::logic_error(
                "MoldUdpReceiver is already running");
        }

        receivedDatagrams_.store(0, std::memory_order_relaxed);
        queuedDatagrams_.store(0, std::memory_order_relaxed);
        queueFullDrops_.store(0, std::memory_order_relaxed);
        maxQueueOccupancy_.store(0, std::memory_order_relaxed);

        try
        {
            worker_ = std::thread(
                &MoldUdpReceiver::receiveLoop,
                this);
        }
        catch (...)
        {
            running_.store(false, std::memory_order_release);
            throw;
        }
    }

    void MoldUdpReceiver::stop() noexcept
    {
        running_.store(false, std::memory_order_release);
    }

    void MoldUdpReceiver::join() noexcept
    {
        if (worker_.joinable())
        {
            worker_.join();
        }
    }

    bool MoldUdpReceiver::running() const noexcept
    {
        return running_.load(std::memory_order_acquire);
    }

    std::uint64_t MoldUdpReceiver::receivedDatagrams() const noexcept
    {
        return receivedDatagrams_.load(std::memory_order_relaxed);
    }

    std::uint64_t MoldUdpReceiver::queuedDatagrams() const noexcept
    {
        return queuedDatagrams_.load(std::memory_order_relaxed);
    }

    std::uint64_t MoldUdpReceiver::queueFullDrops() const noexcept
    {
        return queueFullDrops_.load(std::memory_order_relaxed);
    }

    std::uint64_t MoldUdpReceiver::maxQueueOccupancy() const noexcept
    {
        return maxQueueOccupancy_.load(std::memory_order_relaxed);
    }

    // void MoldUdpReceiver::receiveLoop() noexcept
    // {
    //     ReceivedMoldDatagram datagram{};

    //     while (running_.load(std::memory_order_acquire))
    //     {
    //         if (!source_.receive(datagram))
    //         {
    //             continue;
    //         }

    //         receivedDatagrams_.fetch_add(
    //             1, std::memory_order_relaxed);

    //         if (!queue_.push(datagram))
    //         {
    //             queueFullDrops_.fetch_add(
    //                 1, std::memory_order_relaxed);

    //             continue;
    //         }

    //         queuedDatagrams_.fetch_add(
    //             1, std::memory_order_relaxed);

    //         const auto occupancy =
    //             static_cast<std::uint64_t>(queue_.size());

    //         auto previousMax =
    //             maxQueueOccupancy_.load(std::memory_order_relaxed);

    //         while (occupancy > previousMax &&
    //                !maxQueueOccupancy_.compare_exchange_weak(
    //                    previousMax,
    //                    occupancy,
    //                    std::memory_order_relaxed,
    //                    std::memory_order_relaxed))
    //         {
    //         }
    //     }

    //     running_.store(false, std::memory_order_release);
    // }

    // temp
    void MoldUdpReceiver::receiveLoop() noexcept
    {
        using Clock = std::chrono::steady_clock;
        using Nanoseconds = std::chrono::nanoseconds;

        ReceivedMoldDatagram datagram{};

        auto previousIteration = Clock::now();

        while (running_.load(std::memory_order_acquire))
        {
            const auto loopStart = Clock::now();

            const auto loopGapNs =
                std::chrono::duration_cast<Nanoseconds>(
                    loopStart - previousIteration)
                    .count();

            if (loopGapNs > 0)
            {
                const auto gap =
                    static_cast<std::uint64_t>(loopGapNs);

                timingStats_.maxLoopGapNs =
                    std::max(timingStats_.maxLoopGapNs, gap);

                if (gap > 100'000)
                    ++timingStats_.loopGapsOver100us;

                if (gap > 500'000)
                    ++timingStats_.loopGapsOver500us;

                if (gap > 1'000'000)
                    ++timingStats_.loopGapsOver1ms;
            }

            // Start timing the receive operation.
            const auto receiveStart = Clock::now();

            const bool received = source_.receive(datagram);

            const auto receiveEnd = Clock::now();

            const auto receiveNs =
                std::chrono::duration_cast<Nanoseconds>(
                    receiveEnd - receiveStart)
                    .count();

            if (receiveNs > 0)
            {
                const auto duration =
                    static_cast<std::uint64_t>(receiveNs);

                timingStats_.maxReceiveCallNs =
                    std::max(
                        timingStats_.maxReceiveCallNs,
                        duration);

                if (received && duration > 1'000'000)
                    ++timingStats_.successfulReceivesOver1ms;
            }

            previousIteration = loopStart;

            if (!received)
            {
                ++timingStats_.unsuccessfulReceiveCalls;
                continue;
            }

            receivedDatagrams_.fetch_add(
                1, std::memory_order_relaxed);

            // Measure queue push duration.
            const auto pushStart = Clock::now();

            const bool pushed = queue_.push(datagram);

            const auto pushEnd = Clock::now();

            const auto pushNs =
                std::chrono::duration_cast<Nanoseconds>(
                    pushEnd - pushStart)
                    .count();

            if (pushNs > 0)
            {
                const auto duration =
                    static_cast<std::uint64_t>(pushNs);

                timingStats_.maxQueuePushNs =
                    std::max(
                        timingStats_.maxQueuePushNs,
                        duration);

                if (duration > 10'000)
                    ++timingStats_.pushesOver10us;

                if (duration > 100'000)
                    ++timingStats_.pushesOver100us;

                if (duration > 1'000'000)
                    ++timingStats_.pushesOver1ms;
            }

            if (!pushed)
            {
                queueFullDrops_.fetch_add(
                    1, std::memory_order_relaxed);
            }
            else
            {
                queuedDatagrams_.fetch_add(
                    1, std::memory_order_relaxed);

                const auto occupancy =
                    static_cast<std::uint64_t>(queue_.size());

                auto previousMax =
                    maxQueueOccupancy_.load(
                        std::memory_order_relaxed);

                while (occupancy > previousMax &&
                       !maxQueueOccupancy_.compare_exchange_weak(
                           previousMax,
                           occupancy,
                           std::memory_order_relaxed,
                           std::memory_order_relaxed))
                {
                }
            }

            // Measure all work after receive() returned,
            // including timing instrumentation itself.
            const auto postWorkEnd = Clock::now();

            const auto postWorkNs =
                std::chrono::duration_cast<Nanoseconds>(
                    postWorkEnd - receiveEnd)
                    .count();

            if (postWorkNs > 0)
            {
                const auto duration =
                    static_cast<std::uint64_t>(postWorkNs);

                timingStats_.maxPostReceiveWorkNs =
                    std::max(
                        timingStats_.maxPostReceiveWorkNs,
                        duration);

                if (duration > 10'000)
                    ++timingStats_.postWorkOver10us;

                if (duration > 100'000)
                    ++timingStats_.postWorkOver100us;

                if (duration > 1'000'000)
                    ++timingStats_.postWorkOver1ms;
            }
        }

        running_.store(false, std::memory_order_release);
    }
    //

} // namespace llt::moldudp64
