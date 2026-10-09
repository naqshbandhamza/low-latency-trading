
#include "market_data/moldudp64/MoldUdpReceiver.h"

#include <cstddef>
#include <stdexcept>

namespace llt::moldudp64
{

MoldUdpReceiver::MoldUdpReceiver(
    IMoldMarketDataSource& source,
    MoldDatagramQueue& queue) noexcept
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

void MoldUdpReceiver::receiveLoop() noexcept
{
    ReceivedMoldDatagram datagram{};

    while (running_.load(std::memory_order_acquire))
    {
        if (!source_.receive(datagram))
        {
            continue;
        }

        receivedDatagrams_.fetch_add(
            1, std::memory_order_relaxed);

        if (!queue_.push(datagram))
        {
            queueFullDrops_.fetch_add(
                1, std::memory_order_relaxed);

            continue;
        }

        queuedDatagrams_.fetch_add(
            1, std::memory_order_relaxed);

        const auto occupancy =
            static_cast<std::uint64_t>(queue_.size());

        auto previousMax =
            maxQueueOccupancy_.load(std::memory_order_relaxed);

        while (occupancy > previousMax &&
               !maxQueueOccupancy_.compare_exchange_weak(
                   previousMax,
                   occupancy,
                   std::memory_order_relaxed,
                   std::memory_order_relaxed))
        {
        }
    }

    running_.store(false, std::memory_order_release);
}

} // namespace llt::moldudp64
