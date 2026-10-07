#include <catch2/catch_test_macros.hpp>

#include <array>
#include <cstddef>
#include <cstdint>

#include "market_data/moldudp64/MoldUdp64.h"
#include "market_data/moldudp64/MoldUdp64Codec.h"

TEST_CASE(
    "MoldUDP64 encodes and decodes multiple messages")
{
    using namespace llt::moldudp64;

    Session session{
        '2', '0', '1', '9', '0',
        '1', '3', '0', 'A', ' '};

    Datagram datagram{};

    std::size_t encodedSize{0};

    std::uint16_t messageCount{0};

    REQUIRE(
        MoldUdp64Codec::beginPacket(
            session,
            100,
            datagram,
            encodedSize));

    const std::array<std::uint8_t, 3>
        first{
            0x41,
            0x11,
            0x22};

    const std::array<std::uint8_t, 4>
        second{
            0x54,
            0x33,
            0x44,
            0x55};

    REQUIRE(
        MoldUdp64Codec::appendMessage(
            first.data(),
            first.size(),
            datagram,
            encodedSize,
            messageCount));

    REQUIRE(
        MoldUdp64Codec::appendMessage(
            second.data(),
            second.size(),
            datagram,
            encodedSize,
            messageCount));

    REQUIRE(
        messageCount == 2);

    MoldUdp64Codec::finalizePacket(
        datagram,
        messageCount);

    Header header{};

    REQUIRE(
        MoldUdp64Codec::decodeHeader(
            datagram.data(),
            encodedSize,
            header));

    REQUIRE(
        header.session ==
        session);

    REQUIRE(
        header.sequenceNumber ==
        100);

    REQUIRE(
        header.messageCount ==
        2);

    std::size_t offset =
        HeaderSize;

    MessageView firstDecoded{};

    REQUIRE(
        MoldUdp64Codec::nextMessage(
            datagram.data(),
            encodedSize,
            offset,
            header.sequenceNumber,
            firstDecoded));

    REQUIRE(
        firstDecoded.sequenceNumber ==
        100);

    REQUIRE(
        firstDecoded.size ==
        first.size());

    REQUIRE(
        firstDecoded.data[0] ==
        0x41);

    MessageView secondDecoded{};

    REQUIRE(
        MoldUdp64Codec::nextMessage(
            datagram.data(),
            encodedSize,
            offset,
            header.sequenceNumber + 1,
            secondDecoded));

    REQUIRE(
        secondDecoded.sequenceNumber ==
        101);

    REQUIRE(
        secondDecoded.size ==
        second.size());

    REQUIRE(
        secondDecoded.data[0] ==
        0x54);

    REQUIRE(
        offset ==
        encodedSize);
}