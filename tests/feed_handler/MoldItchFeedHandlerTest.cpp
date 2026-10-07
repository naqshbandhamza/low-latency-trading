#include <catch2/catch_test_macros.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

#include "MoldItchFeedHandler.h"

#include "market_data/itch/ItchMarketState.h"

#include "market_data/moldudp64/IMoldItchSequenceRecovery.h"
#include "market_data/moldudp64/IMoldMarketDataSource.h"
#include "market_data/moldudp64/MoldUdp64.h"
#include "market_data/moldudp64/MoldUdp64Codec.h"
#include "market_data/moldudp64/SequencedItchMessage.h"

#include "types/SequenceCheckResult.h"


namespace
{

using llt::moldudp64::Datagram;
using llt::moldudp64::ReceivedMoldDatagram;
using llt::moldudp64::SequencedItchMessage;
using llt::moldudp64::Session;


class MockMoldMarketDataSource final
    : public llt::moldudp64::IMoldMarketDataSource
{
public:
    bool receive(
        ReceivedMoldDatagram& datagram
    ) noexcept override
    {
        if (nextDatagram >= datagrams.size())
        {
            return false;
        }

        datagram =
            datagrams[nextDatagram];

        ++nextDatagram;

        return true;
    }

    std::vector<ReceivedMoldDatagram>
        datagrams{};

    std::size_t nextDatagram{0};
};


class MockMoldItchSequenceRecovery final
    : public llt::moldudp64::IMoldItchSequenceRecovery
{
public:
    llt::SequenceCheckResult recover(
        std::uint64_t expectedSequence,
        std::uint64_t receivedSequence,
        std::vector<SequencedItchMessage>& recoveredMessages
    ) override
    {
        ++calls;

        lastExpectedSequence =
            expectedSequence;

        lastReceivedSequence =
            receivedSequence;

        recoveredMessages =
            messagesToReturn;

        return resultToReturn;
    }

    llt::SequenceCheckResult resultToReturn{
        llt::SequenceCheckResult::Process};

    std::vector<SequencedItchMessage>
        messagesToReturn{};

    std::uint64_t calls{0};

    std::uint64_t lastExpectedSequence{0};

    std::uint64_t lastReceivedSequence{0};
};


std::array<std::uint8_t, 12>
makeSystemEventMessage(
    char eventCode,
    std::uint64_t timestamp)
{
    std::array<std::uint8_t, 12>
        message{};

    message[0] =
        static_cast<std::uint8_t>('S');

    //
    // Stock Locate
    //
    message[1] = 0;
    message[2] = 0;

    //
    // Tracking Number
    //
    message[3] = 0;
    message[4] = 0;

    //
    // Timestamp is a 6-byte big-endian value.
    //
    message[5] =
        static_cast<std::uint8_t>(
            (timestamp >> 40) & 0xFF);

    message[6] =
        static_cast<std::uint8_t>(
            (timestamp >> 32) & 0xFF);

    message[7] =
        static_cast<std::uint8_t>(
            (timestamp >> 24) & 0xFF);

    message[8] =
        static_cast<std::uint8_t>(
            (timestamp >> 16) & 0xFF);

    message[9] =
        static_cast<std::uint8_t>(
            (timestamp >> 8) & 0xFF);

    message[10] =
        static_cast<std::uint8_t>(
            timestamp & 0xFF);

    message[11] =
        static_cast<std::uint8_t>(
            eventCode);

    return message;
}


Session makeSession()
{
    Session session{};

    constexpr char value[] =
        "20190130A ";

    static_assert(
        sizeof(value) - 1 ==
        llt::moldudp64::SessionSize);

    std::memcpy(
        session.data(),
        value,
        llt::moldudp64::SessionSize);

    return session;
}


ReceivedMoldDatagram makeDatagram(
    std::uint64_t firstSequence,
    const std::vector<
        std::array<std::uint8_t, 12>>& messages)
{
    ReceivedMoldDatagram received{};

    std::size_t encodedSize{0};

    std::uint16_t messageCount{0};

    const auto session =
        makeSession();

    REQUIRE(
        llt::moldudp64::MoldUdp64Codec::
            beginPacket(
                session,
                firstSequence,
                received.bytes,
                encodedSize));

    for (const auto& message : messages)
    {
        REQUIRE(
            llt::moldudp64::MoldUdp64Codec::
                appendMessage(
                    message.data(),
                    message.size(),
                    received.bytes,
                    encodedSize,
                    messageCount));
    }

    llt::moldudp64::MoldUdp64Codec::
        finalizePacket(
            received.bytes,
            messageCount);

    received.size =
        encodedSize;

    return received;
}


SequencedItchMessage
makeRecoveredMessage(
    std::uint64_t sequence,
    char eventCode,
    std::uint64_t timestamp)
{
    const auto itchMessage =
        makeSystemEventMessage(
            eventCode,
            timestamp);

    SequencedItchMessage message{};

    message.sequence =
        sequence;

    message.payloadSize =
        static_cast<std::uint16_t>(
            itchMessage.size());

    std::memcpy(
        message.payload.data(),
        itchMessage.data(),
        itchMessage.size());

    return message;
}

} // namespace


