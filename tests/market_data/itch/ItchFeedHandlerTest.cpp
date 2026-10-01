#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <cstdint>
#include <vector>

#include "FeedHandlerState.h"

#include "ItchFeedHandler.h"
#include "market_data/itch/ItchMarketState.h"
#include "market_data/itch/ItchUdpPacket.h"

#include "MockItchMarketDataSource.h"
#include "MockItchSequenceRecovery.h"

namespace
{

//
// ITCH 5.0 System Event message:
//
//  0      message type = 'S'
//  1-2    stock locate
//  3-4    tracking number
//  5-10   timestamp (48-bit)
//  11     event code
//
// Total = 12 bytes
//
llt::itch::ItchUdpPacket makeSystemEventPacket(
    std::uint64_t transportSequence,
    char eventCode = 'O')
{
    llt::itch::ItchUdpPacket packet{};

    packet.sequence =
        transportSequence;

    packet.payloadSize =
        12;

    packet.payload[0] =
        static_cast<std::uint8_t>('S');

    //
    // stock locate = 0
    //
    packet.payload[1] = 0;
    packet.payload[2] = 0;

    //
    // tracking number = 0
    //
    packet.payload[3] = 0;
    packet.payload[4] = 0;

    //
    // timestamp = transportSequence
    //
    // Store the low 48 bits big-endian.
    //
    packet.payload[5] =
        static_cast<std::uint8_t>(
            (transportSequence >> 40) & 0xFF);

    packet.payload[6] =
        static_cast<std::uint8_t>(
            (transportSequence >> 32) & 0xFF);

    packet.payload[7] =
        static_cast<std::uint8_t>(
            (transportSequence >> 24) & 0xFF);

    packet.payload[8] =
        static_cast<std::uint8_t>(
            (transportSequence >> 16) & 0xFF);

    packet.payload[9] =
        static_cast<std::uint8_t>(
            (transportSequence >> 8) & 0xFF);

    packet.payload[10] =
        static_cast<std::uint8_t>(
            transportSequence & 0xFF);

    packet.payload[11] =
        static_cast<std::uint8_t>(
            eventCode);

    return packet;
}


llt::itch::ItchUdpPacket
makeMalformedPacket(
    std::uint64_t transportSequence)
{
    llt::itch::ItchUdpPacket packet{};

    packet.sequence =
        transportSequence;

    //
    // 'S' requires the complete System Event
    // message, but we're intentionally supplying
    // only one byte.
    //
    packet.payloadSize =
        1;

    packet.payload[0] =
        static_cast<std::uint8_t>('S');

    return packet;
}

} // namespace


TEST_CASE(
    "ITCH feed handler processes first packet")
{
    llt::itch::test::MockItchMarketDataSource
        source{
            {
                makeSystemEventPacket(100)
            }};

    llt::itch::test::MockItchSequenceRecovery
        recovery;

    llt::itch::ItchMarketState
        marketState;

    llt::itch::ItchFeedHandler handler{
        source,
        recovery,
        marketState};

    handler.start(1);

    REQUIRE(
        handler.state() ==
        llt::FeedHandlerState::Stopped);

    REQUIRE(
        handler.processedPackets() ==
        1);

    REQUIRE(
        handler.recoveredPackets() ==
        0);

    REQUIRE(
        handler.ignoredPackets() ==
        0);

    REQUIRE(
        handler.gapsDetected() ==
        0);

    REQUIRE(
        recovery.callCount ==
        0);
}


TEST_CASE(
    "ITCH feed handler processes contiguous packets")
{
    llt::itch::test::MockItchMarketDataSource
        source{
            {
                makeSystemEventPacket(100),
                makeSystemEventPacket(101),
                makeSystemEventPacket(102)
            }};

    llt::itch::test::MockItchSequenceRecovery
        recovery;

    llt::itch::ItchMarketState
        marketState;

    llt::itch::ItchFeedHandler handler{
        source,
        recovery,
        marketState};

    handler.start(3);

    REQUIRE(
        handler.state() ==
        llt::FeedHandlerState::Stopped);

    REQUIRE(
        handler.processedPackets() ==
        3);

    REQUIRE(
        handler.recoveredPackets() ==
        0);

    REQUIRE(
        handler.ignoredPackets() ==
        0);

    REQUIRE(
        handler.gapsDetected() ==
        0);

    REQUIRE(
        recovery.callCount ==
        0);
}


TEST_CASE(
    "ITCH feed handler supports arbitrary starting sequence")
{
    llt::itch::test::MockItchMarketDataSource
        source{
            {
                makeSystemEventPacket(9000),
                makeSystemEventPacket(9001),
                makeSystemEventPacket(9002)
            }};

    llt::itch::test::MockItchSequenceRecovery
        recovery;

    llt::itch::ItchMarketState
        marketState;

    llt::itch::ItchFeedHandler handler{
        source,
        recovery,
        marketState};

    handler.start(3);

    REQUIRE(
        handler.state() ==
        llt::FeedHandlerState::Stopped);

    REQUIRE(
        handler.processedPackets() ==
        3);

    REQUIRE(
        handler.gapsDetected() ==
        0);

    REQUIRE(
        recovery.callCount ==
        0);
}


