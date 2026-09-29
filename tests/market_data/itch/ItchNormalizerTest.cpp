#include <catch2/catch_test_macros.hpp>

#include "market_data/itch/ItchNormalizer.h"

#include "market_data/itch/ItchMarketState.h"
#include "market_data/itch/ItchMessage.h"
#include "market_data/itch/messages/AddOrderMessage.h"
#include "market_data/itch/messages/OrderCancelMessage.h"
#include "market_data/itch/messages/OrderDeleteMessage.h"
#include "market_data/itch/messages/OrderExecutedMessage.h"
#include "market_data/itch/messages/OrderReplaceMessage.h"


using namespace llt;

namespace
{

llt::itch::AddOrderMessage makeAddOrder(
    std::uint64_t orderId,
    std::uint32_t shares
)
{
    llt::itch::AddOrderMessage message{};

    message.stockLocate = 42;
    message.timestamp = 1000;
    message.orderReferenceNumber = orderId;
    message.buySellIndicator = 'B';
    message.shares = shares;
    message.price = 1000000;

    return message;
}

llt::itch::AddOrderMessage makeAddOrder(
    std::uint64_t orderId,
    std::uint16_t stockLocate,
    char side,
    std::uint32_t shares,
    std::uint32_t price
)
{
    llt::itch::AddOrderMessage message{};

    message.stockLocate = stockLocate;
    message.timestamp = 1000;
    message.orderReferenceNumber = orderId;
    message.buySellIndicator = side;
    message.shares = shares;
    message.price = price;

    return message;
}



llt::itch::OrderReplaceMessage makeReplace(
    std::uint64_t oldOrderId,
    std::uint64_t newOrderId,
    std::uint32_t shares,
    std::uint32_t price
)
{
    llt::itch::OrderReplaceMessage message{};

    message.timestamp = 2000;

    message.originalOrderReferenceNumber =
        oldOrderId;

    message.newOrderReferenceNumber =
        newOrderId;

    message.shares = shares;
    message.price = price;

    return message;
}


} // namespace



TEST_CASE(
    "ITCH normalizer converts Stock Directory to Instrument"
)
{
    itch::StockDirectoryMessage message;

    message.stockLocate = 42;

    message.stock = {
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' '
    };

    const auto instrument =
        itch::ItchNormalizer::normalize(message);

    REQUIRE(instrument.id == 42);

    REQUIRE(
        instrument.symbolView() ==
        "AAPL"
    );

    REQUIRE(
        instrument.tradingState ==
        market_data::TradingState::Unknown
    );
}


TEST_CASE(
    "ITCH normalizer preserves eight character symbol"
)
{
    itch::StockDirectoryMessage message;

    message.stockLocate = 123;

    message.stock = {
        'A', 'B', 'C', 'D',
        'E', 'F', 'G', 'H'
    };

    const auto instrument =
        itch::ItchNormalizer::normalize(message);

    REQUIRE(
        instrument.symbolView() ==
        "ABCDEFGH"
    );
}


TEST_CASE(
    "ITCH normalizer preserves maximum instrument id"
)
{
    itch::StockDirectoryMessage message;

    message.stockLocate =
        UINT16_MAX;

    message.stock = {
        'M', 'A', 'X', ' ',
        ' ', ' ', ' ', ' '
    };

    const auto instrument =
        itch::ItchNormalizer::normalize(message);

    REQUIRE(
        instrument.id ==
        UINT16_MAX
    );
}




TEST_CASE(
    "ITCH market state creates instrument from Stock Directory"
)
{
    itch::ItchMarketState state;

    itch::StockDirectoryMessage message;

    message.stockLocate = 42;

    message.stock = {
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' '
    };

    itch::ItchMessage event{
        message
    };

    state.onMessage(event);

    REQUIRE(
        state.instruments().size() == 1
    );

    const auto* instrument =
        state.instruments().find(42);

    REQUIRE(instrument != nullptr);

    REQUIRE(
        instrument->symbolView() ==
        "AAPL"
    );
}


