#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <cstdint>
#include <string_view>
#include <variant>

#include "market_data/MarketEventQueue.h"

#include "market_data/itch/ItchMarketState.h"
#include "market_data/itch/ItchQuotePublisher.h"
#include "market_data/itch/ItchMessage.h"

#include "market_data/itch/messages/StockDirectoryMessage.h"
#include "market_data/itch/messages/AddOrderMessage.h"

#include "market_data/itch/messages/OrderExecutedMessage.h"
#include "market_data/itch/messages/OrderExecutedWithPriceMessage.h"
#include "market_data/itch/messages/OrderCancelMessage.h"

namespace
{

//
// ---------------------------------------------------------
// Test message builders
// ---------------------------------------------------------
//

llt::itch::StockDirectoryMessage makeDirectory(
    std::uint16_t stockLocate,
    std::string_view symbol,
    std::uint64_t timestamp = 1000)
{
    llt::itch::StockDirectoryMessage message{};

    message.stockLocate =
        stockLocate;

    message.timestamp =
        timestamp;

    message.stock.fill(' ');

    const auto length =
        std::min(
            symbol.size(),
            message.stock.size());

    std::copy_n(
        symbol.begin(),
        length,
        message.stock.begin());

    return message;
}


llt::itch::AddOrderMessage makeAdd(
    std::uint64_t orderId,
    std::uint16_t instrumentId,
    char side,
    std::uint32_t quantity,
    std::uint32_t price,
    std::uint64_t timestamp)
{
    llt::itch::AddOrderMessage message{};

    message.stockLocate =
        instrumentId;

    message.timestamp =
        timestamp;

    message.orderReferenceNumber =
        orderId;

    message.buySellIndicator =
        side;

    message.shares =
        quantity;

    message.price =
        price;

    return message;
}




llt::itch::OrderExecutedMessage makeExecution(
    std::uint16_t stockLocate,
    std::uint64_t orderId,
    std::uint32_t executedShares,
    std::uint64_t timestamp)
{
    llt::itch::OrderExecutedMessage message{};

    message.stockLocate =
        stockLocate;

    message.timestamp =
        timestamp;

    message.orderReferenceNumber =
        orderId;

    message.executedShares =
        executedShares;

    message.matchNumber =
        1;

    return message;
}


llt::itch::OrderExecutedWithPriceMessage
makeExecutionWithPrice(
    std::uint16_t stockLocate,
    std::uint64_t orderId,
    std::uint32_t executedShares,
    std::uint32_t executionPrice,
    std::uint64_t timestamp)
{
    llt::itch::OrderExecutedWithPriceMessage message{};

    message.stockLocate =
        stockLocate;

    message.timestamp =
        timestamp;

    message.orderReferenceNumber =
        orderId;

    message.executedShares =
        executedShares;

    message.matchNumber =
        1;

    message.printable =
        'Y';

    message.executionPrice =
        executionPrice;

    return message;
}


llt::itch::OrderCancelMessage makeCancel(
    std::uint16_t stockLocate,
    std::uint64_t orderId,
    std::uint32_t cancelledShares,
    std::uint64_t timestamp)
{
    llt::itch::OrderCancelMessage message{};

    message.stockLocate =
        stockLocate;

    message.timestamp =
        timestamp;

    message.orderReferenceNumber =
        orderId;

    message.cancelledShares =
        cancelledShares;

    return message;
}

//
// ---------------------------------------------------------
// Connected pipeline fixture
// ---------------------------------------------------------
//
// Construction order matters:
//
// queue
//   ↓
// marketState
//   ↓
// publisher references marketState.instruments()
//   ↓
// marketState callback references publisher
//
//

struct Pipeline
{
    llt::MarketEventQueue queue;

    llt::itch::ItchMarketState marketState;

    llt::itch::ItchQuotePublisher publisher;

    Pipeline()
        : queue{},
          marketState{},
          publisher{
              marketState.instruments(),
              queue}
    {
        marketState.setBboChangeHandler(
            [this](
                llt::market_data::InstrumentId instrumentId,
                llt::market_data::Timestamp timestamp,
                const llt::market_data::Bbo &bbo)
            {
                publisher.onBboChange(
                    instrumentId,
                    timestamp,
                    bbo);
            });
    }

    void send(
        const llt::itch::ItchMessage &message)
    {
        marketState.onMessage(
            message);
    }
};

} // namespace


//
// =========================================================
// 1.
// R -> bid -> ask -> normalized Quote -> SPSC
// =========================================================
//

