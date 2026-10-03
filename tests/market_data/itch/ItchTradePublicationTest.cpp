#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <variant>
#include <vector>

#include "market_data/MarketEvent.h"
#include "market_data/MarketEventQueue.h"
#include "market_data/Trade.h"

#include "market_data/itch/ItchMarketState.h"
#include "market_data/itch/ItchMessage.h"
#include "market_data/itch/ItchTradePublisher.h"

#include "market_data/itch/messages/StockDirectoryMessage.h"
#include "market_data/itch/messages/AddOrderMessage.h"
#include "market_data/itch/messages/OrderExecutedMessage.h"
#include "market_data/itch/messages/OrderExecutedWithPriceMessage.h"
#include "market_data/itch/messages/TradeMessage.h"

namespace
{

struct TradeNotification
{
    llt::market_data::InstrumentId instrumentId{0};
    llt::market_data::Timestamp timestamp{0};
    llt::market_data::Price price{0};
    llt::market_data::Quantity quantity{0};
    llt::market_data::Side side{
        llt::market_data::Side::Buy};
};


llt::itch::StockDirectoryMessage
makeStockDirectory(
    std::uint16_t instrumentId,
    const char* symbol)
{
    llt::itch::StockDirectoryMessage message{};

    message.stockLocate =
        instrumentId;

    //
    // ITCH stock symbols are fixed-width and
    // space padded.
    //
    for (auto& c : message.stock)
    {
        c = ' ';
    }

    std::size_t i = 0;

    while (
        symbol[i] != '\0' &&
        i < message.stock.size())
    {
        message.stock[i] =
            symbol[i];

        ++i;
    }

    return message;
}


llt::itch::AddOrderMessage
makeAdd(
    std::uint64_t orderId,
    std::uint16_t instrumentId,
    char side,
    std::uint32_t quantity,
    std::uint32_t price,
    std::uint64_t timestamp = 1000)
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


llt::itch::OrderExecutedMessage
makeExecution(
    std::uint64_t orderId,
    std::uint32_t quantity,
    std::uint64_t timestamp = 2000)
{
    llt::itch::OrderExecutedMessage message{};

    message.timestamp =
        timestamp;

    message.orderReferenceNumber =
        orderId;

    message.executedShares =
        quantity;

    message.matchNumber =
        123456;

    return message;
}


llt::itch::OrderExecutedWithPriceMessage
makeExecutionWithPrice(
    std::uint64_t orderId,
    std::uint32_t quantity,
    std::uint32_t executionPrice,
    std::uint64_t timestamp = 3000)
{
    llt::itch::OrderExecutedWithPriceMessage message{};

    message.timestamp =
        timestamp;

    message.orderReferenceNumber =
        orderId;

    message.executedShares =
        quantity;

    message.matchNumber =
        654321;

    message.printable =
        'Y';

    message.executionPrice =
        executionPrice;

    return message;
}


llt::itch::TradeMessage
makeTrade(
    std::uint16_t instrumentId,
    char side,
    std::uint32_t quantity,
    std::uint32_t price,
    std::uint64_t timestamp = 4000)
{
    llt::itch::TradeMessage message{};

    message.stockLocate =
        instrumentId;

    message.timestamp =
        timestamp;

    message.orderReferenceNumber =
        999999;

    message.buySellIndicator =
        side;

    message.shares =
        quantity;

    message.price =
        price;

    message.matchNumber =
        777777;

    return message;
}


void attachTradeRecorder(
    llt::itch::ItchMarketState& state,
    std::vector<TradeNotification>& notifications)
{
    state.setTradeHandler(
        [&notifications](
            llt::market_data::InstrumentId instrumentId,
            llt::market_data::Timestamp timestamp,
            llt::market_data::Price price,
            llt::market_data::Quantity quantity,
            llt::market_data::Side side)
        {
            notifications.push_back(
                TradeNotification{
                    instrumentId,
                    timestamp,
                    price,
                    quantity,
                    side});
        });
}

} // namespace


TEST_CASE(
    "ITCH E execution publishes trade using resting order data")
{
    std::vector<TradeNotification>
        notifications;

    llt::itch::ItchMarketState state;

    attachTradeRecorder(
        state,
        notifications);

    //
    // Resting SELL order:
    //
    // 100 shares @ 100.0000
    //
    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(
                100,
                42,
                'S',
                100,
                1000000)});

    //
    // Execute 40 shares.
    //
    state.onMessage(
        llt::itch::ItchMessage{
            makeExecution(
                100,
                40,
                2000)});

    REQUIRE(
        notifications.size() == 1);

    const auto& trade =
        notifications.front();

    REQUIRE(
        trade.instrumentId == 42);

    REQUIRE(
        trade.timestamp == 2000);

    //
    // E has no explicit execution price,
    // therefore we use the resting order price.
    //
    REQUIRE(
        trade.price == 1000000);

    REQUIRE(
        trade.quantity == 40);

    REQUIRE(
        trade.side ==
        llt::market_data::Side::Sell);

    //
    // Partial execution:
    // 100 - 40 = 60 remaining.
    //
    const auto* order =
        state.orders().find(100);

    REQUIRE(order != nullptr);

    REQUIRE(
        order->quantity == 60);
}