TEST_CASE(
    "ITCH market state stores multiple instruments"
)
{
    itch::ItchMarketState state;

    itch::StockDirectoryMessage aapl;
    aapl.stockLocate = 10;

    aapl.stock = {
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' '
    };

    itch::StockDirectoryMessage msft;
    msft.stockLocate = 20;

    msft.stock = {
        'M', 'S', 'F', 'T',
        ' ', ' ', ' ', ' '
    };

    state.onMessage(
        itch::ItchMessage{aapl}
    );

    state.onMessage(
        itch::ItchMessage{msft}
    );

    REQUIRE(
        state.instruments().size() == 2
    );

    REQUIRE(
        state.instruments()
            .find(10)
            ->symbolView() ==
        "AAPL"
    );

    REQUIRE(
        state.instruments()
            .find(20)
            ->symbolView() ==
        "MSFT"
    );
}


TEST_CASE(
    "ITCH market state ignores non reference messages"
)
{
    itch::ItchMarketState state;

    itch::AddOrderMessage message{};

    state.onMessage(
        itch::ItchMessage{message}
    );

    REQUIRE(
        state.instruments().size() == 0
    );
}


TEST_CASE(
    "ITCH market state handles duplicate Stock Directory idempotently"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::StockDirectoryMessage message{};

    message.stockLocate = 1335;

    message.stock = {
        'C', 'F', 'G', '-', 'D',
        ' ', ' ', ' '
    };

    state.onMessage(
        llt::itch::ItchMessage{message}
    );

    state.onMessage(
        llt::itch::ItchMessage{message}
    );

    REQUIRE(
        state.instruments().size() == 1
    );

    const auto* instrument =
        state.instruments().find(1335);

    REQUIRE(instrument != nullptr);

    REQUIRE(
        instrument->symbolView() ==
        "CFG-D"
    );
}


TEST_CASE(
    "ITCH normalizer converts trading action states"
)
{
    llt::itch::StockTradingActionMessage message{};

    message.tradingState = 'H';

    REQUIRE(
        llt::itch::ItchNormalizer::
            normalizeTradingState(message) ==
        llt::market_data::TradingState::Halted
    );

    message.tradingState = 'P';

    REQUIRE(
        llt::itch::ItchNormalizer::
            normalizeTradingState(message) ==
        llt::market_data::TradingState::Paused
    );

    message.tradingState = 'Q';

    REQUIRE(
        llt::itch::ItchNormalizer::
            normalizeTradingState(message) ==
        llt::market_data::TradingState::QuotationOnly
    );

    message.tradingState = 'T';

    REQUIRE(
        llt::itch::ItchNormalizer::
            normalizeTradingState(message) ==
        llt::market_data::TradingState::Trading
    );
}


TEST_CASE(
    "ITCH normalizer maps unknown trading state to Unknown"
)
{
    llt::itch::StockTradingActionMessage message{};

    message.tradingState = '?';

    REQUIRE(
        llt::itch::ItchNormalizer::
            normalizeTradingState(message) ==
        llt::market_data::TradingState::Unknown
    );
}

TEST_CASE(
    "ITCH market state updates instrument trading state"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::StockDirectoryMessage directory{};

    directory.stockLocate = 42;

    directory.stock = {
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' '
    };

    state.onMessage(
        llt::itch::ItchMessage{
            directory
        }
    );

    const auto* before =
        state.instruments().find(42);

    REQUIRE(before != nullptr);

    REQUIRE(
        before->tradingState ==
        llt::market_data::TradingState::Unknown
    );

    llt::itch::StockTradingActionMessage action{};

    action.stockLocate = 42;
    action.tradingState = 'H';

    state.onMessage(
        llt::itch::ItchMessage{
            action
        }
    );

    const auto* after =
        state.instruments().find(42);

    REQUIRE(after != nullptr);

    REQUIRE(
        after->tradingState ==
        llt::market_data::TradingState::Halted
    );
}

