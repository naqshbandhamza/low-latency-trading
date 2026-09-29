#include <catch2/catch_test_macros.hpp>

#include <cstdint>

#include "market_data/itch/ItchMarketState.h"
#include "market_data/itch/ItchMessage.h"

#include "market_data/itch/messages/AddOrderMessage.h"
#include "market_data/itch/messages/OrderExecutedMessage.h"
#include "market_data/itch/messages/OrderExecutedWithPriceMessage.h"
#include "market_data/itch/messages/OrderCancelMessage.h"
#include "market_data/itch/messages/OrderDeleteMessage.h"
#include "market_data/itch/messages/OrderReplaceMessage.h"

namespace
{

llt::itch::AddOrderMessage makeAdd(
    std::uint64_t id,
    std::uint16_t instrument,
    char side,
    std::uint32_t quantity,
    std::uint32_t price
)
{
    llt::itch::AddOrderMessage message{};

    message.stockLocate = instrument;
    message.timestamp = 1000;
    message.orderReferenceNumber = id;
    message.buySellIndicator = side;
    message.shares = quantity;
    message.price = price;

    return message;
}

} // namespace


TEST_CASE(
    "Order execution reduces book quantity"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(100, 42, 'B', 500, 1000000)
        }
    );

    llt::itch::OrderExecutedMessage execution{};

    execution.orderReferenceNumber = 100;
    execution.executedShares = 200;

    state.onMessage(
        llt::itch::ItchMessage{execution}
    );

    const auto* level =
        state.books()
            .find(42)
            ->bids()
            .find(1000000);

    REQUIRE(level != nullptr);
    REQUIRE(level->quantity == 300);
    REQUIRE(level->orderCount == 1);

    REQUIRE(
        state.orders().find(100)->quantity ==
        300
    );
}


TEST_CASE(
    "Full order execution removes price level"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(100, 42, 'B', 500, 1000000)
        }
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

    REQUIRE(
        state.books()
            .find(42)
            ->bids()
            .find(1000000) ==
        nullptr
    );
}


TEST_CASE(
    "Priced execution reduces resting price level"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(100, 42, 'S', 500, 1000100)
        }
    );

    llt::itch::OrderExecutedWithPriceMessage execution{};

    execution.orderReferenceNumber = 100;
    execution.executedShares = 200;

    // Different execution price.
    execution.executionPrice = 999900;

    state.onMessage(
        llt::itch::ItchMessage{execution}
    );

    const auto* level =
        state.books()
            .find(42)
            ->asks()
            .find(1000100);

    REQUIRE(level != nullptr);
    REQUIRE(level->quantity == 300);

    // Resting level remains at original price.
    REQUIRE(level->price == 1000100);
}


TEST_CASE(
    "Order cancel reduces book quantity"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(100, 42, 'B', 1000, 1000000)
        }
    );

    llt::itch::OrderCancelMessage cancel{};

    cancel.orderReferenceNumber = 100;
    cancel.cancelledShares = 300;

    state.onMessage(
        llt::itch::ItchMessage{cancel}
    );

    const auto* level =
        state.books()
            .find(42)
            ->bids()
            .find(1000000);

    REQUIRE(level != nullptr);
    REQUIRE(level->quantity == 700);
    REQUIRE(level->orderCount == 1);
}


TEST_CASE(
    "Order delete removes remaining quantity from book"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(100, 42, 'B', 1000, 1000000)
        }
    );

    llt::itch::OrderExecutedMessage execution{};

    execution.orderReferenceNumber = 100;
    execution.executedShares = 300;

    state.onMessage(
        llt::itch::ItchMessage{execution}
    );

    REQUIRE(
        state.books()
            .find(42)
            ->bids()
            .find(1000000)
            ->quantity ==
        700
    );

    llt::itch::OrderDeleteMessage deletion{};

    deletion.orderReferenceNumber = 100;

    state.onMessage(
        llt::itch::ItchMessage{deletion}
    );

    REQUIRE(
        state.orders().find(100) ==
        nullptr
    );

    REQUIRE(
        state.books()
            .find(42)
            ->bids()
            .find(1000000) ==
        nullptr
    );
}


TEST_CASE(
    "Deleting one order preserves other quantity at same price"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(100, 42, 'B', 500, 1000000)
        }
    );

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(200, 42, 'B', 300, 1000000)
        }
    );

    llt::itch::OrderDeleteMessage deletion{};

    deletion.orderReferenceNumber = 100;

    state.onMessage(
        llt::itch::ItchMessage{deletion}
    );

    const auto* level =
        state.books()
            .find(42)
            ->bids()
            .find(1000000);

    REQUIRE(level != nullptr);
    REQUIRE(level->quantity == 300);
    REQUIRE(level->orderCount == 1);

    REQUIRE(
        state.orders().find(200) !=
        nullptr
    );
}


