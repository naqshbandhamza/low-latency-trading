#pragma once

#include <cstddef>

#include "logging/ILogger.h"
#include "market_data/IMarketDataSource.h"
#include "market_data/MarketEvent.h"
#include "ring_buffer/SpscRingBuffer.h"

namespace llt
{

using MarketEventQueue =
    SpscRingBuffer<MarketEvent, 4096>;

class FeedHandler
{
public:

    FeedHandler(
        ILogger& logger,
        MarketEventQueue& queue,
        IMarketDataSource& source
    ) noexcept;

    void start(
        std::size_t eventCount
    );

private:

    MarketEvent createMarketEvent(
        const MarketDataMessage& message
    );

private:

    ILogger& logger_;
    MarketEventQueue& queue_;
    IMarketDataSource& source_;
};

} // namespace llt