TEST_CASE(
    "ITCH market state applies trading state transitions"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::StockDirectoryMessage directory{};

    directory.stockLocate = 42;

    directory.stock = {
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' '
    };

    state.onMessage(
        llt::itch::ItchMessage{
            directory
        }
    );

    llt::itch::StockTradingActionMessage action{};

    action.stockLocate = 42;

    action.tradingState = 'H';

    state.onMessage(
        llt::itch::ItchMessage{
            action
        }
    );

    REQUIRE(
        state.instruments()
            .find(42)
            ->tradingState ==
        llt::market_data::TradingState::Halted
    );

    action.tradingState = 'T';

    state.onMessage(
        llt::itch::ItchMessage{
            action
        }
    );

    REQUIRE(
        state.instruments()
            .find(42)
            ->tradingState ==
        llt::market_data::TradingState::Trading
    );
}

TEST_CASE(
    "ITCH market state ignores trading action for unknown instrument"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::StockTradingActionMessage action{};

    action.stockLocate = 999;
    action.tradingState = 'H';

    REQUIRE(
        state.instruments().size() == 0
    );

    state.onMessage(
        llt::itch::ItchMessage{
            action
        }
    );

    REQUIRE(
        state.instruments().size() == 0
    );

    REQUIRE(
        state.instruments().find(999) ==
        nullptr
    );

    REQUIRE(
        state.unknownTradingActionInstruments() == 1
    );
}


TEST_CASE(
    "ITCH normalizer converts Reg SHO restriction states"
)
{
    llt::itch::RegShoRestrictionMessage message{};

    message.regShoAction = '0';

    REQUIRE(
        llt::itch::ItchNormalizer::
            normalizeRegShoState(message) ==
        llt::market_data::RegShoState::NoRestriction
    );

    message.regShoAction = '1';

    REQUIRE(
        llt::itch::ItchNormalizer::
            normalizeRegShoState(message) ==
        llt::market_data::RegShoState::RestrictionInEffect
    );

    message.regShoAction = '2';

    REQUIRE(
        llt::itch::ItchNormalizer::
            normalizeRegShoState(message) ==
        llt::market_data::RegShoState::RestrictionRemains
    );
}


TEST_CASE(
    "ITCH normalizer maps unknown Reg SHO state to Unknown"
)
{
    llt::itch::RegShoRestrictionMessage message{};

    message.regShoAction = '?';

    REQUIRE(
        llt::itch::ItchNormalizer::
            normalizeRegShoState(message) ==
        llt::market_data::RegShoState::Unknown
    );
}


TEST_CASE(
    "ITCH market state updates instrument Reg SHO state"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::StockDirectoryMessage directory{};

    directory.stockLocate = 42;

    directory.stock = {
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' '
    };

    state.onMessage(
        llt::itch::ItchMessage{directory}
    );

    llt::itch::RegShoRestrictionMessage restriction{};

    restriction.stockLocate = 42;
    restriction.regShoAction = '1';

    state.onMessage(
        llt::itch::ItchMessage{restriction}
    );

    const auto* instrument =
        state.instruments().find(42);

    REQUIRE(instrument != nullptr);

    REQUIRE(
        instrument->regShoState ==
        llt::market_data::RegShoState::RestrictionInEffect
    );
}


TEST_CASE(
    "ITCH market state applies Reg SHO state transitions"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::StockDirectoryMessage directory{};

    directory.stockLocate = 42;

    directory.stock = {
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' '
    };

    state.onMessage(
        llt::itch::ItchMessage{directory}
    );

    llt::itch::RegShoRestrictionMessage restriction{};

    restriction.stockLocate = 42;
    restriction.regShoAction = '1';

    state.onMessage(
        llt::itch::ItchMessage{restriction}
    );

    REQUIRE(
        state.instruments()
            .find(42)
            ->regShoState ==
        llt::market_data::RegShoState::RestrictionInEffect
    );

    restriction.regShoAction = '2';

    state.onMessage(
        llt::itch::ItchMessage{restriction}
    );

    REQUIRE(
        state.instruments()
            .find(42)
            ->regShoState ==
        llt::market_data::RegShoState::RestrictionRemains
    );
}