TEST_CASE(
    "ITCH pipeline publishes quote when book becomes two sided")
{
    Pipeline pipeline;

    //
    // R: register stockLocate 42 as AAPL.
    //
    pipeline.send(
        llt::itch::ItchMessage{
            makeDirectory(
                42,
                "AAPL")});

    REQUIRE(
        pipeline.marketState
            .instruments()
            .contains(42));

    //
    // First order creates only a bid.
    //
    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                100,
                42,
                'B',
                500,
                1000000,
                2000)});

    //
    // Publisher deliberately requires a
    // two-sided BBO.
    //
    REQUIRE(
        pipeline.queue.size() ==
        0);

    REQUIRE(
        pipeline.publisher
            .publishedQuotes() ==
        0);

    //
    // First ask makes the BBO two-sided.
    //
    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                200,
                42,
                'S',
                300,
                1000100,
                3000)});

    REQUIRE(
        pipeline.queue.size() ==
        1);

    REQUIRE(
        pipeline.publisher
            .publishedQuotes() ==
        1);

    REQUIRE(
        pipeline.publisher
            .droppedQuotes() ==
        0);

    REQUIRE(
        pipeline.publisher
            .unknownInstruments() ==
        0);

    auto event =
        pipeline.queue.pop();

    REQUIRE(
        event.has_value());

    REQUIRE(
        std::holds_alternative<llt::Quote>(
            *event));
}


//
// =========================================================
// 2.
// Order behind BBO must not publish another Quote
// =========================================================
//

TEST_CASE(
    "ITCH pipeline ignores order behind current BBO")
{
    Pipeline pipeline;

    pipeline.send(
        llt::itch::ItchMessage{
            makeDirectory(
                42,
                "AAPL")});

    //
    // Best bid.
    //
    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                100,
                42,
                'B',
                500,
                1000000,
                2000)});

    //
    // Best ask. This creates first published Quote.
    //
    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                200,
                42,
                'S',
                300,
                1000100,
                3000)});

    REQUIRE(
        pipeline.queue.size() ==
        1);

    //
    // Remove first event so queue state is obvious.
    //
    auto firstEvent =
        pipeline.queue.pop();

    REQUIRE(
        firstEvent.has_value());

    REQUIRE(
        pipeline.queue.size() ==
        0);

    //
    // Worse bid.
    //
    // Book changes:
    //
    // 100.00 x 500   <- best
    //  99.99 x 700
    //
    // But BBO itself remains unchanged.
    //
    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                300,
                42,
                'B',
                700,
                999900,
                4000)});

    REQUIRE(
        pipeline.queue.size() ==
        0);

    REQUIRE(
        pipeline.publisher
            .publishedQuotes() ==
        1);

    REQUIRE(
        pipeline.publisher
            .droppedQuotes() ==
        0);
}


//
// =========================================================
// 3.
// Better bid changes BBO and publishes another Quote
// =========================================================
//

TEST_CASE(
    "ITCH pipeline publishes another quote when BBO changes")
{
    Pipeline pipeline;

    pipeline.send(
        llt::itch::ItchMessage{
            makeDirectory(
                42,
                "AAPL")});

    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                100,
                42,
                'B',
                500,
                1000000,
                2000)});

    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                200,
                42,
                'S',
                300,
                1000100,
                3000)});

    REQUIRE(
        pipeline.queue.size() ==
        1);

    //
    // Better bid:
    //
    // Before:
    // 100.00 x 500 / 100.01 x 300
    //
    // After:
    // 100.005 x 250 / 100.01 x 300
    //
    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                300,
                42,
                'B',
                250,
                1000050,
                4000)});

    REQUIRE(
        pipeline.queue.size() ==
        2);

    REQUIRE(
        pipeline.publisher
            .publishedQuotes() ==
        2);

    REQUIRE(
        pipeline.publisher
            .droppedQuotes() ==
        0);
}


//
// =========================================================
// 4.
// Published Quote contains correct normalized data
// =========================================================
//

