#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <cstdint>
#include <limits>

#include "market_data/itch/ItchUdpCodec.h"
#include "market_data/itch/ItchUdpPacket.h"


TEST_CASE(
    "ITCH UDP codec round trips packet")
{
    llt::itch::ItchUdpPacket packet{};

    packet.sequence =
        123456789;

    packet.payloadSize =
        5;

    //
    // Deliberately arbitrary bytes.
    //
    // The transport layer must not care what
    // ITCH message type these represent.
    //
    packet.payload[0] =
        static_cast<std::uint8_t>('A');

    packet.payload[1] = 0x11;
    packet.payload[2] = 0x22;
    packet.payload[3] = 0x33;
    packet.payload[4] = 0x44;

    llt::itch::ItchUdpCodec::Datagram
        datagram{};

    std::size_t encodedSize{0};

    REQUIRE(
        llt::itch::ItchUdpCodec::encode(
            packet,
            datagram,
            encodedSize));

    REQUIRE(
        encodedSize ==
        llt::itch::ItchUdpCodec::HeaderSize +
        5);

    llt::itch::ItchUdpPacket decoded{};

    REQUIRE(
        llt::itch::ItchUdpCodec::decode(
            datagram.data(),
            encodedSize,
            decoded));

    REQUIRE(
        decoded.sequence ==
        packet.sequence);

    REQUIRE(
        decoded.payloadSize ==
        packet.payloadSize);

    for (
        std::size_t i = 0;
        i < packet.payloadSize;
        ++i)
    {
        REQUIRE(
            decoded.payload[i] ==
            packet.payload[i]);
    }
}


TEST_CASE(
    "ITCH UDP codec preserves maximum sequence")
{
    llt::itch::ItchUdpPacket packet{};

    packet.sequence =
        std::numeric_limits<
            std::uint64_t>::max();

    packet.payloadSize =
        1;

    packet.payload[0] =
        static_cast<std::uint8_t>('S');

    llt::itch::ItchUdpCodec::Datagram
        datagram{};

    std::size_t encodedSize{0};

    REQUIRE(
        llt::itch::ItchUdpCodec::encode(
            packet,
            datagram,
            encodedSize));

    llt::itch::ItchUdpPacket decoded{};

    REQUIRE(
        llt::itch::ItchUdpCodec::decode(
            datagram.data(),
            encodedSize,
            decoded));

    REQUIRE(
        decoded.sequence ==
        std::numeric_limits<
            std::uint64_t>::max());

    REQUIRE(decoded.payloadSize == 1);

    REQUIRE(
        decoded.payload[0] ==
        static_cast<std::uint8_t>('S'));
}


TEST_CASE(
    "ITCH UDP codec supports empty payload")
{
    llt::itch::ItchUdpPacket packet{};

    packet.sequence =
        42;

    packet.payloadSize =
        0;

    llt::itch::ItchUdpCodec::Datagram
        datagram{};

    std::size_t encodedSize{0};

    REQUIRE(
        llt::itch::ItchUdpCodec::encode(
            packet,
            datagram,
            encodedSize));

    REQUIRE(
        encodedSize ==
        llt::itch::ItchUdpCodec::HeaderSize);

    llt::itch::ItchUdpPacket decoded{};

    REQUIRE(
        llt::itch::ItchUdpCodec::decode(
            datagram.data(),
            encodedSize,
            decoded));

    REQUIRE(decoded.sequence == 42);
    REQUIRE(decoded.payloadSize == 0);
}


TEST_CASE(
    "ITCH UDP codec rejects truncated header")
{
    llt::itch::ItchUdpCodec::Datagram
        datagram{};

    llt::itch::ItchUdpPacket decoded{};

    REQUIRE_FALSE(
        llt::itch::ItchUdpCodec::decode(
            datagram.data(),
            llt::itch::ItchUdpCodec::HeaderSize - 1,
            decoded));
}


TEST_CASE(
    "ITCH UDP codec rejects truncated payload")
{
    llt::itch::ItchUdpPacket packet{};

    packet.sequence =
        100;

    packet.payloadSize =
        5;

    packet.payload[0] = 1;
    packet.payload[1] = 2;
    packet.payload[2] = 3;
    packet.payload[3] = 4;
    packet.payload[4] = 5;

    llt::itch::ItchUdpCodec::Datagram
        datagram{};

    std::size_t encodedSize{0};

    REQUIRE(
        llt::itch::ItchUdpCodec::encode(
            packet,
            datagram,
            encodedSize));

    llt::itch::ItchUdpPacket decoded{};

    REQUIRE_FALSE(
        llt::itch::ItchUdpCodec::decode(
            datagram.data(),
            encodedSize - 1,
            decoded));
}


TEST_CASE(
    "ITCH UDP codec rejects trailing bytes")
{
    llt::itch::ItchUdpPacket packet{};

    packet.sequence =
        100;

    packet.payloadSize =
        3;

    packet.payload[0] = 1;
    packet.payload[1] = 2;
    packet.payload[2] = 3;

    llt::itch::ItchUdpCodec::Datagram
        datagram{};

    std::size_t encodedSize{0};

    REQUIRE(
        llt::itch::ItchUdpCodec::encode(
            packet,
            datagram,
            encodedSize));

    //
    // Datagram buffer has spare capacity,
    // therefore pretending one extra byte was
    // received must fail exact framing.
    //
    llt::itch::ItchUdpPacket decoded{};

    REQUIRE_FALSE(
        llt::itch::ItchUdpCodec::decode(
            datagram.data(),
            encodedSize + 1,
            decoded));
}


TEST_CASE(
    "ITCH UDP codec rejects null input")
{
    llt::itch::ItchUdpPacket decoded{};

    REQUIRE_FALSE(
        llt::itch::ItchUdpCodec::decode(
            nullptr,
            0,
            decoded));
}


TEST_CASE(
    "ITCH UDP codec handles maximum payload")
{
    llt::itch::ItchUdpPacket packet{};

    packet.sequence =
        999;

    packet.payloadSize =
        static_cast<std::uint16_t>(
            llt::itch::ItchUdpPacket::
                MaxPayloadSize);

    for (
        std::size_t i = 0;
        i < packet.payloadSize;
        ++i)
    {
        packet.payload[i] =
            static_cast<std::uint8_t>(
                i & 0xFF);
    }

    llt::itch::ItchUdpCodec::Datagram
        datagram{};

    std::size_t encodedSize{0};

    REQUIRE(
        llt::itch::ItchUdpCodec::encode(
            packet,
            datagram,
            encodedSize));

    REQUIRE(
        encodedSize ==
        llt::itch::ItchUdpCodec::
            MaxDatagramSize);

    llt::itch::ItchUdpPacket decoded{};

    REQUIRE(
        llt::itch::ItchUdpCodec::decode(
            datagram.data(),
            encodedSize,
            decoded));

    REQUIRE(decoded.sequence == 999);

    REQUIRE(
        decoded.payloadSize ==
        packet.payloadSize);

    for (
        std::size_t i = 0;
        i < packet.payloadSize;
        ++i)
    {
        REQUIRE(
            decoded.payload[i] ==
            packet.payload[i]);
    }
}