TEST_CASE(
    "ITCH market state ignores Reg SHO update for unknown instrument"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::RegShoRestrictionMessage restriction{};

    restriction.stockLocate = 999;
    restriction.regShoAction = '1';

    state.onMessage(
        llt::itch::ItchMessage{restriction}
    );

    REQUIRE(
        state.instruments().size() == 0
    );

    REQUIRE(
        state.instruments().find(999) ==
        nullptr
    );

    REQUIRE(
        state.unknownRegShoInstruments() == 1
    );
}

TEST_CASE(
    "ITCH normalizer converts Add Order to normalized Order"
)
{
    llt::itch::AddOrderMessage message{};

    message.stockLocate = 42;
    message.timestamp = 123456;
    message.orderReferenceNumber = 1001;
    message.buySellIndicator = 'B';
    message.shares = 500;
    message.price = 1872500;

    const auto order =
        llt::itch::ItchNormalizer::
            normalizeOrder(message);

    REQUIRE(order.orderId == 1001);
    REQUIRE(order.instrumentId == 42);
    REQUIRE(order.timestamp == 123456);
    REQUIRE(order.quantity == 500);
    REQUIRE(order.price == 1872500);
    REQUIRE(
        order.side ==
        llt::market_data::Side::Buy
    );
}


TEST_CASE(
    "ITCH normalizer converts Add Order with MPID to normalized Order"
)
{
    llt::itch::AddOrderWithMpidMessage message{};

    message.stockLocate = 42;
    message.timestamp = 123456;
    message.orderReferenceNumber = 2001;
    message.buySellIndicator = 'S';
    message.shares = 250;
    message.price = 1873000;

    const auto order =
        llt::itch::ItchNormalizer::
            normalizeOrder(message);

    REQUIRE(order.orderId == 2001);
    REQUIRE(order.instrumentId == 42);
    REQUIRE(order.timestamp == 123456);
    REQUIRE(order.quantity == 250);
    REQUIRE(order.price == 1873000);

    REQUIRE(
        order.side ==
        llt::market_data::Side::Sell
    );
}


TEST_CASE(
    "ITCH market state stores Add Order"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::AddOrderMessage message{};

    message.stockLocate = 42;
    message.timestamp = 1000;
    message.orderReferenceNumber = 100;
    message.buySellIndicator = 'B';
    message.shares = 500;
    message.price = 1872500;

    state.onMessage(
        llt::itch::ItchMessage{message}
    );

    REQUIRE(state.orders().size() == 1);

    const auto* order =
        state.orders().find(100);

    REQUIRE(order != nullptr);
    REQUIRE(order->instrumentId == 42);
    REQUIRE(order->quantity == 500);
    REQUIRE(order->price == 1872500);

    REQUIRE(
        order->side ==
        llt::market_data::Side::Buy
    );
}

TEST_CASE(
    "ITCH market state stores Add Order with MPID"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::AddOrderWithMpidMessage message{};

    message.stockLocate = 77;
    message.timestamp = 2000;
    message.orderReferenceNumber = 200;
    message.buySellIndicator = 'S';
    message.shares = 300;
    message.price = 995000;

    state.onMessage(
        llt::itch::ItchMessage{message}
    );

    REQUIRE(state.orders().size() == 1);

    const auto* order =
        state.orders().find(200);

    REQUIRE(order != nullptr);
    REQUIRE(order->instrumentId == 77);
    REQUIRE(order->quantity == 300);
    REQUIRE(order->price == 995000);

    REQUIRE(
        order->side ==
        llt::market_data::Side::Sell
    );
}


TEST_CASE(
    "ITCH market state partially executes order"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::AddOrderMessage add{};

    add.stockLocate = 42;
    add.orderReferenceNumber = 100;
    add.buySellIndicator = 'B';
    add.shares = 500;
    add.price = 1872500;

    state.onMessage(
        llt::itch::ItchMessage{add}
    );

    llt::itch::OrderExecutedMessage execution{};

    execution.orderReferenceNumber = 100;
    execution.executedShares = 200;

    state.onMessage(
        llt::itch::ItchMessage{execution}
    );

    const auto* order =
        state.orders().find(100);

    REQUIRE(order != nullptr);
    REQUIRE(order->quantity == 300);
    REQUIRE(state.orders().size() == 1);
}