TEST_CASE(
    "ITCH pipeline publishes correct normalized quote data")
{
    Pipeline pipeline;

    pipeline.send(
        llt::itch::ItchMessage{
            makeDirectory(
                42,
                "AAPL")});

    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                100,
                42,
                'B',
                500,
                1000000,
                2000)});

    //
    // This message makes the book two-sided.
    // Therefore its timestamp should become the
    // timestamp of the published normalized Quote.
    //
    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                200,
                42,
                'S',
                300,
                1000100,
                987654)});

    auto event =
        pipeline.queue.pop();

    REQUIRE(
        event.has_value());

    REQUIRE(
        std::holds_alternative<llt::Quote>(
            *event));

    const auto &quote =
        std::get<llt::Quote>(
            *event);

    REQUIRE(
        quote.instrument() ==
        llt::Instrument{
            "AAPL"});

    REQUIRE(
        quote.sequence() ==
        llt::SequenceNumber{
            0});

    REQUIRE(
        quote.timestamp() ==
        llt::Timestamp{
            987654});

    REQUIRE(
        quote.bid().price() ==
        llt::Price{
            1000000});

    REQUIRE(
        quote.bid().quantity() ==
        llt::Quantity{
            500});

    REQUIRE(
        quote.ask().price() ==
        llt::Price{
            1000100});

    REQUIRE(
        quote.ask().quantity() ==
        llt::Quantity{
            300});

    REQUIRE(
        pipeline.queue.size() ==
        0);
}


//
// =========================================================
// 5.
// Published normalized events receive increasing sequence
// =========================================================
//

TEST_CASE(
    "ITCH pipeline assigns increasing normalized quote sequences")
{
    Pipeline pipeline;

    pipeline.send(
        llt::itch::ItchMessage{
            makeDirectory(
                42,
                "AAPL")});

    //
    // Bid only — no publication.
    //
    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                100,
                42,
                'B',
                500,
                1000000,
                2000)});

    //
    // Ask makes book two-sided.
    //
    // Published sequence = 0.
    //
    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                200,
                42,
                'S',
                300,
                1000100,
                3000)});

    //
    // Better bid changes BBO.
    //
    // Published sequence = 1.
    //
    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                300,
                42,
                'B',
                250,
                1000050,
                4000)});

    REQUIRE(
        pipeline.queue.size() ==
        2);

    REQUIRE(
        pipeline.publisher
            .publishedQuotes() ==
        2);

    auto firstEvent =
        pipeline.queue.pop();

    auto secondEvent =
        pipeline.queue.pop();

    REQUIRE(
        firstEvent.has_value());

    REQUIRE(
        secondEvent.has_value());

    REQUIRE(
        std::holds_alternative<llt::Quote>(
            *firstEvent));

    REQUIRE(
        std::holds_alternative<llt::Quote>(
            *secondEvent));

    const auto &firstQuote =
        std::get<llt::Quote>(
            *firstEvent);

    const auto &secondQuote =
        std::get<llt::Quote>(
            *secondEvent);

    REQUIRE(
        firstQuote.sequence() ==
        llt::SequenceNumber{
            0});

    REQUIRE(
        secondQuote.sequence() ==
        llt::SequenceNumber{
            1});

    REQUIRE(
        firstQuote.timestamp() ==
        llt::Timestamp{
            3000});

    REQUIRE(
        secondQuote.timestamp() ==
        llt::Timestamp{
            4000});

    REQUIRE(
        firstQuote.bid().price() ==
        llt::Price{
            1000000});

    REQUIRE(
        secondQuote.bid().price() ==
        llt::Price{
            1000050});

    //
    // Ask did not change between publications.
    //
    REQUIRE(
        firstQuote.ask().price() ==
        secondQuote.ask().price());

    REQUIRE(
        firstQuote.ask().quantity() ==
        secondQuote.ask().quantity());

    REQUIRE(
        pipeline.queue.size() ==
        0);
}




TEST_CASE(
    "Full E execution publishes next BBO with correct instrument")
{
    Pipeline pipeline;

    pipeline.send(
        llt::itch::ItchMessage{
            makeDirectory(
                42,
                "AAPL")});

    //
    // Best bid.
    //
    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                100,
                42,
                'B',
                500,
                1000000,
                2000)});

    //
    // Next-best bid.
    //
    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                101,
                42,
                'B',
                400,
                999900,
                2100)});

    //
    // Ask makes book two-sided and publishes
    // the initial quote.
    //
    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                200,
                42,
                'S',
                300,
                1000100,
                2200)});

    REQUIRE(
        pipeline.queue.size() ==
        1);

    auto initial =
        pipeline.queue.pop();

    REQUIRE(initial.has_value());

    //
    // Fully execute best bid #100.
    //
    // This is the exact branch that previously
    // dereferenced order after orders_.remove().
    //
    pipeline.send(
        llt::itch::ItchMessage{
            makeExecution(
                42,
                100,
                500,
                3000)});

    REQUIRE(
        pipeline.queue.size() ==
        1);

    REQUIRE(
        pipeline.publisher
            .unknownInstruments() ==
        0);

    REQUIRE(
        pipeline.publisher
            .publishedQuotes() ==
        2);

    auto event =
        pipeline.queue.pop();

    REQUIRE(event.has_value());

    REQUIRE(
        std::holds_alternative<llt::Quote>(
            *event));

    const auto &quote =
        std::get<llt::Quote>(
            *event);

    REQUIRE(
        quote.instrument() ==
        llt::Instrument{"AAPL"});

    REQUIRE(
        quote.bid().price() ==
        llt::Price{999900});

    REQUIRE(
        quote.bid().quantity() ==
        llt::Quantity{400});

    REQUIRE(
        quote.ask().price() ==
        llt::Price{1000100});

    REQUIRE(
        quote.timestamp() ==
        llt::Timestamp{3000});
}


