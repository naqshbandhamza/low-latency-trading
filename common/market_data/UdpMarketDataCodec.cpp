#include "market_data/UdpMarketDataCodec.h"

namespace llt
{

namespace
{

void writeUint64(
    std::uint8_t* destination,
    std::uint64_t value
) noexcept
{
    for (int i = 7; i >= 0; --i)
    {
        destination[7 - i] =
            static_cast<std::uint8_t>(
                (value >> (i * 8)) & 0xFF
            );
    }
}

std::uint64_t readUint64(
    const std::uint8_t* source
) noexcept
{
    std::uint64_t value = 0;

    for (int i = 0; i < 8; ++i)
    {
        value =
            (value << 8)
            | static_cast<std::uint64_t>(source[i]);
    }

    return value;
}

void writeInt64(
    std::uint8_t* destination,
    std::int64_t value
) noexcept
{
    writeUint64(
        destination,
        static_cast<std::uint64_t>(value)
    );
}

std::int64_t readInt64(
    const std::uint8_t* source
) noexcept
{
    return static_cast<std::int64_t>(
        readUint64(source)
    );
}

} // namespace

UdpMarketDataCodec::Buffer
UdpMarketDataCodec::encode(
    const UdpMarketDataPacket& packet
) noexcept
{
    Buffer buffer{};

    buffer[0] =
        static_cast<std::uint8_t>(packet.type);

    writeUint64(
        buffer.data() + 1,
        packet.sequence
    );

    writeUint64(
        buffer.data() + 9,
        packet.timestamp
    );

    writeInt64(
        buffer.data() + 17,
        packet.price
    );

    writeUint64(
        buffer.data() + 25,
        packet.quantity
    );

    writeInt64(
        buffer.data() + 33,
        packet.bidPrice
    );

    writeUint64(
        buffer.data() + 41,
        packet.bidQuantity
    );

    writeInt64(
        buffer.data() + 49,
        packet.askPrice
    );

    writeUint64(
        buffer.data() + 57,
        packet.askQuantity
    );

    buffer[65] =
        static_cast<std::uint8_t>(packet.side);

    return buffer;
}

bool UdpMarketDataCodec::decode(
    const std::uint8_t* data,
    std::size_t size,
    UdpMarketDataPacket& packet
) noexcept
{
    if (data == nullptr || size != WireSize)
    {
        return false;
    }

    const auto type =
        static_cast<UdpMarketDataPacketType>(data[0]);

    const auto side =
        static_cast<UdpMarketDataPacketSide>(data[65]);

    if (
        type != UdpMarketDataPacketType::Quote
        && type != UdpMarketDataPacketType::Trade
    )
    {
        return false;
    }

    if (
        side != UdpMarketDataPacketSide::Buy
        && side != UdpMarketDataPacketSide::Sell
    )
    {
        return false;
    }

    packet.type = type;

    packet.sequence =
        readUint64(data + 1);

    packet.timestamp =
        readUint64(data + 9);

    packet.price =
        readInt64(data + 17);

    packet.quantity =
        readUint64(data + 25);

    packet.bidPrice =
        readInt64(data + 33);

    packet.bidQuantity =
        readUint64(data + 41);

    packet.askPrice =
        readInt64(data + 49);

    packet.askQuantity =
        readUint64(data + 57);

    packet.side = side;

    return true;
}

} // namespace llt