TEST_CASE(
    "ITCH market state removes fully executed order"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::AddOrderMessage add{};

    add.orderReferenceNumber = 100;
    add.buySellIndicator = 'B';
    add.shares = 500;
    add.price = 1872500;

    state.onMessage(
        llt::itch::ItchMessage{add}
    );

    llt::itch::OrderExecutedMessage execution{};

    execution.orderReferenceNumber = 100;
    execution.executedShares = 500;

    state.onMessage(
        llt::itch::ItchMessage{execution}
    );

    REQUIRE(
        state.orders().find(100) ==
        nullptr
    );

    REQUIRE(state.orders().size() == 0);
}

TEST_CASE(
    "ITCH market state counts execution for unknown order"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::OrderExecutedMessage execution{};

    execution.orderReferenceNumber = 999;
    execution.executedShares = 100;

    state.onMessage(
        llt::itch::ItchMessage{execution}
    );

    REQUIRE(
        state.unknownOrderExecutions() == 1
    );

    REQUIRE(state.orders().size() == 0);
}

TEST_CASE(
    "ITCH market state rejects execution larger than remaining quantity"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::AddOrderMessage add{};

    add.orderReferenceNumber = 100;
    add.buySellIndicator = 'B';
    add.shares = 500;
    add.price = 1872500;

    state.onMessage(
        llt::itch::ItchMessage{add}
    );

    llt::itch::OrderExecutedMessage execution{};

    execution.orderReferenceNumber = 100;
    execution.executedShares = 501;

    state.onMessage(
        llt::itch::ItchMessage{execution}
    );

    REQUIRE(
        state.overExecutedOrders() == 1
    );

    // Invalid mutation must not corrupt the order.
    const auto* order =
        state.orders().find(100);

    REQUIRE(order != nullptr);
    REQUIRE(order->quantity == 500);
}



TEST_CASE(
    "ITCH market state partially executes order with price"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::AddOrderMessage add{};

    add.orderReferenceNumber = 100;
    add.buySellIndicator = 'B';
    add.shares = 500;
    add.price = 1000000;

    state.onMessage(
        llt::itch::ItchMessage{add}
    );

    llt::itch::OrderExecutedWithPriceMessage execution{};

    execution.orderReferenceNumber = 100;
    execution.executedShares = 200;
    execution.executionPrice = 999500;

    state.onMessage(
        llt::itch::ItchMessage{execution}
    );

    const auto* order =
        state.orders().find(100);

    REQUIRE(order != nullptr);
    REQUIRE(order->quantity == 300);

    // Execution price must NOT mutate resting price.
    REQUIRE(order->price == 1000000);
}

TEST_CASE(
    "ITCH market state removes fully executed order with price"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::AddOrderMessage add{};

    add.orderReferenceNumber = 100;
    add.buySellIndicator = 'S';
    add.shares = 500;
    add.price = 1000000;

    state.onMessage(
        llt::itch::ItchMessage{add}
    );

    llt::itch::OrderExecutedWithPriceMessage execution{};

    execution.orderReferenceNumber = 100;
    execution.executedShares = 500;
    execution.executionPrice = 1000500;

    state.onMessage(
        llt::itch::ItchMessage{execution}
    );

    REQUIRE(
        state.orders().find(100) == nullptr
    );

    REQUIRE(state.orders().size() == 0);
}

TEST_CASE(
    "ITCH market state counts priced execution for unknown order"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::OrderExecutedWithPriceMessage execution{};

    execution.orderReferenceNumber = 999;
    execution.executedShares = 100;
    execution.executionPrice = 1000000;

    state.onMessage(
        llt::itch::ItchMessage{execution}
    );

    REQUIRE(
        state.unknownOrderExecutionsWithPrice() == 1
    );

    REQUIRE(state.orders().size() == 0);
}

