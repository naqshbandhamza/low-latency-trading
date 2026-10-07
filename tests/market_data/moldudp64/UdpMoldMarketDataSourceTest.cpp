#include <catch2/catch_test_macros.hpp>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <cstddef>
#include <cstdint>

#include "market_data/moldudp64/MoldUdp64.h"
#include "market_data/moldudp64/MoldUdp64Codec.h"
#include "market_data/moldudp64/UdpMoldMarketDataSource.h"


TEST_CASE(
    "UdpMoldMarketDataSource receives MoldUDP64 datagram")
{
    using namespace llt::moldudp64;


    constexpr std::uint16_t port =
        19002;


    //
    // Receiver
    //
    UdpMoldMarketDataSource source{
        port,
        1000};


    //
    // Build a real MoldUDP64-formatted datagram.
    //
    Session session{
        'T', 'E', 'S', 'T', 'S',
        'E', 'S', 'S', '0', '1'};


    Datagram outgoing{};

    std::size_t encodedSize{0};

    std::uint16_t messageCount{0};


    REQUIRE(
        MoldUdp64Codec::beginPacket(
            session,
            500,
            outgoing,
            encodedSize));


    const std::array<
        std::uint8_t,
        3>
        firstMessage{
            0x41,
            0x11,
            0x22};


    const std::array<
        std::uint8_t,
        4>
        secondMessage{
            0x54,
            0x33,
            0x44,
            0x55};


    REQUIRE(
        MoldUdp64Codec::appendMessage(
            firstMessage.data(),
            firstMessage.size(),
            outgoing,
            encodedSize,
            messageCount));


    REQUIRE(
        MoldUdp64Codec::appendMessage(
            secondMessage.data(),
            secondMessage.size(),
            outgoing,
            encodedSize,
            messageCount));


    MoldUdp64Codec::finalizePacket(
        outgoing,
        messageCount);


    REQUIRE(
        messageCount ==
        2);


    //
    // Sender socket
    //
    const int sender =
        ::socket(
            AF_INET,
            SOCK_DGRAM,
            0);


    REQUIRE(
        sender >= 0);


    sockaddr_in destination{};

    destination.sin_family =
        AF_INET;

    destination.sin_addr.s_addr =
        htonl(
            INADDR_LOOPBACK);

    destination.sin_port =
        htons(
            port);


    const auto sent =
        ::sendto(
            sender,
            outgoing.data(),
            encodedSize,
            0,
            reinterpret_cast<
                const sockaddr*>(
                    &destination),
            sizeof(destination));


    REQUIRE(
        sent ==
        static_cast<ssize_t>(
            encodedSize));


    //
    // Receive raw MoldUDP64 datagram.
    //
    ReceivedMoldDatagram incoming{};


    REQUIRE(
        source.receive(
            incoming));


    REQUIRE(
        incoming.size ==
        encodedSize);


    //
    // Decode MoldUDP64 header.
    //
    Header header{};


    REQUIRE(
        MoldUdp64Codec::decodeHeader(
            incoming.bytes.data(),
            incoming.size,
            header));


    REQUIRE(
        header.session ==
        session);


    REQUIRE(
        header.sequenceNumber ==
        500);


    REQUIRE(
        header.messageCount ==
        2);


    //
    // Decode message #1.
    //
    std::size_t offset =
        HeaderSize;


    MessageView firstDecoded{};


    REQUIRE(
        MoldUdp64Codec::nextMessage(
            incoming.bytes.data(),
            incoming.size,
            offset,
            header.sequenceNumber,
            firstDecoded));


    REQUIRE(
        firstDecoded.sequenceNumber ==
        500);


    REQUIRE(
        firstDecoded.size ==
        firstMessage.size());


    REQUIRE(
        firstDecoded.data[0] ==
        0x41);


    REQUIRE(
        firstDecoded.data[1] ==
        0x11);


    REQUIRE(
        firstDecoded.data[2] ==
        0x22);


    //
    // Decode message #2.
    //
    MessageView secondDecoded{};


    REQUIRE(
        MoldUdp64Codec::nextMessage(
            incoming.bytes.data(),
            incoming.size,
            offset,
            header.sequenceNumber + 1,
            secondDecoded));


    REQUIRE(
        secondDecoded.sequenceNumber ==
        501);


    REQUIRE(
        secondDecoded.size ==
        secondMessage.size());


    REQUIRE(
        secondDecoded.data[0] ==
        0x54);


    REQUIRE(
        secondDecoded.data[1] ==
        0x33);


    REQUIRE(
        secondDecoded.data[2] ==
        0x44);


    REQUIRE(
        secondDecoded.data[3] ==
        0x55);


    //
    // Entire datagram must have been consumed.
    //
    REQUIRE(
        offset ==
        incoming.size);


    ::close(
        sender);
}