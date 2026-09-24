#pragma once

#include "ring_buffer/SpscRingBuffer.h"
#include "strategy/OrderIntent.h"

namespace llt
{

using OrderIntentQueue =
    SpscRingBuffer<
        OrderIntent,
        4096
    >;

} // namespace llt