TEST_CASE(
    "ITCH full E execution publishes trade and removes order")
{
    std::vector<TradeNotification>
        notifications;

    llt::itch::ItchMarketState state;

    attachTradeRecorder(
        state,
        notifications);

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(
                101,
                42,
                'B',
                100,
                999900)});

    state.onMessage(
        llt::itch::ItchMessage{
            makeExecution(
                101,
                100,
                2100)});

    REQUIRE(
        notifications.size() == 1);

    REQUIRE(
        notifications.front().price ==
        999900);

    REQUIRE(
        notifications.front().quantity ==
        100);

    REQUIRE(
        notifications.front().side ==
        llt::market_data::Side::Buy);

    //
    // Entire order was executed.
    //
    REQUIRE(
        state.orders().find(101) ==
        nullptr);
}


TEST_CASE(
    "ITCH C execution publishes trade using explicit execution price")
{
    std::vector<TradeNotification>
        notifications;

    llt::itch::ItchMarketState state;

    attachTradeRecorder(
        state,
        notifications);

    //
    // Resting order price:
    // 100.0000
    //
    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(
                200,
                55,
                'S',
                100,
                1000000)});

    //
    // Execution price:
    // 100.2500
    //
    state.onMessage(
        llt::itch::ItchMessage{
            makeExecutionWithPrice(
                200,
                40,
                1002500,
                3000)});

    REQUIRE(
        notifications.size() == 1);

    const auto& trade =
        notifications.front();

    REQUIRE(
        trade.instrumentId == 55);

    REQUIRE(
        trade.timestamp == 3000);

    //
    // Critical test:
    //
    // C must use executionPrice,
    // NOT the resting order's price.
    //
    REQUIRE(
        trade.price == 1002500);

    REQUIRE(
        trade.price != 1000000);

    REQUIRE(
        trade.quantity == 40);

    REQUIRE(
        trade.side ==
        llt::market_data::Side::Sell);

    //
    // Book/order reduction still happens
    // against the original resting order.
    //
    const auto* order =
        state.orders().find(200);

    REQUIRE(order != nullptr);

    REQUIRE(
        order->price == 1000000);

    REQUIRE(
        order->quantity == 60);
}


TEST_CASE(
    "ITCH P message publishes trade without modifying order state")
{
    std::vector<TradeNotification>
        notifications;

    llt::itch::ItchMarketState state;

    attachTradeRecorder(
        state,
        notifications);

    const auto ordersBefore =
        state.orders().size();

    const auto booksBefore =
        state.books().size();

    state.onMessage(
        llt::itch::ItchMessage{
            makeTrade(
                77,
                'B',
                250,
                1505000,
                4000)});

    REQUIRE(
        notifications.size() == 1);

    const auto& trade =
        notifications.front();

    REQUIRE(
        trade.instrumentId == 77);

    REQUIRE(
        trade.timestamp == 4000);

    REQUIRE(
        trade.price == 1505000);

    REQUIRE(
        trade.quantity == 250);

    REQUIRE(
        trade.side ==
        llt::market_data::Side::Buy);

    //
    // P is a trade report.
    //
    // It must NOT mutate our reconstructed
    // displayed order book.
    //
    REQUIRE(
        state.orders().size() ==
        ordersBefore);

    REQUIRE(
        state.books().size() ==
        booksBefore);
}


TEST_CASE(
    "Unknown ITCH E order does not publish trade")
{
    std::vector<TradeNotification>
        notifications;

    llt::itch::ItchMarketState state;

    attachTradeRecorder(
        state,
        notifications);

    state.onMessage(
        llt::itch::ItchMessage{
            makeExecution(
                999999,
                10)});

    REQUIRE(
        notifications.empty());

    REQUIRE(
        state.unknownOrderExecutions() ==
        1);
}


TEST_CASE(
    "Unknown ITCH C order does not publish trade")
{
    std::vector<TradeNotification>
        notifications;

    llt::itch::ItchMarketState state;

    attachTradeRecorder(
        state,
        notifications);

    state.onMessage(
        llt::itch::ItchMessage{
            makeExecutionWithPrice(
                999999,
                10,
                1000000)});

    REQUIRE(
        notifications.empty());

    REQUIRE(
        state
            .unknownOrderExecutionsWithPrice()
        == 1);
}


TEST_CASE(
    "Over-executed ITCH E order does not publish trade")
{
    std::vector<TradeNotification>
        notifications;

    llt::itch::ItchMarketState state;

    attachTradeRecorder(
        state,
        notifications);

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(
                300,
                88,
                'B',
                100,
                2000000)});

    //
    // Impossible:
    // only 100 shares exist.
    //
    state.onMessage(
        llt::itch::ItchMessage{
            makeExecution(
                300,
                101)});

    REQUIRE(
        notifications.empty());

    REQUIRE(
        state.overExecutedOrders() ==
        1);

    const auto* order =
        state.orders().find(300);

    REQUIRE(order != nullptr);

    REQUIRE(
        order->quantity == 100);
}


