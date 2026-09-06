#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "market_data/UdpMarketDataPacket.h"

namespace llt
{

class UdpMarketDataCodec
{
public:

    static constexpr std::size_t WireSize = 66;

    using Buffer =
        std::array<std::uint8_t, WireSize>;

    static Buffer encode(
        const UdpMarketDataPacket& packet
    ) noexcept;

    static bool decode(
        const std::uint8_t* data,
        std::size_t size,
        UdpMarketDataPacket& packet
    ) noexcept;
};

} // namespace llt