TEST_CASE(
    "Full C execution publishes next BBO with correct instrument")
{
    Pipeline pipeline;

    pipeline.send(
        llt::itch::ItchMessage{
            makeDirectory(
                42,
                "AAPL")});

    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                100,
                42,
                'B',
                500,
                1000000,
                2000)});

    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                101,
                42,
                'B',
                400,
                999900,
                2100)});

    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                200,
                42,
                'S',
                300,
                1000100,
                2200)});

    REQUIRE(
        pipeline.queue.size() ==
        1);

    auto initial =
        pipeline.queue.pop();

    REQUIRE(initial.has_value());

    //
    // Fully execute best bid using C.
    //
    pipeline.send(
        llt::itch::ItchMessage{
            makeExecutionWithPrice(
                42,
                100,
                500,
                1000000,
                3000)});

    REQUIRE(
        pipeline.queue.size() ==
        1);

    REQUIRE(
        pipeline.publisher
            .unknownInstruments() ==
        0);

    REQUIRE(
        pipeline.publisher
            .publishedQuotes() ==
        2);

    auto event =
        pipeline.queue.pop();

    REQUIRE(event.has_value());

    REQUIRE(
        std::holds_alternative<llt::Quote>(
            *event));

    const auto &quote =
        std::get<llt::Quote>(
            *event);

    REQUIRE(
        quote.instrument() ==
        llt::Instrument{"AAPL"});

    REQUIRE(
        quote.bid().price() ==
        llt::Price{999900});

    REQUIRE(
        quote.bid().quantity() ==
        llt::Quantity{400});

    REQUIRE(
        quote.ask().price() ==
        llt::Price{1000100});

    REQUIRE(
        quote.timestamp() ==
        llt::Timestamp{3000});
}


TEST_CASE(
    "Full X cancellation publishes next BBO with correct instrument")
{
    Pipeline pipeline;

    pipeline.send(
        llt::itch::ItchMessage{
            makeDirectory(
                42,
                "AAPL")});

    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                100,
                42,
                'B',
                500,
                1000000,
                2000)});

    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                101,
                42,
                'B',
                400,
                999900,
                2100)});

    pipeline.send(
        llt::itch::ItchMessage{
            makeAdd(
                200,
                42,
                'S',
                300,
                1000100,
                2200)});

    REQUIRE(
        pipeline.queue.size() ==
        1);

    auto initial =
        pipeline.queue.pop();

    REQUIRE(initial.has_value());

    //
    // Cancel the entire quantity of best bid #100.
    //
    pipeline.send(
        llt::itch::ItchMessage{
            makeCancel(
                42,
                100,
                500,
                3000)});

    REQUIRE(
        pipeline.queue.size() ==
        1);

    REQUIRE(
        pipeline.publisher
            .unknownInstruments() ==
        0);

    REQUIRE(
        pipeline.publisher
            .publishedQuotes() ==
        2);

    auto event =
        pipeline.queue.pop();

    REQUIRE(event.has_value());

    REQUIRE(
        std::holds_alternative<llt::Quote>(
            *event));

    const auto &quote =
        std::get<llt::Quote>(
            *event);

    REQUIRE(
        quote.instrument() ==
        llt::Instrument{"AAPL"});

    REQUIRE(
        quote.bid().price() ==
        llt::Price{999900});

    REQUIRE(
        quote.bid().quantity() ==
        llt::Quantity{400});

    REQUIRE(
        quote.ask().price() ==
        llt::Price{1000100});

    REQUIRE(
        quote.timestamp() ==
        llt::Timestamp{3000});
}