TEST_CASE(
    "Mold feed handler processes multiple ITCH messages from one datagram")
{
    MockMoldMarketDataSource source;

    MockMoldItchSequenceRecovery recovery;

    llt::itch::ItchMarketState marketState;

    source.datagrams.push_back(
        makeDatagram(
            100,
            {
                makeSystemEventMessage(
                    'O',
                    1001),

                makeSystemEventMessage(
                    'S',
                    1002),

                makeSystemEventMessage(
                    'Q',
                    1003)
            }));

    llt::itch::MoldItchFeedHandler handler{
        source,
        recovery,
        marketState};

    handler.start(1);

    CHECK(
        handler.state() ==
        llt::FeedHandlerState::Stopped);

    CHECK(
        handler.processedDatagrams() ==
        1);

    CHECK(
        handler.processedMessages() ==
        3);

    CHECK(
        handler.recoveredMessages() ==
        0);

    CHECK(
        handler.ignoredMessages() ==
        0);

    CHECK(
        handler.gapsDetected() ==
        0);

    CHECK(
        handler.malformedDatagrams() ==
        0);

    CHECK(
        handler.expectedSequence() ==
        103);

    CHECK(
        recovery.calls ==
        0);
}


TEST_CASE(
    "Mold feed handler processes contiguous datagrams")
{
    MockMoldMarketDataSource source;

    MockMoldItchSequenceRecovery recovery;

    llt::itch::ItchMarketState marketState;

    source.datagrams.push_back(
        makeDatagram(
            100,
            {
                makeSystemEventMessage(
                    'O',
                    1001),

                makeSystemEventMessage(
                    'S',
                    1002)
            }));

    source.datagrams.push_back(
        makeDatagram(
            102,
            {
                makeSystemEventMessage(
                    'Q',
                    1003),

                makeSystemEventMessage(
                    'M',
                    1004)
            }));

    llt::itch::MoldItchFeedHandler handler{
        source,
        recovery,
        marketState};

    handler.start(2);

    CHECK(
        handler.state() ==
        llt::FeedHandlerState::Stopped);

    CHECK(
        handler.processedDatagrams() ==
        2);

    CHECK(
        handler.processedMessages() ==
        4);

    CHECK(
        handler.recoveredMessages() ==
        0);

    CHECK(
        handler.ignoredMessages() ==
        0);

    CHECK(
        handler.gapsDetected() ==
        0);

    CHECK(
        handler.expectedSequence() ==
        104);

    CHECK(
        recovery.calls ==
        0);
}


TEST_CASE(
    "Mold feed handler ignores duplicate prefix and processes overlapping suffix")
{
    MockMoldMarketDataSource source;

    MockMoldItchSequenceRecovery recovery;

    llt::itch::ItchMarketState marketState;

    //
    // Establish:
    //
    // 100, 101, 102, 103, 104
    //
    // expected = 105
    //
    source.datagrams.push_back(
        makeDatagram(
            100,
            {
                makeSystemEventMessage(
                    'O',
                    1001),

                makeSystemEventMessage(
                    'S',
                    1002),

                makeSystemEventMessage(
                    'Q',
                    1003),

                makeSystemEventMessage(
                    'M',
                    1004),

                makeSystemEventMessage(
                    'E',
                    1005)
            }));

    //
    // Overlap:
    //
    // 103, 104 -> duplicate
    // 105, 106, 107 -> new
    //
    source.datagrams.push_back(
        makeDatagram(
            103,
            {
                makeSystemEventMessage(
                    'S',
                    2001),

                makeSystemEventMessage(
                    'S',
                    2002),

                makeSystemEventMessage(
                    'S',
                    2003),

                makeSystemEventMessage(
                    'S',
                    2004),

                makeSystemEventMessage(
                    'S',
                    2005)
            }));

    llt::itch::MoldItchFeedHandler handler{
        source,
        recovery,
        marketState};

    handler.start(2);

    CHECK(
        handler.state() ==
        llt::FeedHandlerState::Stopped);

    CHECK(
        handler.processedDatagrams() ==
        2);

    //
    // 5 original + 3 new suffix messages.
    //
    CHECK(
        handler.processedMessages() ==
        8);

    CHECK(
        handler.ignoredMessages() ==
        2);

    CHECK(
        handler.gapsDetected() ==
        0);

    CHECK(
        handler.recoveredMessages() ==
        0);

    CHECK(
        handler.expectedSequence() ==
        108);

    CHECK(
        recovery.calls ==
        0);
}


