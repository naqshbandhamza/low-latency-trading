
#pragma once

#include <atomic>
#include <cstdint>
#include <thread>

#include "market_data/moldudp64/IMoldMarketDataSource.h"
#include "market_data/moldudp64/MoldDatagramQueue.h"

namespace llt::moldudp64
{

class MoldUdpReceiver final
{
public:
    MoldUdpReceiver(
        IMoldMarketDataSource& source,
        MoldDatagramQueue& queue) noexcept;

    ~MoldUdpReceiver();

    MoldUdpReceiver(const MoldUdpReceiver&) = delete;
    MoldUdpReceiver& operator=(const MoldUdpReceiver&) = delete;

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

private:
    void receiveLoop() noexcept;

    IMoldMarketDataSource& source_;
    MoldDatagramQueue& queue_;

    std::thread worker_;

    std::atomic<bool> running_{false};

    std::atomic<std::uint64_t> receivedDatagrams_{0};
    std::atomic<std::uint64_t> queuedDatagrams_{0};
    std::atomic<std::uint64_t> queueFullDrops_{0};
    std::atomic<std::uint64_t> maxQueueOccupancy_{0};
};

} // namespace llt::moldudp64
