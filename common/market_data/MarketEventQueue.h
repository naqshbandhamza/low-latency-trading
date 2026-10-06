#pragma once

#include "market_data/MarketEvent.h"
#include "ring_buffer/SpscRingBuffer.h"

namespace llt
{

using MarketEventQueue =
    SpscRingBuffer<
        MarketEvent,
        65536
    >;

} // namespace llt