TEST_CASE(
    "Mold feed handler recovers forward message gap")
{
    MockMoldMarketDataSource source;

    MockMoldItchSequenceRecovery recovery;

    llt::itch::ItchMarketState marketState;

    //
    // First datagram:
    //
    // 100, 101
    //
    // expected = 102
    //
    source.datagrams.push_back(
        makeDatagram(
            100,
            {
                makeSystemEventMessage(
                    'O',
                    1001),

                makeSystemEventMessage(
                    'S',
                    1002)
            }));

    //
    // Next live message is 105.
    //
    // Missing:
    //
    // 102, 103, 104
    //
    source.datagrams.push_back(
        makeDatagram(
            105,
            {
                makeSystemEventMessage(
                    'E',
                    1006)
            }));

    recovery.messagesToReturn = {
        makeRecoveredMessage(
            102,
            'Q',
            1003),

        makeRecoveredMessage(
            103,
            'M',
            1004),

        makeRecoveredMessage(
            104,
            'E',
            1005)
    };

    llt::itch::MoldItchFeedHandler handler{
        source,
        recovery,
        marketState};

    handler.start(2);

    CHECK(
        handler.state() ==
        llt::FeedHandlerState::Stopped);

    CHECK(
        handler.processedDatagrams() ==
        2);

    //
    // Live:
    //
    // 100, 101, 105
    //
    CHECK(
        handler.processedMessages() ==
        3);

    //
    // Recovery:
    //
    // 102, 103, 104
    //
    CHECK(
        handler.recoveredMessages() ==
        3);

    CHECK(
        handler.gapsDetected() ==
        1);

    CHECK(
        handler.ignoredMessages() ==
        0);

    CHECK(
        recovery.calls ==
        1);

    CHECK(
        recovery.lastExpectedSequence ==
        102);

    CHECK(
        recovery.lastReceivedSequence ==
        105);

    CHECK(
        handler.expectedSequence() ==
        106);
}


TEST_CASE(
    "Mold feed handler fails when sequence recovery fails")
{
    MockMoldMarketDataSource source;

    MockMoldItchSequenceRecovery recovery;

    llt::itch::ItchMarketState marketState;

    source.datagrams.push_back(
        makeDatagram(
            100,
            {
                makeSystemEventMessage(
                    'O',
                    1001)
            }));

    source.datagrams.push_back(
        makeDatagram(
            105,
            {
                makeSystemEventMessage(
                    'S',
                    1005)
            }));

    recovery.resultToReturn =
        llt::SequenceCheckResult::Stop;

    llt::itch::MoldItchFeedHandler handler{
        source,
        recovery,
        marketState};

    handler.start(2);

    CHECK(
        handler.state() ==
        llt::FeedHandlerState::Failed);

    CHECK(
        handler.processedDatagrams() ==
        1);

    CHECK(
        handler.processedMessages() ==
        1);

    CHECK(
        handler.recoveredMessages() ==
        0);

    CHECK(
        handler.gapsDetected() ==
        1);

    CHECK(
        recovery.calls ==
        1);

    CHECK(
        recovery.lastExpectedSequence ==
        101);

    CHECK(
        recovery.lastReceivedSequence ==
        105);
}


TEST_CASE(
    "Mold feed handler rejects malformed datagram before processing messages")
{
    MockMoldMarketDataSource source;

    MockMoldItchSequenceRecovery recovery;

    llt::itch::ItchMarketState marketState;

    auto datagram =
        makeDatagram(
            100,
            {
                makeSystemEventMessage(
                    'O',
                    1001),

                makeSystemEventMessage(
                    'S',
                    1002)
            });

    //
    // Truncate the datagram after it was correctly
    // encoded.
    //
    // The first message remains present, but the
    // second Mold message is incomplete.
    //
    REQUIRE(
        datagram.size >
        llt::moldudp64::HeaderSize);

    --datagram.size;

    source.datagrams.push_back(
        datagram);

    llt::itch::MoldItchFeedHandler handler{
        source,
        recovery,
        marketState};

    handler.start(1);

    CHECK(
        handler.state() ==
        llt::FeedHandlerState::Failed);

    CHECK(
        handler.processedDatagrams() ==
        0);

    //
    // Important:
    //
    // Pass 1 must reject the whole Mold datagram
    // before message 100 mutates market state.
    //
    CHECK(
        handler.processedMessages() ==
        0);

    CHECK(
        handler.recoveredMessages() ==
        0);

    CHECK(
        handler.expectedSequence() ==
        0);

    CHECK(
        handler.malformedDatagrams() ==
        1);

    CHECK(
        recovery.calls ==
        0);
}