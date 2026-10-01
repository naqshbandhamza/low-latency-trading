#include <catch2/catch_test_macros.hpp>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstddef>
#include <cstdint>
#include <vector>

#include "market_data/itch/IItchRecoverySource.h"
#include "ItchFeedHandler.h"
#include "market_data/itch/ItchMarketState.h"
#include "market_data/itch/ItchSequenceRecovery.h"
#include "market_data/itch/ItchUdpCodec.h"
#include "market_data/itch/ItchUdpPacket.h"
#include "market_data/itch/UdpItchMarketDataSource.h"

namespace
{

//
// Deterministic recovery backend.
//
// This is NOT mocking ItchSequenceRecovery.
//
// The real ItchSequenceRecovery is under test.
// This object represents the external historical
// packet store/recovery channel from which it asks
// for missing transport packets.
//
class TestRecoverySource final
    : public llt::itch::IItchRecoverySource
{
public:
    std::vector<llt::itch::ItchUdpPacket>
        availablePackets{};

    std::size_t callCount{0};

    std::uint64_t lastFromSequence{0};

    std::uint64_t lastToSequence{0};

    bool recover(
        std::uint64_t fromSequence,
        std::uint64_t toSequence,
        std::vector<
            llt::itch::ItchUdpPacket>& packets
    ) override
    {
        ++callCount;

        lastFromSequence =
            fromSequence;

        lastToSequence =
            toSequence;

        packets.clear();

        for (
            const auto& packet :
            availablePackets)
        {
            if (
                packet.sequence >= fromSequence &&
                packet.sequence < toSequence)
            {
                packets.push_back(
                    packet);
            }
        }

        return true;
    }
};


llt::itch::ItchUdpPacket
makeSystemEventPacket(
    std::uint64_t transportSequence,
    char eventCode = 'O')
{
    llt::itch::ItchUdpPacket packet{};

    packet.sequence =
        transportSequence;

    //
    // ITCH 5.0 System Event:
    //
    // 1  byte  type
    // 2  bytes stock locate
    // 2  bytes tracking number
    // 6  bytes timestamp
    // 1  byte  event code
    //
    packet.payloadSize =
        12;

    packet.payload[0] =
        static_cast<std::uint8_t>('S');

    //
    // Stock locate.
    //
    packet.payload[1] = 0;
    packet.payload[2] = 0;

    //
    // Tracking number.
    //
    packet.payload[3] = 0;
    packet.payload[4] = 0;

    //
    // 48-bit timestamp.
    //
    packet.payload[5] =
        static_cast<std::uint8_t>(
            (transportSequence >> 40) &
            0xFF);

    packet.payload[6] =
        static_cast<std::uint8_t>(
            (transportSequence >> 32) &
            0xFF);

    packet.payload[7] =
        static_cast<std::uint8_t>(
            (transportSequence >> 24) &
            0xFF);

    packet.payload[8] =
        static_cast<std::uint8_t>(
            (transportSequence >> 16) &
            0xFF);

    packet.payload[9] =
        static_cast<std::uint8_t>(
            (transportSequence >> 8) &
            0xFF);

    packet.payload[10] =
        static_cast<std::uint8_t>(
            transportSequence &
            0xFF);

    packet.payload[11] =
        static_cast<std::uint8_t>(
            eventCode);

    return packet;
}


int createUdpSender()
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
    "UDP ITCH feed recovers transport gap through real sequence recovery")
{
    constexpr std::uint16_t port =
        19200;

    constexpr std::uint32_t timeoutMs =
        100;

    //
    // Real UDP receiver.
    //
    llt::itch::UdpItchMarketDataSource
        source{
            port,
            timeoutMs};

    //
    // Controlled historical source.
    //
    TestRecoverySource recoverySource;

    recoverySource.availablePackets = {
        makeSystemEventPacket(102),
        makeSystemEventPacket(103)
    };

    //
    // REAL recovery implementation.
    //
    llt::itch::ItchSequenceRecovery
        recovery{
            recoverySource};

    //
    // REAL ITCH market state.
    //
    llt::itch::ItchMarketState
        marketState;

    //
    // REAL handler.
    //
    llt::itch::ItchFeedHandler handler{
        source,
        recovery,
        marketState};

    const int sender =
        createUdpSender();

    REQUIRE(sender >= 0);

    //
    // Send:
    //
    // 100
    // 101
    // 104
    //
    // Missing:
    //
    // 102
    // 103
    //
    REQUIRE(
        sendPacket(
            sender,
            port,
            makeSystemEventPacket(100)));

    REQUIRE(
        sendPacket(
            sender,
            port,
            makeSystemEventPacket(101)));

    REQUIRE(
        sendPacket(
            sender,
            port,
            makeSystemEventPacket(104)));

    //
    // Three LIVE packets should be accepted.
    //
    // When 104 arrives:
    //
    // expected = 102
    // received = 104
    //
    // ItchSequenceRecovery requests [102, 104).
    //
    handler.start(3);

    ::close(sender);

    REQUIRE(
        handler.state() ==
        llt::FeedHandlerState::Stopped);

    REQUIRE(
        handler.processedPackets() ==
        3);

    REQUIRE(
        handler.recoveredPackets() ==
        2);

    REQUIRE(
        handler.gapsDetected() ==
        1);

    REQUIRE(
        handler.ignoredPackets() ==
        0);

    REQUIRE(
        recoverySource.callCount ==
        1);

    REQUIRE(
        recoverySource.lastFromSequence ==
        102);

    REQUIRE(
        recoverySource.lastToSequence ==
        104);
}