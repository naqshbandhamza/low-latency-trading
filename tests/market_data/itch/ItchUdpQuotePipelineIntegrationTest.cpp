#include <catch2/catch_test_macros.hpp>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <variant>
#include <vector>

#include "FeedHandlerState.h"

#include "market_data/itch/IItchRecoverySource.h"
#include "ItchFeedHandler.h"
#include "market_data/itch/ItchMarketState.h"
#include "market_data/itch/ItchQuotePublisher.h"
#include "market_data/itch/ItchSequenceRecovery.h"
#include "market_data/itch/ItchUdpCodec.h"
#include "market_data/itch/ItchUdpPacket.h"
#include "market_data/itch/UdpItchMarketDataSource.h"

#include "market_data/MarketEvent.h"
#include "market_data/MarketEventQueue.h"

namespace
{

// =========================================================
// Big-endian wire helpers
// =========================================================

void writeU16(
    std::uint8_t* destination,
    std::uint16_t value)
{
    destination[0] =
        static_cast<std::uint8_t>(
            (value >> 8) & 0xFF);

    destination[1] =
        static_cast<std::uint8_t>(
            value & 0xFF);
}


void writeU32(
    std::uint8_t* destination,
    std::uint32_t value)
{
    destination[0] =
        static_cast<std::uint8_t>(
            (value >> 24) & 0xFF);

    destination[1] =
        static_cast<std::uint8_t>(
            (value >> 16) & 0xFF);

    destination[2] =
        static_cast<std::uint8_t>(
            (value >> 8) & 0xFF);

    destination[3] =
        static_cast<std::uint8_t>(
            value & 0xFF);
}


void writeU48(
    std::uint8_t* destination,
    std::uint64_t value)
{
    destination[0] =
        static_cast<std::uint8_t>(
            (value >> 40) & 0xFF);

    destination[1] =
        static_cast<std::uint8_t>(
            (value >> 32) & 0xFF);

    destination[2] =
        static_cast<std::uint8_t>(
            (value >> 24) & 0xFF);

    destination[3] =
        static_cast<std::uint8_t>(
            (value >> 16) & 0xFF);

    destination[4] =
        static_cast<std::uint8_t>(
            (value >> 8) & 0xFF);

    destination[5] =
        static_cast<std::uint8_t>(
            value & 0xFF);
}


void writeU64(
    std::uint8_t* destination,
    std::uint64_t value)
{
    for (
        std::size_t i = 0;
        i < 8;
        ++i)
    {
        const auto shift =
            static_cast<unsigned>(
                (7 - i) * 8);

        destination[i] =
            static_cast<std::uint8_t>(
                (value >> shift) & 0xFF);
    }
}


// =========================================================
// ITCH packet builders
// =========================================================

llt::itch::ItchUdpPacket
makeStockDirectoryPacket(
    std::uint64_t transportSequence,
    std::uint16_t stockLocate,
    std::uint64_t timestamp,
    const char* symbol)
{
    llt::itch::ItchUdpPacket packet{};

    packet.sequence =
        transportSequence;

    //
    // NASDAQ ITCH 5.0 Stock Directory = 39 bytes.
    //
    packet.payloadSize =
        39;

    auto* p =
        packet.payload.data();

    //
    // Message type.
    //
    p[0] =
        static_cast<std::uint8_t>('R');

    //
    // Stock locate.
    //
    writeU16(
        p + 1,
        stockLocate);

    //
    // Tracking number.
    //
    writeU16(
        p + 3,
        0);

    //
    // Timestamp.
    //
    writeU48(
        p + 5,
        timestamp);

    //
    // Stock symbol:
    // 8-byte space padded field.
    //
    for (
        std::size_t i = 0;
        i < 8;
        ++i)
    {
        p[11 + i] =
            static_cast<std::uint8_t>(' ');
    }

    for (
        std::size_t i = 0;
        i < 8 &&
        symbol[i] != '\0';
        ++i)
    {
        p[11 + i] =
            static_cast<std::uint8_t>(
                symbol[i]);
    }

    //
    // Market category.
    //
    p[19] =
        static_cast<std::uint8_t>('Q');

    //
    // Financial status indicator.
    //
    p[20] =
        static_cast<std::uint8_t>('N');

    //
    // Round lot size.
    //
    writeU32(
        p + 21,
        100);

    //
    // Round lots only.
    //
    p[25] =
        static_cast<std::uint8_t>('N');

    //
    // Issue classification.
    //
    p[26] =
        static_cast<std::uint8_t>('A');

    //
    // Issue subtype.
    //
    p[27] =
        static_cast<std::uint8_t>(' ');

    p[28] =
        static_cast<std::uint8_t>(' ');

    //
    // Authenticity.
    //
    p[29] =
        static_cast<std::uint8_t>('P');

    //
    // Short sale threshold indicator.
    //
    p[30] =
        static_cast<std::uint8_t>('N');

    //
    // IPO flag.
    //
    p[31] =
        static_cast<std::uint8_t>('N');

    //
    // LULD reference price tier.
    //
    p[32] =
        static_cast<std::uint8_t>('1');

    //
    // ETP flag.
    //
    p[33] =
        static_cast<std::uint8_t>('N');

    //
    // ETP leverage factor.
    //
    writeU32(
        p + 34,
        1);

    //
    // Inverse indicator.
    //
    p[38] =
        static_cast<std::uint8_t>('N');

    return packet;
}


llt::itch::ItchUdpPacket
makeAddOrderPacket(
    std::uint64_t transportSequence,
    std::uint16_t stockLocate,
    std::uint64_t timestamp,
    std::uint64_t orderReferenceNumber,
    char side,
    std::uint32_t shares,
    std::uint32_t price)
{
    llt::itch::ItchUdpPacket packet{};

    packet.sequence =
        transportSequence;

    //
    // NASDAQ ITCH 5.0 Add Order = 36 bytes.
    //
    packet.payloadSize =
        36;

    auto* p =
        packet.payload.data();

    //
    // Message type.
    //
    p[0] =
        static_cast<std::uint8_t>('A');

    //
    // Stock locate.
    //
    writeU16(
        p + 1,
        stockLocate);

    //
    // Tracking number.
    //
    writeU16(
        p + 3,
        0);

    //
    // Timestamp.
    //
    writeU48(
        p + 5,
        timestamp);

    //
    // Order reference number.
    //
    writeU64(
        p + 11,
        orderReferenceNumber);

    //
    // Buy / sell indicator.
    //
    p[19] =
        static_cast<std::uint8_t>(
            side);

    //
    // Shares.
    //
    writeU32(
        p + 20,
        shares);

    //
    // Stock symbol.
    //
    for (
        std::size_t i = 0;
        i < 8;
        ++i)
    {
        p[24 + i] =
            static_cast<std::uint8_t>(' ');
    }

    constexpr char symbol[] =
        "AAPL";

    for (
        std::size_t i = 0;
        i < 4;
        ++i)
    {
        p[24 + i] =
            static_cast<std::uint8_t>(
                symbol[i]);
    }

    //
    // Price.
    //
    writeU32(
        p + 32,
        price);

    return packet;
}


llt::itch::ItchUdpPacket
makeDeleteOrderPacket(
    std::uint64_t transportSequence,
    std::uint16_t stockLocate,
    std::uint64_t timestamp,
    std::uint64_t orderReferenceNumber)
{
    llt::itch::ItchUdpPacket packet{};

    packet.sequence =
        transportSequence;

    //
    // NASDAQ ITCH 5.0 Order Delete = 19 bytes.
    //
    packet.payloadSize =
        19;

    auto* p =
        packet.payload.data();

    p[0] =
        static_cast<std::uint8_t>('D');

    writeU16(
        p + 1,
        stockLocate);

    writeU16(
        p + 3,
        0);

    writeU48(
        p + 5,
        timestamp);

    writeU64(
        p + 11,
        orderReferenceNumber);

    return packet;
}


// =========================================================
// Deterministic recovery backend
// =========================================================

class TestRecoverySource final
    : public llt::itch::IItchRecoverySource
{
public:
    std::vector<
        llt::itch::ItchUdpPacket>
        packets{};

    std::size_t callCount{0};

    std::uint64_t lastFrom{0};

    std::uint64_t lastTo{0};

    bool recover(
        std::uint64_t fromSequence,
        std::uint64_t toSequence,
        std::vector<
            llt::itch::ItchUdpPacket>& output
    ) override
    {
        ++callCount;

        lastFrom =
            fromSequence;

        lastTo =
            toSequence;

        output.clear();

        for (
            const auto& packet :
            packets)
        {
            if (
                packet.sequence >=
                    fromSequence &&
                packet.sequence <
                    toSequence)
            {
                output.push_back(
                    packet);
            }
        }

        return true;
    }
};


// =========================================================
// Real UDP sender
// =========================================================

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
    "UDP ITCH pipeline recovers gap and publishes normalized BBO quotes")
{
    constexpr std::uint16_t port =
        19210;

    constexpr std::uint16_t stockLocate =
        42;

    //
    // =====================================================
    // Production pipeline
    // =====================================================
    //

    llt::itch::UdpItchMarketDataSource
        source{
            port,
            100};

    TestRecoverySource
        recoverySource;

    //
    // Real recovery implementation.
    //
    llt::itch::ItchSequenceRecovery
        recovery{
            recoverySource};

    //
    // Real market state.
    //
    llt::itch::ItchMarketState
        marketState;

    //
    // Existing generic downstream queue.
    //
    llt::MarketEventQueue
        marketEventQueue;

    //
    // Existing ITCH -> generic Quote publisher.
    //
    llt::itch::ItchQuotePublisher
        publisher{
            marketState.instruments(),
            marketEventQueue};

    //
    // IMPORTANT:
    //
    // ItchMarketState uses market_data::Timestamp,
    // which is the raw uint64_t timestamp type.
    //
    // ItchQuotePublisher converts that into the
    // generic strong llt::Timestamp used by Quote.
    //
    marketState.setBboChangeHandler(
        [&publisher](
            llt::market_data::InstrumentId instrumentId,
            llt::market_data::Timestamp timestamp,
            const llt::market_data::Bbo& bbo)
        {
            publisher.onBboChange(
                instrumentId,
                timestamp,
                bbo);
        });

    //
    // Real feed handler.
    //
    llt::itch::ItchFeedHandler
        handler{
            source,
            recovery,
            marketState};

    //
    // =====================================================
    // Sequence plan
    // =====================================================
    //
    // LIVE:
    //
    // 100 R : Stock Directory AAPL
    // 101 A : bid 100 @ 100.0000
    // 103 A : ask 150 @ 101.0000
    // 104 D : delete bid order #1
    //
    // RECOVERY:
    //
    // 102 A : bid 200 @ 99.0000
    //
    //
    // When live sequence 103 arrives:
    //
    // expected = 102
    // received = 103
    //
    // therefore recovery requests:
    //
    // [102, 103)
    //
    //
    // Correct processing order must become:
    //
    // 100 R
    // 101 A
    // 102 A   <- recovered
    // 103 A
    // 104 D
    //
    //
    // Before delete:
    //
    // BID
    // 100.0000 x 100  (#1)
    //  99.0000 x 200  (#2 recovered)
    //
    // ASK
    // 101.0000 x 150  (#3)
    //
    //
    // After deleting #1:
    //
    // BID
    // 99.0000 x 200
    //
    // ASK
    // 101.0000 x 150
    //
    // This proves recovered market data actually
    // participated in reconstruction of the book.
    //

    const auto directory =
        makeStockDirectoryPacket(
            100,
            stockLocate,
            1000,
            "AAPL");

    const auto bestBid =
        makeAddOrderPacket(
            101,
            stockLocate,
            1001,
            1,
            'B',
            100,
            1000000);

    const auto recoveredSecondBid =
        makeAddOrderPacket(
            102,
            stockLocate,
            1002,
            2,
            'B',
            200,
            990000);

    const auto ask =
        makeAddOrderPacket(
            103,
            stockLocate,
            1003,
            3,
            'S',
            150,
            1010000);

    const auto deleteBestBid =
        makeDeleteOrderPacket(
            104,
            stockLocate,
            1004,
            1);

    //
    // Recovery backend contains the missing packet.
    //
    recoverySource.packets = {
        recoveredSecondBid
    };

    //
    // =====================================================
    // Send real UDP traffic
    // =====================================================
    //

    const int sender =
        createSender();

    REQUIRE(
        sender >= 0);

    REQUIRE(
        sendPacket(
            sender,
            port,
            directory));

    REQUIRE(
        sendPacket(
            sender,
            port,
            bestBid));

    //
    // Sequence 102 deliberately missing from UDP.
    //

    REQUIRE(
        sendPacket(
            sender,
            port,
            ask));

    REQUIRE(
        sendPacket(
            sender,
            port,
            deleteBestBid));

    //
    // There are four accepted LIVE packets:
    //
    // 100
    // 101
    // 103
    // 104
    //
    handler.start(4);

    ::close(sender);

    //
    // =====================================================
    // Transport / sequencing assertions
    // =====================================================
    //

    REQUIRE(
        handler.state() ==
        llt::FeedHandlerState::Stopped);

    REQUIRE(
        handler.processedPackets() ==
        4);

    REQUIRE(
        handler.recoveredPackets() ==
        1);

    REQUIRE(
        handler.gapsDetected() ==
        1);

    REQUIRE(
        handler.ignoredPackets() ==
        0);

    //
    // =====================================================
    // Recovery assertions
    // =====================================================
    //

    REQUIRE(
        recoverySource.callCount ==
        1);

    REQUIRE(
        recoverySource.lastFrom ==
        102);

    REQUIRE(
        recoverySource.lastTo ==
        103);

    //
    // =====================================================
    // Market-state assertions
    // =====================================================
    //

    REQUIRE(
        marketState.instruments().contains(
            stockLocate));

    //
    // #1 was deleted.
    //
    // Remaining:
    //
    // #2 recovered bid
    // #3 live ask
    //
    REQUIRE(
        marketState.orders().size() ==
        2);

    REQUIRE(
        marketState.missingBooks() ==
        0);

    REQUIRE(
        marketState.failedBookReductions() ==
        0);

    REQUIRE(
        marketState.failedBookRemovals() ==
        0);

    //
    // =====================================================
    // Publisher assertions
    // =====================================================
    //

    REQUIRE(
        publisher.publishedQuotes() >
        0);

    REQUIRE(
        publisher.droppedQuotes() ==
        0);

    REQUIRE(
        publisher.unknownInstruments() ==
        0);

    //
    // =====================================================
    // Drain SPSC and retain latest Quote
    // =====================================================
    //
    // Quote has no default constructor and no usable
    // copy-assignment operator.
    //
    // std::optional::emplace() reconstructs the Quote
    // from its copy constructor instead.
    //

    std::optional<llt::Quote>
        latestQuote;

    while (true)
    {
        auto event =
            marketEventQueue.pop();

        if (!event.has_value())
        {
            break;
        }

        if (
            !std::holds_alternative<
                llt::Quote>(*event))
        {
            continue;
        }

        latestQuote.emplace(
            std::get<llt::Quote>(
                *event));
    }

    REQUIRE(
        latestQuote.has_value());

    //
    // Instrument was resolved through:
    //
    // stockLocate 42 -> InstrumentStore -> "AAPL"
    //
    REQUIRE(
        latestQuote->instrument() ==
        llt::Instrument{"AAPL"});

    //
    // Critical recovery assertion:
    //
    // The 99.0000 bid arrived ONLY through recovery.
    //
    // After deleting live order #1 at 100.0000,
    // recovered order #2 must become best bid.
    //
    REQUIRE(
        latestQuote->bid().price() ==
        llt::Price{990000});

    REQUIRE(
        latestQuote->bid().quantity() ==
        llt::Quantity{200});

    //
    // Live ask remains unchanged.
    //
    REQUIRE(
        latestQuote->ask().price() ==
        llt::Price{1010000});

    REQUIRE(
        latestQuote->ask().quantity() ==
        llt::Quantity{150});
}