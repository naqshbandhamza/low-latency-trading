#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <vector>

#include "market_data/moldudp64/IMoldItchRecoverySource.h"
#include "market_data/moldudp64/MoldItchSequenceRecovery.h"
#include "market_data/moldudp64/SequencedItchMessage.h"

#include "types/SequenceCheckResult.h"


namespace
{

class MockMoldItchRecoverySource final
    : public llt::moldudp64::IMoldItchRecoverySource
{
public:
    bool recover(
        std::uint64_t fromSequence,
        std::uint64_t toSequence,
        std::vector<
            llt::moldudp64::SequencedItchMessage>& messages
    ) override
    {
        ++calls;

        lastFromSequence =
            fromSequence;

        lastToSequence =
            toSequence;

        messages =
            messagesToReturn;

        return shouldSucceed;
    }

    bool shouldSucceed{true};

    std::vector<
        llt::moldudp64::SequencedItchMessage>
        messagesToReturn{};

    std::uint64_t calls{0};

    std::uint64_t lastFromSequence{0};

    std::uint64_t lastToSequence{0};
};


llt::moldudp64::SequencedItchMessage
makeMessage(
    std::uint64_t sequence)
{
    llt::moldudp64::SequencedItchMessage
        message{};

    message.sequence =
        sequence;

    message.payloadSize =
        1;

    message.payload[0] =
        static_cast<std::uint8_t>('S');

    return message;
}

} // namespace


TEST_CASE(
    "Mold ITCH sequence recovery accepts complete contiguous range")
{
    MockMoldItchRecoverySource source;

    source.messagesToReturn = {
        makeMessage(102),
        makeMessage(103),
        makeMessage(104)
    };

    llt::moldudp64::MoldItchSequenceRecovery
        recovery{source};

    std::vector<
        llt::moldudp64::SequencedItchMessage>
        recoveredMessages;

    const auto result =
        recovery.recover(
            102,
            105,
            recoveredMessages);

    REQUIRE(
        result ==
        llt::SequenceCheckResult::Process);

    REQUIRE(source.calls == 1);

    REQUIRE(
        source.lastFromSequence ==
        102);

    REQUIRE(
        source.lastToSequence ==
        105);

    REQUIRE(
        recoveredMessages.size() ==
        3);

    CHECK(
        recoveredMessages[0].sequence ==
        102);

    CHECK(
        recoveredMessages[1].sequence ==
        103);

    CHECK(
        recoveredMessages[2].sequence ==
        104);
}


TEST_CASE(
    "Mold ITCH sequence recovery stops when source fails")
{
    MockMoldItchRecoverySource source;

    source.shouldSucceed =
        false;

    source.messagesToReturn = {
        makeMessage(102),
        makeMessage(103),
        makeMessage(104)
    };

    llt::moldudp64::MoldItchSequenceRecovery
        recovery{source};

    std::vector<
        llt::moldudp64::SequencedItchMessage>
        recoveredMessages;

    recoveredMessages.push_back(
        makeMessage(999));

    const auto result =
        recovery.recover(
            102,
            105,
            recoveredMessages);

    REQUIRE(
        result ==
        llt::SequenceCheckResult::Stop);

    REQUIRE(source.calls == 1);

    CHECK(
        recoveredMessages.empty());
}


TEST_CASE(
    "Mold ITCH sequence recovery rejects incomplete range")
{
    MockMoldItchRecoverySource source;

    //
    // Requested:
    //
    // [102, 105)
    //
    // Required:
    //
    // 102, 103, 104
    //
    // But source only returns two messages.
    //
    source.messagesToReturn = {
        makeMessage(102),
        makeMessage(103)
    };

    llt::moldudp64::MoldItchSequenceRecovery
        recovery{source};

    std::vector<
        llt::moldudp64::SequencedItchMessage>
        recoveredMessages;

    const auto result =
        recovery.recover(
            102,
            105,
            recoveredMessages);

    REQUIRE(
        result ==
        llt::SequenceCheckResult::Stop);

    REQUIRE(source.calls == 1);

    CHECK(
        recoveredMessages.empty());
}


TEST_CASE(
    "Mold ITCH sequence recovery rejects non contiguous range")
{
    MockMoldItchRecoverySource source;

    //
    // 103 is missing.
    //
    source.messagesToReturn = {
        makeMessage(102),
        makeMessage(104),
        makeMessage(105)
    };

    llt::moldudp64::MoldItchSequenceRecovery
        recovery{source};

    std::vector<
        llt::moldudp64::SequencedItchMessage>
        recoveredMessages;

    const auto result =
        recovery.recover(
            102,
            105,
            recoveredMessages);

    REQUIRE(
        result ==
        llt::SequenceCheckResult::Stop);

    REQUIRE(source.calls == 1);

    CHECK(
        recoveredMessages.empty());
}


TEST_CASE(
    "Mold ITCH sequence recovery rejects wrong starting sequence")
{
    MockMoldItchRecoverySource source;

    source.messagesToReturn = {
        makeMessage(101),
        makeMessage(102),
        makeMessage(103)
    };

    llt::moldudp64::MoldItchSequenceRecovery
        recovery{source};

    std::vector<
        llt::moldudp64::SequencedItchMessage>
        recoveredMessages;

    const auto result =
        recovery.recover(
            102,
            105,
            recoveredMessages);

    REQUIRE(
        result ==
        llt::SequenceCheckResult::Stop);

    REQUIRE(source.calls == 1);

    CHECK(
        recoveredMessages.empty());
}


TEST_CASE(
    "Mold ITCH sequence recovery ignores non forward range")
{
    MockMoldItchRecoverySource source;

    llt::moldudp64::MoldItchSequenceRecovery
        recovery{source};

    std::vector<
        llt::moldudp64::SequencedItchMessage>
        recoveredMessages;

    recoveredMessages.push_back(
        makeMessage(999));

    const auto result =
        recovery.recover(
            105,
            105,
            recoveredMessages);

    REQUIRE(
        result ==
        llt::SequenceCheckResult::Ignore);

    CHECK(source.calls == 0);

    CHECK(
        recoveredMessages.empty());
}


TEST_CASE(
    "Mold ITCH sequence recovery ignores older received sequence")
{
    MockMoldItchRecoverySource source;

    llt::moldudp64::MoldItchSequenceRecovery
        recovery{source};

    std::vector<
        llt::moldudp64::SequencedItchMessage>
        recoveredMessages;

    const auto result =
        recovery.recover(
            105,
            103,
            recoveredMessages);

    REQUIRE(
        result ==
        llt::SequenceCheckResult::Ignore);

    CHECK(source.calls == 0);

    CHECK(
        recoveredMessages.empty());
}