TEST_CASE(
    "ITCH feed handler ignores duplicate packet")
{
    llt::itch::test::MockItchMarketDataSource
        source{
            {
                makeSystemEventPacket(100),
                makeSystemEventPacket(101),

                //
                // duplicate
                //
                makeSystemEventPacket(101),

                makeSystemEventPacket(102)
            }};

    llt::itch::test::MockItchSequenceRecovery
        recovery;

    llt::itch::ItchMarketState
        marketState;

    llt::itch::ItchFeedHandler handler{
        source,
        recovery,
        marketState};

    //
    // Three accepted live packets:
    //
    // 100
    // 101
    // 102
    //
    handler.start(3);

    REQUIRE(
        handler.state() ==
        llt::FeedHandlerState::Stopped);

    REQUIRE(
        handler.processedPackets() ==
        3);

    REQUIRE(
        handler.ignoredPackets() ==
        1);

    REQUIRE(
        handler.gapsDetected() ==
        0);

    REQUIRE(
        handler.recoveredPackets() ==
        0);

    REQUIRE(
        source.delivered() ==
        4);
}


TEST_CASE(
    "ITCH feed handler recovers single missing packet")
{
    llt::itch::test::MockItchMarketDataSource
        source{
            {
                makeSystemEventPacket(100),
                makeSystemEventPacket(101),
                makeSystemEventPacket(103)
            }};

    llt::itch::test::MockItchSequenceRecovery
        recovery;

    recovery.packetsToReturn = {
        makeSystemEventPacket(102)
    };

    llt::itch::ItchMarketState
        marketState;

    llt::itch::ItchFeedHandler handler{
        source,
        recovery,
        marketState};

    handler.start(3);

    REQUIRE(
        handler.state() ==
        llt::FeedHandlerState::Stopped);

    REQUIRE(
        handler.processedPackets() ==
        3);

    REQUIRE(
        handler.recoveredPackets() ==
        1);

    REQUIRE(
        handler.gapsDetected() ==
        1);

    REQUIRE(
        handler.ignoredPackets() ==
        0);

    REQUIRE(
        recovery.callCount ==
        1);

    REQUIRE(
        recovery.lastExpectedSequence ==
        102);

    REQUIRE(
        recovery.lastReceivedSequence ==
        103);
}


TEST_CASE(
    "ITCH feed handler recovers multiple missing packets")
{
    llt::itch::test::MockItchMarketDataSource
        source{
            {
                makeSystemEventPacket(100),
                makeSystemEventPacket(104)
            }};

    llt::itch::test::MockItchSequenceRecovery
        recovery;

    recovery.packetsToReturn = {
        makeSystemEventPacket(101),
        makeSystemEventPacket(102),
        makeSystemEventPacket(103)
    };

    llt::itch::ItchMarketState
        marketState;

    llt::itch::ItchFeedHandler handler{
        source,
        recovery,
        marketState};

    handler.start(2);

    REQUIRE(
        handler.state() ==
        llt::FeedHandlerState::Stopped);

    REQUIRE(
        handler.processedPackets() ==
        2);

    REQUIRE(
        handler.recoveredPackets() ==
        3);

    REQUIRE(
        handler.gapsDetected() ==
        1);

    REQUIRE(
        handler.ignoredPackets() ==
        0);

    REQUIRE(
        recovery.callCount ==
        1);

    REQUIRE(
        recovery.lastExpectedSequence ==
        101);

    REQUIRE(
        recovery.lastReceivedSequence ==
        104);
}


TEST_CASE(
    "ITCH feed handler fails when sequence recovery fails")
{
    llt::itch::test::MockItchMarketDataSource
        source{
            {
                makeSystemEventPacket(100),
                makeSystemEventPacket(102)
            }};

    llt::itch::test::MockItchSequenceRecovery
        recovery;

    recovery.result =
        llt::SequenceCheckResult::Stop;

    llt::itch::ItchMarketState
        marketState;

    llt::itch::ItchFeedHandler handler{
        source,
        recovery,
        marketState};

    //
    // Handler will stop before processing live 102.
    //
    handler.start(2);

    REQUIRE(
        handler.state() ==
        llt::FeedHandlerState::Failed);

    REQUIRE(
        handler.processedPackets() ==
        1);

    REQUIRE(
        handler.recoveredPackets() ==
        0);

    REQUIRE(
        handler.gapsDetected() ==
        1);

    REQUIRE(
        recovery.callCount ==
        1);

    REQUIRE(
        recovery.lastExpectedSequence ==
        101);

    REQUIRE(
        recovery.lastReceivedSequence ==
        102);
}


TEST_CASE(
    "ITCH feed handler fails on malformed ITCH payload")
{
    llt::itch::test::MockItchMarketDataSource
        source{
            {
                makeMalformedPacket(100)
            }};

    llt::itch::test::MockItchSequenceRecovery
        recovery;

    llt::itch::ItchMarketState
        marketState;

    llt::itch::ItchFeedHandler handler{
        source,
        recovery,
        marketState};

    handler.start(1);

    REQUIRE(
        handler.state() ==
        llt::FeedHandlerState::Failed);

    REQUIRE(
        handler.processedPackets() ==
        0);

    REQUIRE(
        handler.recoveredPackets() ==
        0);

    REQUIRE(
        recovery.callCount ==
        0);
}


TEST_CASE(
    "ITCH feed handler fails when recovered packet is malformed")
{
    llt::itch::test::MockItchMarketDataSource
        source{
            {
                makeSystemEventPacket(100),
                makeSystemEventPacket(102)
            }};

    llt::itch::test::MockItchSequenceRecovery
        recovery;

    recovery.packetsToReturn = {
        makeMalformedPacket(101)
    };

    llt::itch::ItchMarketState
        marketState;

    llt::itch::ItchFeedHandler handler{
        source,
        recovery,
        marketState};

    handler.start(2);

    REQUIRE(
        handler.state() ==
        llt::FeedHandlerState::Failed);

    REQUIRE(
        handler.processedPackets() ==
        1);

    REQUIRE(
        handler.recoveredPackets() ==
        0);

    REQUIRE(
        handler.gapsDetected() ==
        1);

    REQUIRE(
        recovery.callCount ==
        1);
}