TEST_CASE(
    "ITCH market state rejects priced execution larger than remaining quantity"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::AddOrderMessage add{};

    add.orderReferenceNumber = 100;
    add.buySellIndicator = 'B';
    add.shares = 500;
    add.price = 1000000;

    state.onMessage(
        llt::itch::ItchMessage{add}
    );

    llt::itch::OrderExecutedWithPriceMessage execution{};

    execution.orderReferenceNumber = 100;
    execution.executedShares = 501;
    execution.executionPrice = 999500;

    state.onMessage(
        llt::itch::ItchMessage{execution}
    );

    REQUIRE(
        state.overExecutedOrdersWithPrice() == 1
    );

    const auto* order =
        state.orders().find(100);

    REQUIRE(order != nullptr);
    REQUIRE(order->quantity == 500);
    REQUIRE(order->price == 1000000);
}





TEST_CASE(
    "ITCH market state partially cancels order"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAddOrder(100, 500)
        }
    );

    llt::itch::OrderCancelMessage cancel{};

    cancel.orderReferenceNumber = 100;
    cancel.cancelledShares = 200;

    state.onMessage(
        llt::itch::ItchMessage{cancel}
    );

    const auto* order =
        state.orders().find(100);

    REQUIRE(order != nullptr);
    REQUIRE(order->quantity == 300);
    REQUIRE(state.orders().size() == 1);
}


TEST_CASE(
    "ITCH market state removes fully cancelled order"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAddOrder(100, 500)
        }
    );

    llt::itch::OrderCancelMessage cancel{};

    cancel.orderReferenceNumber = 100;
    cancel.cancelledShares = 500;

    state.onMessage(
        llt::itch::ItchMessage{cancel}
    );

    REQUIRE(
        state.orders().find(100) ==
        nullptr
    );

    REQUIRE(state.orders().size() == 0);
}


TEST_CASE(
    "ITCH market state counts cancel for unknown order"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::OrderCancelMessage cancel{};

    cancel.orderReferenceNumber = 999;
    cancel.cancelledShares = 100;

    state.onMessage(
        llt::itch::ItchMessage{cancel}
    );

    REQUIRE(
        state.unknownOrderCancels() == 1
    );

    REQUIRE(
        state.overCancelledOrders() == 0
    );

    REQUIRE(state.orders().size() == 0);
}


TEST_CASE(
    "ITCH market state rejects cancel larger than remaining quantity"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAddOrder(100, 500)
        }
    );

    llt::itch::OrderCancelMessage cancel{};

    cancel.orderReferenceNumber = 100;
    cancel.cancelledShares = 501;

    state.onMessage(
        llt::itch::ItchMessage{cancel}
    );

    REQUIRE(
        state.overCancelledOrders() == 1
    );

    REQUIRE(
        state.unknownOrderCancels() == 0
    );

    // Invalid X must not corrupt the order.
    const auto* order =
        state.orders().find(100);

    REQUIRE(order != nullptr);
    REQUIRE(order->quantity == 500);
    REQUIRE(state.orders().size() == 1);
}


TEST_CASE(
    "ITCH market state applies multiple cancellations"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAddOrder(100, 1000)
        }
    );

    llt::itch::OrderCancelMessage first{};

    first.orderReferenceNumber = 100;
    first.cancelledShares = 200;

    state.onMessage(
        llt::itch::ItchMessage{first}
    );

    REQUIRE(
        state.orders().find(100)->quantity ==
        800
    );

    llt::itch::OrderCancelMessage second{};

    second.orderReferenceNumber = 100;
    second.cancelledShares = 300;

    state.onMessage(
        llt::itch::ItchMessage{second}
    );

    const auto* order =
        state.orders().find(100);

    REQUIRE(order != nullptr);
    REQUIRE(order->quantity == 500);
}


TEST_CASE(
    "ITCH market state can execute then cancel same order"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAddOrder(100, 1000)
        }
    );

    llt::itch::OrderExecutedMessage execution{};

    execution.orderReferenceNumber = 100;
    execution.executedShares = 300;

    state.onMessage(
        llt::itch::ItchMessage{execution}
    );

    REQUIRE(
        state.orders().find(100)->quantity ==
        700
    );

    llt::itch::OrderCancelMessage cancel{};

    cancel.orderReferenceNumber = 100;
    cancel.cancelledShares = 200;

    state.onMessage(
        llt::itch::ItchMessage{cancel}
    );

    const auto* order =
        state.orders().find(100);

    REQUIRE(order != nullptr);
    REQUIRE(order->quantity == 500);

    REQUIRE(
        state.unknownOrderExecutions() == 0
    );

    REQUIRE(
        state.unknownOrderCancels() == 0
    );
}






