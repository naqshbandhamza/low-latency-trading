#pragma once

#include <cstdint>

namespace llt
{

enum class UdpMarketDataPacketType : std::uint8_t
{
    Quote = 0,
    Trade = 1
};

enum class UdpMarketDataPacketSide : std::uint8_t
{
    Buy = 0,
    Sell = 1
};

struct UdpMarketDataPacket
{
    UdpMarketDataPacketType type;

    std::uint64_t sequence;

    std::uint64_t timestamp;

    std::int64_t price;

    std::uint64_t quantity;

    std::int64_t bidPrice;

    std::uint64_t bidQuantity;

    std::int64_t askPrice;

    std::uint64_t askQuantity;

    UdpMarketDataPacketSide side;
};

} // namespace llt