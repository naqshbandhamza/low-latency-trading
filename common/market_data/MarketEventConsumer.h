#pragma once

#include <atomic>
#include <cstdint>

#include "market_data/MarketEventQueue.h"

namespace llt
{

class MarketEventConsumer
{
public:
    explicit MarketEventConsumer(
        MarketEventQueue &queue) noexcept
        : queue_(queue)
    {
    }

    void run(
        const std::atomic<bool> &producerDone) noexcept
    {
        //
        // Continue until:
        //
        // 1. producer has finished
        // 2. AND everything already published has
        //    been consumed.
        //
        while (
            !producerDone.load(
                std::memory_order_acquire) ||
            !queue_.empty())
        {
            auto event =
                queue_.pop();

            if (!event.has_value())
            {
                continue;
            }

            ++consumedEvents_;
        }
    }

    [[nodiscard]]
    std::uint64_t consumedEvents() const noexcept
    {
        return consumedEvents_;
    }

private:
    MarketEventQueue &queue_;

    std::uint64_t consumedEvents_{0};
};

} // namespace llt