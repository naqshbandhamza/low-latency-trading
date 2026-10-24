
#include "market_data/moldudp64/MoldUdpReceiver.h"
#include <chrono>
#include <cstddef>
#include <stdexcept>

#include <algorithm>
#include <cstdint>

#ifdef LLT_ENABLE_LATENCY_TRACE
#include "market_data/utility/LatencyClock.h"
#endif

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
        timingStats_ = ReceiverTimingStats{};

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

    void MoldUdpReceiver::receiveLoop() noexcept
    {
        // Used only when the ring is full. Still drain the socket so
        // queue-full drops are counted and the receive path keeps moving.
        ReceivedMoldDatagram discard{};
        using Result = MoldDatagramQueue::ProduceResult;

#ifdef LLT_ENABLE_RECEIVER_DIAGNOSTICS
        using Clock = std::chrono::steady_clock;
        using Ns = std::chrono::nanoseconds;
        auto previousLoopStart = Clock::now();
        auto previousReceiveEnd = Clock::time_point{};
        bool havePreviousReceive = false;
        const auto elapsed = [](Clock::time_point a, Clock::time_point b)
            -> std::uint64_t {
            return static_cast<std::uint64_t>(
                std::chrono::duration_cast<Ns>(b - a).count());
        };
#endif

        while (running_.load(std::memory_order_acquire))
        {
#ifdef LLT_ENABLE_RECEIVER_DIAGNOSTICS
            const auto loopStart = Clock::now();
            const auto loopGap = elapsed(previousLoopStart, loopStart);
            previousLoopStart = loopStart;
            timingStats_.maxLoopGapNs =
                std::max(timingStats_.maxLoopGapNs, loopGap);
            if (loopGap > 100'000) ++timingStats_.loopGapsOver100us;
            if (loopGap > 500'000) ++timingStats_.loopGapsOver500us;
            if (loopGap > 1'000'000) ++timingStats_.loopGapsOver1ms;
            const auto receiveStart = Clock::now();
            if (havePreviousReceive)
            {
                const auto outside = elapsed(previousReceiveEnd, receiveStart);
                timingStats_.maxOutsideReceiveNs =
                    std::max(timingStats_.maxOutsideReceiveNs, outside);
                if (outside > 100'000) ++timingStats_.outsideReceiveOver100us;
                if (outside > 1'000'000) ++timingStats_.outsideReceiveOver1ms;
            }
#endif
            // Direct receive into an unpublished SPSC slot. On failure,
            // try_produce destroys the slot without advancing writeIndex.
            // The callback must not throw.
            const Result result = queue_.try_produce(
                [this](ReceivedMoldDatagram& slot) noexcept -> bool {
                    if (!source_.receive(slot))
                        return false;
#ifdef LLT_ENABLE_LATENCY_TRACE
                    slot.queuedAtNs = llt::latency::nowNs();
#endif
                    return true;
                });

            bool received = (result == Result::Published);
            bool queued = received;
            if (result == Result::Full)
            {
                // No free slot: preserve old behavior by receiving into
                // a scratch datagram and counting a drop on success.
                received = source_.receive(discard);
            }

#ifdef LLT_ENABLE_RECEIVER_DIAGNOSTICS
            const auto receiveEnd = Clock::now();
            previousReceiveEnd = receiveEnd;
            havePreviousReceive = true;
            const auto callDuration = elapsed(receiveStart, receiveEnd);
            // Includes slot acquisition/publication, not just recvmsg().
            timingStats_.maxReceiveCallNs =
                std::max(timingStats_.maxReceiveCallNs, callDuration);
            if (received && callDuration > 1'000'000)
                ++timingStats_.successfulReceivesOver1ms;
#endif
            if (!received)
            {
#ifdef LLT_ENABLE_RECEIVER_DIAGNOSTICS
                ++timingStats_.unsuccessfulReceiveCalls;
#endif
                continue;
            }

            receivedDatagrams_.fetch_add(1, std::memory_order_relaxed);
            if (queued)
                queuedDatagrams_.fetch_add(1, std::memory_order_relaxed);
            else
                queueFullDrops_.fetch_add(1, std::memory_order_relaxed);

#ifdef LLT_ENABLE_RECEIVER_DIAGNOSTICS
            const auto postWork = elapsed(receiveEnd, Clock::now());
            timingStats_.maxPostReceiveWorkNs =
                std::max(timingStats_.maxPostReceiveWorkNs, postWork);
            if (postWork > 10'000) ++timingStats_.postWorkOver10us;
            if (postWork > 100'000) ++timingStats_.postWorkOver100us;
            if (postWork > 1'000'000) ++timingStats_.postWorkOver1ms;
            // Old 'push' measurements do not apply: receiving and
            // publishing are now fused into a single operation.
#endif
        }
        running_.store(false, std::memory_order_release);
    }

} // namespace llt::moldudp64