TEST_CASE(
    "Order replace moves quantity between price levels"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(100, 42, 'B', 500, 1000000)
        }
    );

    llt::itch::OrderReplaceMessage replacement{};

    replacement.timestamp = 2000;

    replacement.originalOrderReferenceNumber =
        100;

    replacement.newOrderReferenceNumber =
        200;

    replacement.shares = 700;
    replacement.price = 1000200;

    state.onMessage(
        llt::itch::ItchMessage{
            replacement
        }
    );

    const auto* book =
        state.books().find(42);

    REQUIRE(book != nullptr);

    REQUIRE(
        book->bids().find(1000000) ==
        nullptr
    );

    const auto* newLevel =
        book->bids().find(1000200);

    REQUIRE(newLevel != nullptr);
    REQUIRE(newLevel->quantity == 700);
    REQUIRE(newLevel->orderCount == 1);

    REQUIRE(
        state.orders().find(100) ==
        nullptr
    );

    const auto* order =
        state.orders().find(200);

    REQUIRE(order != nullptr);
    REQUIRE(order->price == 1000200);
    REQUIRE(order->quantity == 700);
}


TEST_CASE(
    "Order lifecycle updates BBO"
)
{
    llt::itch::ItchMarketState state;

    // Best bid.
    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(100, 42, 'B', 500, 1000000)
        }
    );

    // Second bid.
    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(200, 42, 'B', 300, 999900)
        }
    );

    // Best ask.
    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(300, 42, 'S', 400, 1000100)
        }
    );

    auto* book =
        state.books().find(42);

    REQUIRE(book != nullptr);

    auto bbo =
        book->bbo();

    REQUIRE(bbo.bidPrice == 1000000);
    REQUIRE(bbo.bidQuantity == 500);

    REQUIRE(bbo.askPrice == 1000100);
    REQUIRE(bbo.askQuantity == 400);

    // Remove the current best bid.
    llt::itch::OrderDeleteMessage deletion{};

    deletion.orderReferenceNumber = 100;

    state.onMessage(
        llt::itch::ItchMessage{
            deletion
        }
    );

    bbo = book->bbo();

    REQUIRE(bbo.hasBid);
    REQUIRE(bbo.bidPrice == 999900);
    REQUIRE(bbo.bidQuantity == 300);

    REQUIRE(bbo.hasAsk);
    REQUIRE(bbo.askPrice == 1000100);
    REQUIRE(bbo.askQuantity == 400);
}


TEST_CASE(
    "Complex order lifecycle keeps order store and book synchronized"
)
{
    llt::itch::ItchMarketState state;

    // 1000 @ 100.00
    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(100, 42, 'B', 1000, 1000000)
        }
    );

    // Execute 200 -> 800.
    llt::itch::OrderExecutedMessage execution{};

    execution.orderReferenceNumber = 100;
    execution.executedShares = 200;

    state.onMessage(
        llt::itch::ItchMessage{
            execution
        }
    );

    // Cancel 300 -> 500.
    llt::itch::OrderCancelMessage cancel{};

    cancel.orderReferenceNumber = 100;
    cancel.cancelledShares = 300;

    state.onMessage(
        llt::itch::ItchMessage{
            cancel
        }
    );

    auto* order =
        state.orders().find(100);

    REQUIRE(order != nullptr);
    REQUIRE(order->quantity == 500);

    auto* book =
        state.books().find(42);

    REQUIRE(book != nullptr);

    auto* level =
        book->bids().find(1000000);

    REQUIRE(level != nullptr);
    REQUIRE(level->quantity == 500);
    REQUIRE(level->orderCount == 1);

    // Replace remaining order.
    llt::itch::OrderReplaceMessage replacement{};

    replacement.timestamp = 2000;
    replacement.originalOrderReferenceNumber = 100;
    replacement.newOrderReferenceNumber = 200;
    replacement.shares = 600;
    replacement.price = 1000100;

    state.onMessage(
        llt::itch::ItchMessage{
            replacement
        }
    );

    REQUIRE(
        book->bids().find(1000000) ==
        nullptr
    );

    level =
        book->bids().find(1000100);

    REQUIRE(level != nullptr);
    REQUIRE(level->quantity == 600);
    REQUIRE(level->orderCount == 1);

    REQUIRE(
        state.orders().find(100) ==
        nullptr
    );

    order =
        state.orders().find(200);

    REQUIRE(order != nullptr);
    REQUIRE(order->quantity == 600);
    REQUIRE(order->price == 1000100);

    // Delete replacement.
    llt::itch::OrderDeleteMessage deletion{};

    deletion.orderReferenceNumber = 200;

    state.onMessage(
        llt::itch::ItchMessage{
            deletion
        }
    );

    REQUIRE(
        state.orders().find(200) ==
        nullptr
    );

    REQUIRE(
        book->bids().find(1000100) ==
        nullptr
    );

    REQUIRE(book->empty());
}