// namespace
// {

// llt::itch::AddOrderMessage makeAddOrder(
//     std::uint64_t orderId,
//     std::uint32_t shares
// )
// {
//     llt::itch::AddOrderMessage message{};

//     message.stockLocate = 42;
//     message.timestamp = 1000;
//     message.orderReferenceNumber = orderId;
//     message.buySellIndicator = 'B';
//     message.shares = shares;
//     message.price = 1000000;

//     return message;
// }

// } // namespace


TEST_CASE(
    "ITCH market state deletes order"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAddOrder(100, 500)
        }
    );

    REQUIRE(state.orders().size() == 1);
    REQUIRE(state.orders().find(100) != nullptr);

    llt::itch::OrderDeleteMessage deletion{};

    deletion.orderReferenceNumber = 100;

    state.onMessage(
        llt::itch::ItchMessage{deletion}
    );

    REQUIRE(state.orders().size() == 0);

    REQUIRE(
        state.orders().find(100) ==
        nullptr
    );

    REQUIRE(
        state.unknownOrderDeletes() == 0
    );
}


TEST_CASE(
    "ITCH market state counts delete for unknown order"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::OrderDeleteMessage deletion{};

    deletion.orderReferenceNumber = 999;

    state.onMessage(
        llt::itch::ItchMessage{deletion}
    );

    REQUIRE(state.orders().size() == 0);

    REQUIRE(
        state.unknownOrderDeletes() == 1
    );
}


TEST_CASE(
    "ITCH market state deletes partially executed order"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAddOrder(100, 1000)
        }
    );

    llt::itch::OrderExecutedMessage execution{};

    execution.orderReferenceNumber = 100;
    execution.executedShares = 300;

    state.onMessage(
        llt::itch::ItchMessage{execution}
    );

    const auto* remaining =
        state.orders().find(100);

    REQUIRE(remaining != nullptr);
    REQUIRE(remaining->quantity == 700);

    llt::itch::OrderDeleteMessage deletion{};

    deletion.orderReferenceNumber = 100;

    state.onMessage(
        llt::itch::ItchMessage{deletion}
    );

    REQUIRE(
        state.orders().find(100) ==
        nullptr
    );

    REQUIRE(state.orders().size() == 0);
    REQUIRE(state.unknownOrderDeletes() == 0);
}


TEST_CASE(
    "ITCH market state deletes partially cancelled order"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAddOrder(100, 1000)
        }
    );

    llt::itch::OrderCancelMessage cancel{};

    cancel.orderReferenceNumber = 100;
    cancel.cancelledShares = 250;

    state.onMessage(
        llt::itch::ItchMessage{cancel}
    );

    const auto* remaining =
        state.orders().find(100);

    REQUIRE(remaining != nullptr);
    REQUIRE(remaining->quantity == 750);

    llt::itch::OrderDeleteMessage deletion{};

    deletion.orderReferenceNumber = 100;

    state.onMessage(
        llt::itch::ItchMessage{deletion}
    );

    REQUIRE(
        state.orders().find(100) ==
        nullptr
    );

    REQUIRE(state.orders().size() == 0);
    REQUIRE(state.unknownOrderDeletes() == 0);
}


TEST_CASE(
    "ITCH market state deleting one order does not affect another"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAddOrder(100, 500)
        }
    );

    state.onMessage(
        llt::itch::ItchMessage{
            makeAddOrder(200, 750)
        }
    );

    REQUIRE(state.orders().size() == 2);

    llt::itch::OrderDeleteMessage deletion{};

    deletion.orderReferenceNumber = 100;

    state.onMessage(
        llt::itch::ItchMessage{deletion}
    );

    REQUIRE(state.orders().size() == 1);

    REQUIRE(
        state.orders().find(100) ==
        nullptr
    );

    const auto* surviving =
        state.orders().find(200);

    REQUIRE(surviving != nullptr);
    REQUIRE(surviving->quantity == 750);
}




