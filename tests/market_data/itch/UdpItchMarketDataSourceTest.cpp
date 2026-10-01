#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstddef>
#include <cstdint>

#include <catch2/catch_test_macros.hpp>

#include "market_data/itch/ItchUdpCodec.h"
#include "market_data/itch/ItchUdpPacket.h"
#include "market_data/itch/UdpItchMarketDataSource.h"

namespace
{

int createSender()
{
    return ::socket(
        AF_INET,
        SOCK_DGRAM,
        0);
}


bool sendPacket(
    int socketFd,
    std::uint16_t port,
    const llt::itch::ItchUdpPacket& packet)
{
    llt::itch::ItchUdpCodec::Datagram
        datagram{};

    std::size_t encodedSize{0};

    if (
        !llt::itch::ItchUdpCodec::encode(
            packet,
            datagram,
            encodedSize))
    {
        return false;
    }

    sockaddr_in address{};

    address.sin_family =
        AF_INET;

    address.sin_addr.s_addr =
        htonl(INADDR_LOOPBACK);

    address.sin_port =
        htons(port);

    const auto sent =
        ::sendto(
            socketFd,
            datagram.data(),
            encodedSize,
            0,
            reinterpret_cast<
                const sockaddr*>(
                    &address),
            sizeof(address));

    return
        sent ==
        static_cast<ssize_t>(
            encodedSize);
}

} // namespace


TEST_CASE(
    "UDP ITCH market data source receives packet")
{
    constexpr std::uint16_t port =
        19100;

    constexpr std::uint32_t timeoutMs =
        100;

    llt::itch::UdpItchMarketDataSource
        source{
            port,
            timeoutMs};

    const int sender =
        createSender();

    REQUIRE(sender >= 0);

    llt::itch::ItchUdpPacket outgoing{};

    outgoing.sequence =
        1001;

    outgoing.payloadSize =
        5;

    outgoing.payload[0] =
        static_cast<std::uint8_t>('A');

    outgoing.payload[1] = 0x11;
    outgoing.payload[2] = 0x22;
    outgoing.payload[3] = 0x33;
    outgoing.payload[4] = 0x44;

    REQUIRE(
        sendPacket(
            sender,
            port,
            outgoing));

    llt::itch::ItchUdpPacket incoming{};

    REQUIRE(
        source.receive(
            incoming));

    ::close(sender);

    REQUIRE(
        incoming.sequence ==
        1001);

    REQUIRE(
        incoming.payloadSize ==
        5);

    REQUIRE(
        incoming.payload[0] ==
        static_cast<std::uint8_t>('A'));

    REQUIRE(incoming.payload[1] == 0x11);
    REQUIRE(incoming.payload[2] == 0x22);
    REQUIRE(incoming.payload[3] == 0x33);
    REQUIRE(incoming.payload[4] == 0x44);
}


TEST_CASE(
    "UDP ITCH market data source returns false on timeout")
{
    constexpr std::uint16_t port =
        19101;

    constexpr std::uint32_t timeoutMs =
        10;

    llt::itch::UdpItchMarketDataSource
        source{
            port,
            timeoutMs};

    llt::itch::ItchUdpPacket packet{};

    REQUIRE_FALSE(
        source.receive(
            packet));
}


TEST_CASE(
    "UDP ITCH market data source rejects malformed datagram")
{
    constexpr std::uint16_t port =
        19102;

    constexpr std::uint32_t timeoutMs =
        100;

    llt::itch::UdpItchMarketDataSource
        source{
            port,
            timeoutMs};

    const int sender =
        createSender();

    REQUIRE(sender >= 0);

    //
    // Not a valid ItchUdpCodec datagram.
    //
    const std::uint8_t malformed[] = {
        0x01,
        0x02,
        0x03,
        0x04
    };

    sockaddr_in address{};

    address.sin_family =
        AF_INET;

    address.sin_addr.s_addr =
        htonl(INADDR_LOOPBACK);

    address.sin_port =
        htons(port);

    const auto sent =
        ::sendto(
            sender,
            malformed,
            sizeof(malformed),
            0,
            reinterpret_cast<
                const sockaddr*>(
                    &address),
            sizeof(address));

    REQUIRE(
        sent ==
        static_cast<ssize_t>(
            sizeof(malformed)));

    llt::itch::ItchUdpPacket packet{};

    REQUIRE_FALSE(
        source.receive(
            packet));

    ::close(sender);
}