#pragma once

#include <cstddef>

#include "ring_buffer/SpscRingBuffer.h"
#include "market_data/moldudp64/MoldUdp64.h"

namespace llt::moldudp64
{

inline constexpr std::size_t
    MoldDatagramQueueCapacity = 65536;

using MoldDatagramQueue =
    llt::SpscRingBuffer<
        ReceivedMoldDatagram,
        MoldDatagramQueueCapacity>;

} // namespace llt::moldudp64