TEST_CASE(
    "Over-executed ITCH C order does not publish trade")
{
    std::vector<TradeNotification>
        notifications;

    llt::itch::ItchMarketState state;

    attachTradeRecorder(
        state,
        notifications);

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(
                301,
                88,
                'S',
                100,
                2000000)});

    state.onMessage(
        llt::itch::ItchMessage{
            makeExecutionWithPrice(
                301,
                101,
                2001000)});

    REQUIRE(
        notifications.empty());

    REQUIRE(
        state
            .overExecutedOrdersWithPrice()
        == 1);

    const auto* order =
        state.orders().find(301);

    REQUIRE(order != nullptr);

    REQUIRE(
        order->quantity == 100);
}


TEST_CASE(
    "ItchTradePublisher publishes normalized Trade into MarketEventQueue")
{
    llt::itch::ItchMarketState state;

    llt::MarketEventQueue queue;

    llt::itch::ItchTradePublisher publisher{
        state.instruments(),
        queue};

    //
    // Register AAPL so publisher can resolve
    // instrument ID -> symbol.
    //
    state.onMessage(
        llt::itch::ItchMessage{
            makeStockDirectory(
                42,
                "AAPL")});

    publisher.onOrderExecution(
        42,
        5000,
        1234500,
        75,
        llt::market_data::Side::Buy);

    REQUIRE(
        publisher.publishedTrades() ==
        1);

    REQUIRE(
        publisher.droppedTrades() ==
        0);

    REQUIRE(
        publisher.unknownInstruments() ==
        0);

    auto event =
        queue.pop();

    REQUIRE(
        event.has_value());

    REQUIRE(
        std::holds_alternative<llt::Trade>(
            *event));

    const auto& trade =
        std::get<llt::Trade>(
            *event);

    REQUIRE(
        trade.instrument() ==
        llt::Instrument("AAPL"));

    REQUIRE(
        trade.sequence() ==
        llt::SequenceNumber(0));

    REQUIRE(
        trade.timestamp() ==
        llt::Timestamp(5000));

    REQUIRE(
        trade.price() ==
        llt::Price(1234500));

    REQUIRE(
        trade.quantity() ==
        llt::Quantity(75));

    REQUIRE(
        trade.side() ==
        llt::Side::Buy);

    REQUIRE(
        queue.empty());
}


TEST_CASE(
    "ItchTradePublisher rejects unknown instrument")
{
    llt::itch::ItchMarketState state;

    llt::MarketEventQueue queue;

    llt::itch::ItchTradePublisher publisher{
        state.instruments(),
        queue};

    //
    // Instrument 999 was never introduced
    // by a Stock Directory message.
    //
    publisher.onOrderExecution(
        999,
        6000,
        1000000,
        10,
        llt::market_data::Side::Sell);

    REQUIRE(
        publisher.publishedTrades() ==
        0);

    REQUIRE(
        publisher.unknownInstruments() ==
        1);

    REQUIRE(
        queue.empty());
}


TEST_CASE(
    "ItchMarketState and ItchTradePublisher publish E end to end")
{
    llt::itch::ItchMarketState state;

    llt::MarketEventQueue queue;

    llt::itch::ItchTradePublisher publisher{
        state.instruments(),
        queue};

    state.setTradeHandler(
        [&publisher](
            llt::market_data::InstrumentId instrumentId,
            llt::market_data::Timestamp timestamp,
            llt::market_data::Price price,
            llt::market_data::Quantity quantity,
            llt::market_data::Side side)
        {
            publisher.onOrderExecution(
                instrumentId,
                timestamp,
                price,
                quantity,
                side);
        });

    //
    // AAPL exists.
    //
    state.onMessage(
        llt::itch::ItchMessage{
            makeStockDirectory(
                42,
                "AAPL")});

    //
    // Resting ask.
    //
    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(
                500,
                42,
                'S',
                100,
                1010000)});

    //
    // 25 shares execute.
    //
    state.onMessage(
        llt::itch::ItchMessage{
            makeExecution(
                500,
                25,
                7000)});

    REQUIRE(
        publisher.publishedTrades() ==
        1);

    auto event =
        queue.pop();

    REQUIRE(
        event.has_value());

    REQUIRE(
        std::holds_alternative<llt::Trade>(
            *event));

    const auto& trade =
        std::get<llt::Trade>(
            *event);

    REQUIRE(
        trade.instrument() ==
        llt::Instrument("AAPL"));

    REQUIRE(
        trade.timestamp() ==
        llt::Timestamp(7000));

    REQUIRE(
        trade.price() ==
        llt::Price(1010000));

    REQUIRE(
        trade.quantity() ==
        llt::Quantity(25));

    REQUIRE(
        trade.side() ==
        llt::Side::Sell);

    //
    // And market state was also updated.
    //
    const auto* remaining =
        state.orders().find(500);

    REQUIRE(
        remaining != nullptr);

    REQUIRE(
        remaining->quantity == 75);

    REQUIRE(
        queue.empty());
}