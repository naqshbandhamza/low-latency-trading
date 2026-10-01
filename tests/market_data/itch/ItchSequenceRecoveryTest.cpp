#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <vector>

#include "market_data/itch/ItchSequenceRecovery.h"
#include "market_data/itch/ItchUdpPacket.h"
#include "types/SequenceCheckResult.h"

#include "MockItchRecoverySource.h"

namespace
{

llt::itch::ItchUdpPacket makePacket(
    std::uint64_t sequence)
{
    llt::itch::ItchUdpPacket packet{};

    packet.sequence =
        sequence;

    packet.payloadSize =
        1;

    packet.payload[0] =
        static_cast<std::uint8_t>('S');

    return packet;
}

} // namespace


TEST_CASE(
    "ITCH sequence recovery recovers single missing packet")
{
    llt::itch::test::MockItchRecoverySource
        source;

    source.packetsToReturn = {
        makePacket(102)
    };

    llt::itch::ItchSequenceRecovery
        recovery{
            source};

    std::vector<
        llt::itch::ItchUdpPacket
    > recovered;

    const auto result =
        recovery.recover(
            102,
            103,
            recovered);

    REQUIRE(
        result ==
        llt::SequenceCheckResult::Process);

    REQUIRE(source.callCount == 1);

    REQUIRE(
        source.lastFromSequence ==
        102);

    REQUIRE(
        source.lastToSequence ==
        103);

    REQUIRE(recovered.size() == 1);

    REQUIRE(
        recovered[0].sequence ==
        102);
}


TEST_CASE(
    "ITCH sequence recovery recovers multiple missing packets")
{
    llt::itch::test::MockItchRecoverySource
        source;

    source.packetsToReturn = {
        makePacket(102),
        makePacket(103),
        makePacket(104)
    };

    llt::itch::ItchSequenceRecovery
        recovery{
            source};

    std::vector<
        llt::itch::ItchUdpPacket
    > recovered;

    const auto result =
        recovery.recover(
            102,
            105,
            recovered);

    REQUIRE(
        result ==
        llt::SequenceCheckResult::Process);

    REQUIRE(recovered.size() == 3);

    REQUIRE(
        recovered[0].sequence ==
        102);

    REQUIRE(
        recovered[1].sequence ==
        103);

    REQUIRE(
        recovered[2].sequence ==
        104);
}


TEST_CASE(
    "ITCH sequence recovery stops when source fails")
{
    llt::itch::test::MockItchRecoverySource
        source;

    source.succeed =
        false;

    llt::itch::ItchSequenceRecovery
        recovery{
            source};

    std::vector<
        llt::itch::ItchUdpPacket
    > recovered;

    const auto result =
        recovery.recover(
            102,
            105,
            recovered);

    REQUIRE(
        result ==
        llt::SequenceCheckResult::Stop);

    REQUIRE(source.callCount == 1);

    REQUIRE(recovered.empty());
}


TEST_CASE(
    "ITCH sequence recovery rejects incomplete recovery")
{
    llt::itch::test::MockItchRecoverySource
        source;

    //
    // [102, 105) requires:
    //
    // 102, 103, 104
    //
    source.packetsToReturn = {
        makePacket(102),
        makePacket(103)
    };

    llt::itch::ItchSequenceRecovery
        recovery{
            source};

    std::vector<
        llt::itch::ItchUdpPacket
    > recovered;

    const auto result =
        recovery.recover(
            102,
            105,
            recovered);

    REQUIRE(
        result ==
        llt::SequenceCheckResult::Stop);

    REQUIRE(recovered.empty());
}


TEST_CASE(
    "ITCH sequence recovery rejects out of order packets")
{
    llt::itch::test::MockItchRecoverySource
        source;

    source.packetsToReturn = {
        makePacket(102),
        makePacket(104),
        makePacket(103)
    };

    llt::itch::ItchSequenceRecovery
        recovery{
            source};

    std::vector<
        llt::itch::ItchUdpPacket
    > recovered;

    const auto result =
        recovery.recover(
            102,
            105,
            recovered);

    REQUIRE(
        result ==
        llt::SequenceCheckResult::Stop);

    REQUIRE(recovered.empty());
}


TEST_CASE(
    "ITCH sequence recovery rejects wrong recovered sequence")
{
    llt::itch::test::MockItchRecoverySource
        source;

    //
    // Correct count, wrong final sequence.
    //
    source.packetsToReturn = {
        makePacket(102),
        makePacket(103),
        makePacket(106)
    };

    llt::itch::ItchSequenceRecovery
        recovery{
            source};

    std::vector<
        llt::itch::ItchUdpPacket
    > recovered;

    const auto result =
        recovery.recover(
            102,
            105,
            recovered);

    REQUIRE(
        result ==
        llt::SequenceCheckResult::Stop);

    REQUIRE(recovered.empty());
}


TEST_CASE(
    "ITCH sequence recovery ignores non forward recovery request")
{
    llt::itch::test::MockItchRecoverySource
        source;

    llt::itch::ItchSequenceRecovery
        recovery{
            source};

    std::vector<
        llt::itch::ItchUdpPacket
    > recovered;

    SECTION("equal sequence")
    {
        const auto result =
            recovery.recover(
                102,
                102,
                recovered);

        REQUIRE(
            result ==
            llt::SequenceCheckResult::Ignore);

        REQUIRE(source.callCount == 0);
        REQUIRE(recovered.empty());
    }

    SECTION("backward sequence")
    {
        const auto result =
            recovery.recover(
                103,
                102,
                recovered);

        REQUIRE(
            result ==
            llt::SequenceCheckResult::Ignore);

        REQUIRE(source.callCount == 0);
        REQUIRE(recovered.empty());
    }
}