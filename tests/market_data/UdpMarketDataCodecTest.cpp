#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "market_data/UdpMarketDataCodec.h"

namespace
{

llt::UdpMarketDataPacket makeQuotePacket()
{
    llt::UdpMarketDataPacket packet{};

    packet.type =
        llt::UdpMarketDataPacketType::Quote;

    packet.sequence = 123456;
    packet.timestamp = 987654321;

    packet.price = 234500;
    packet.quantity = 100;

    packet.bidPrice = 234490;
    packet.bidQuantity = 50;

    packet.askPrice = 234510;
    packet.askQuantity = 75;

    packet.side =
        llt::UdpMarketDataPacketSide::Buy;

    return packet;
}

llt::UdpMarketDataPacket makeTradePacket()
{
    llt::UdpMarketDataPacket packet{};

    packet.type =
        llt::UdpMarketDataPacketType::Trade;

    packet.sequence = 987654;
    packet.timestamp = 123456789;

    packet.price = 234505;
    packet.quantity = 25;

    packet.bidPrice = 234500;
    packet.bidQuantity = 40;

    packet.askPrice = 234510;
    packet.askQuantity = 60;

    packet.side =
        llt::UdpMarketDataPacketSide::Sell;

    return packet;
}

} // namespace


TEST_CASE("UDP codec encodes and decodes quote packet")
{
    const auto original =
        makeQuotePacket();

    const auto buffer =
        llt::UdpMarketDataCodec::encode(
            original
        );

    llt::UdpMarketDataPacket decoded{};

    REQUIRE(
        llt::UdpMarketDataCodec::decode(
            buffer.data(),
            buffer.size(),
            decoded
        )
    );

    REQUIRE(decoded.type == original.type);
    REQUIRE(decoded.sequence == original.sequence);
    REQUIRE(decoded.timestamp == original.timestamp);
    REQUIRE(decoded.price == original.price);
    REQUIRE(decoded.quantity == original.quantity);
    REQUIRE(decoded.bidPrice == original.bidPrice);
    REQUIRE(decoded.bidQuantity == original.bidQuantity);
    REQUIRE(decoded.askPrice == original.askPrice);
    REQUIRE(decoded.askQuantity == original.askQuantity);
    REQUIRE(decoded.side == original.side);
}


TEST_CASE("UDP codec encodes and decodes trade packet")
{
    const auto original =
        makeTradePacket();

    const auto buffer =
        llt::UdpMarketDataCodec::encode(
            original
        );

    llt::UdpMarketDataPacket decoded{};

    REQUIRE(
        llt::UdpMarketDataCodec::decode(
            buffer.data(),
            buffer.size(),
            decoded
        )
    );

    REQUIRE(decoded.type == original.type);
    REQUIRE(decoded.sequence == original.sequence);
    REQUIRE(decoded.timestamp == original.timestamp);
    REQUIRE(decoded.price == original.price);
    REQUIRE(decoded.quantity == original.quantity);
    REQUIRE(decoded.bidPrice == original.bidPrice);
    REQUIRE(decoded.bidQuantity == original.bidQuantity);
    REQUIRE(decoded.askPrice == original.askPrice);
    REQUIRE(decoded.askQuantity == original.askQuantity);
    REQUIRE(decoded.side == original.side);
}


TEST_CASE("UDP codec preserves negative prices")
{
    auto original =
        makeQuotePacket();

    original.price = -123456789;
    original.bidPrice = -234500;
    original.askPrice = -234400;

    const auto buffer =
        llt::UdpMarketDataCodec::encode(
            original
        );

    llt::UdpMarketDataPacket decoded{};

    REQUIRE(
        llt::UdpMarketDataCodec::decode(
            buffer.data(),
            buffer.size(),
            decoded
        )
    );

    REQUIRE(decoded.price == original.price);
    REQUIRE(decoded.bidPrice == original.bidPrice);
    REQUIRE(decoded.askPrice == original.askPrice);
}


TEST_CASE("UDP codec preserves maximum integer values")
{
    auto original =
        makeQuotePacket();

    original.sequence =
        std::numeric_limits<std::uint64_t>::max();

    original.timestamp =
        std::numeric_limits<std::uint64_t>::max();

    original.quantity =
        std::numeric_limits<std::uint64_t>::max();

    original.bidQuantity =
        std::numeric_limits<std::uint64_t>::max();

    original.askQuantity =
        std::numeric_limits<std::uint64_t>::max();

    original.price =
        std::numeric_limits<std::int64_t>::max();

    original.bidPrice =
        std::numeric_limits<std::int64_t>::max();

    original.askPrice =
        std::numeric_limits<std::int64_t>::max();

    const auto buffer =
        llt::UdpMarketDataCodec::encode(
            original
        );

    llt::UdpMarketDataPacket decoded{};

    REQUIRE(
        llt::UdpMarketDataCodec::decode(
            buffer.data(),
            buffer.size(),
            decoded
        )
    );

    REQUIRE(decoded.sequence == original.sequence);
    REQUIRE(decoded.timestamp == original.timestamp);
    REQUIRE(decoded.quantity == original.quantity);
    REQUIRE(decoded.bidQuantity == original.bidQuantity);
    REQUIRE(decoded.askQuantity == original.askQuantity);
    REQUIRE(decoded.price == original.price);
    REQUIRE(decoded.bidPrice == original.bidPrice);
    REQUIRE(decoded.askPrice == original.askPrice);
}


TEST_CASE("UDP codec preserves minimum int64 price")
{
    auto original =
        makeQuotePacket();

    original.price =
        std::numeric_limits<std::int64_t>::min();

    original.bidPrice =
        std::numeric_limits<std::int64_t>::min();

    original.askPrice =
        std::numeric_limits<std::int64_t>::min();

    const auto buffer =
        llt::UdpMarketDataCodec::encode(
            original
        );

    llt::UdpMarketDataPacket decoded{};

    REQUIRE(
        llt::UdpMarketDataCodec::decode(
            buffer.data(),
            buffer.size(),
            decoded
        )
    );

    REQUIRE(decoded.price == original.price);
    REQUIRE(decoded.bidPrice == original.bidPrice);
    REQUIRE(decoded.askPrice == original.askPrice);
}


TEST_CASE("UDP codec rejects truncated packet")
{
    const auto original =
        makeQuotePacket();

    const auto buffer =
        llt::UdpMarketDataCodec::encode(
            original
        );

    llt::UdpMarketDataPacket decoded{};

    REQUIRE(
        !llt::UdpMarketDataCodec::decode(
            buffer.data(),
            buffer.size() - 1,
            decoded
        )
    );
}


TEST_CASE("UDP codec rejects invalid packet type")
{
    const auto original =
        makeQuotePacket();

    auto buffer =
        llt::UdpMarketDataCodec::encode(
            original
        );

    buffer[0] = 2;

    llt::UdpMarketDataPacket decoded{};

    REQUIRE(
        !llt::UdpMarketDataCodec::decode(
            buffer.data(),
            buffer.size(),
            decoded
        )
    );
}


TEST_CASE("UDP codec rejects invalid packet side")
{
    const auto original =
        makeQuotePacket();

    auto buffer =
        llt::UdpMarketDataCodec::encode(
            original
        );

    buffer[65] = 2;

    llt::UdpMarketDataPacket decoded{};

    REQUIRE(
        !llt::UdpMarketDataCodec::decode(
            buffer.data(),
            buffer.size(),
            decoded
        )
    );
}