TEST_CASE(
    "ITCH market state replaces order"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAddOrder(
                100,
                42,
                'B',
                500,
                1000000
            )
        }
    );

    state.onMessage(
        llt::itch::ItchMessage{
            makeReplace(
                100,
                200,
                700,
                1002500
            )
        }
    );

    REQUIRE(state.orders().size() == 1);

    REQUIRE(
        state.orders().find(100) ==
        nullptr
    );

    const auto* replacement =
        state.orders().find(200);

    REQUIRE(replacement != nullptr);

    REQUIRE(replacement->orderId == 200);
    REQUIRE(replacement->instrumentId == 42);
    REQUIRE(replacement->side == llt::market_data::Side::Buy);
    REQUIRE(replacement->quantity == 700);
    REQUIRE(replacement->price == 1002500);
    REQUIRE(replacement->timestamp == 2000);

    REQUIRE(
        state.unknownOrderReplaces() == 0
    );

    REQUIRE(
        state.duplicateReplacementOrderIds() == 0
    );
}


TEST_CASE(
    "ITCH order replace preserves sell side"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAddOrder(
                100,
                77,
                'S',
                400,
                2000000
            )
        }
    );

    state.onMessage(
        llt::itch::ItchMessage{
            makeReplace(
                100,
                200,
                300,
                1995000
            )
        }
    );

    const auto* replacement =
        state.orders().find(200);

    REQUIRE(replacement != nullptr);

    REQUIRE(
        replacement->side ==
        llt::market_data::Side::Sell
    );

    REQUIRE(replacement->instrumentId == 77);
    REQUIRE(replacement->quantity == 300);
    REQUIRE(replacement->price == 1995000);
}


TEST_CASE(
    "ITCH market state counts replace for unknown order"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeReplace(
                999,
                1000,
                500,
                1000000
            )
        }
    );

    REQUIRE(state.orders().size() == 0);

    REQUIRE(
        state.unknownOrderReplaces() == 1
    );

    REQUIRE(
        state.duplicateReplacementOrderIds() == 0
    );
}


TEST_CASE(
    "ITCH market state rejects replacement when new order id already exists"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAddOrder(
                100,
                42,
                'B',
                500,
                1000000
            )
        }
    );

    state.onMessage(
        llt::itch::ItchMessage{
            makeAddOrder(
                200,
                77,
                'S',
                300,
                2000000
            )
        }
    );

    state.onMessage(
        llt::itch::ItchMessage{
            makeReplace(
                100,
                200,
                700,
                1002500
            )
        }
    );

    REQUIRE(state.orders().size() == 2);

    // Old order must remain intact.
    const auto* original =
        state.orders().find(100);

    REQUIRE(original != nullptr);
    REQUIRE(original->instrumentId == 42);
    REQUIRE(original->quantity == 500);
    REQUIRE(original->price == 1000000);

    // Existing ID 200 must also remain intact.
    const auto* existing =
        state.orders().find(200);

    REQUIRE(existing != nullptr);
    REQUIRE(existing->instrumentId == 77);
    REQUIRE(existing->quantity == 300);
    REQUIRE(existing->price == 2000000);

    REQUIRE(
        state.duplicateReplacementOrderIds() == 1
    );
}


TEST_CASE(
    "ITCH replacement order participates in later lifecycle"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAddOrder(
                100,
                42,
                'B',
                1000,
                1000000
            )
        }
    );

    state.onMessage(
        llt::itch::ItchMessage{
            makeReplace(
                100,
                200,
                800,
                1002500
            )
        }
    );

    llt::itch::OrderExecutedMessage execution{};

    execution.orderReferenceNumber = 200;
    execution.executedShares = 300;

    state.onMessage(
        llt::itch::ItchMessage{
            execution
        }
    );

    const auto* order =
        state.orders().find(200);

    REQUIRE(order != nullptr);
    REQUIRE(order->quantity == 500);
    REQUIRE(order->price == 1002500);

    REQUIRE(
        state.orders().find(100) ==
        nullptr
    );

    REQUIRE(
        state.unknownOrderExecutions() == 0
    );
}