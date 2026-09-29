#include <catch2/catch_test_macros.hpp>

#include <cstdint>

#include "market_data/itch/ItchMarketState.h"
#include "market_data/itch/ItchMessage.h"
#include "market_data/itch/messages/AddOrderMessage.h"
#include "market_data/itch/messages/AddOrderWithMpidMessage.h"

namespace
{

llt::itch::AddOrderMessage makeAdd(
    std::uint64_t orderId,
    std::uint16_t instrumentId,
    char side,
    std::uint32_t shares,
    std::uint32_t price
)
{
    llt::itch::AddOrderMessage message{};

    message.stockLocate = instrumentId;
    message.timestamp = 1000;
    message.orderReferenceNumber = orderId;
    message.buySellIndicator = side;
    message.shares = shares;
    message.price = price;

    return message;
}

} // namespace


TEST_CASE(
    "ITCH Add Order creates book price level"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(
                100,
                42,
                'B',
                500,
                1000000
            )
        }
    );

    REQUIRE(state.orders().size() == 1);
    REQUIRE(state.books().size() == 1);

    const auto* book =
        state.books().find(42);

    REQUIRE(book != nullptr);

    const auto* level =
        book->bids().find(1000000);

    REQUIRE(level != nullptr);
    REQUIRE(level->quantity == 500);
    REQUIRE(level->orderCount == 1);
}


TEST_CASE(
    "ITCH Add Order routes sell order to ask side"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(
                100,
                42,
                'S',
                300,
                1000100
            )
        }
    );

    const auto* book =
        state.books().find(42);

    REQUIRE(book != nullptr);

    REQUIRE(
        book->bids().find(1000100) ==
        nullptr
    );

    const auto* ask =
        book->asks().find(1000100);

    REQUIRE(ask != nullptr);
    REQUIRE(ask->quantity == 300);
    REQUIRE(ask->orderCount == 1);
}


TEST_CASE(
    "ITCH Add Orders aggregate at same price"
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

    const auto* book =
        state.books().find(42);

    REQUIRE(book != nullptr);

    const auto* level =
        book->bids().find(1000000);

    REQUIRE(level != nullptr);
    REQUIRE(level->quantity == 800);
    REQUIRE(level->orderCount == 2);

    REQUIRE(state.orders().size() == 2);
}


TEST_CASE(
    "ITCH Add Orders create correct BBO"
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
            makeAdd(200, 42, 'B', 300, 999900)
        }
    );

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(300, 42, 'S', 400, 1000100)
        }
    );

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(400, 42, 'S', 700, 1000200)
        }
    );

    const auto* book =
        state.books().find(42);

    REQUIRE(book != nullptr);

    const auto bbo =
        book->bbo();

    REQUIRE(bbo.hasBid);
    REQUIRE(bbo.bidPrice == 1000000);
    REQUIRE(bbo.bidQuantity == 500);

    REQUIRE(bbo.hasAsk);
    REQUIRE(bbo.askPrice == 1000100);
    REQUIRE(bbo.askQuantity == 400);
}


TEST_CASE(
    "ITCH Add Orders maintain independent instrument books"
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
            makeAdd(200, 77, 'B', 300, 2000000)
        }
    );

    REQUIRE(state.books().size() == 2);

    const auto* first =
        state.books().find(42);

    const auto* second =
        state.books().find(77);

    REQUIRE(first != nullptr);
    REQUIRE(second != nullptr);

    REQUIRE(
        first->bbo().bidPrice ==
        1000000
    );

    REQUIRE(
        first->bbo().bidQuantity ==
        500
    );

    REQUIRE(
        second->bbo().bidPrice ==
        2000000
    );

    REQUIRE(
        second->bbo().bidQuantity ==
        300
    );
}


TEST_CASE(
    "Duplicate ITCH Add Order does not duplicate book quantity"
)
{
    llt::itch::ItchMarketState state;

    const auto add =
        makeAdd(
            100,
            42,
            'B',
            500,
            1000000
        );

    state.onMessage(
        llt::itch::ItchMessage{add}
    );

    state.onMessage(
        llt::itch::ItchMessage{add}
    );

    REQUIRE(state.orders().size() == 1);
    REQUIRE(state.books().size() == 1);

    const auto* level =
        state.books()
            .find(42)
            ->bids()
            .find(1000000);

    REQUIRE(level != nullptr);

    // Must remain 500, not become 1000.
    REQUIRE(level->quantity == 500);
    REQUIRE(level->orderCount == 1);

    REQUIRE(
        state.duplicateAddOrders() == 1
    );
}


TEST_CASE(
    "ITCH Add Order with MPID updates order book"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::AddOrderWithMpidMessage message{};

    message.stockLocate = 42;
    message.timestamp = 1000;
    message.orderReferenceNumber = 500;
    message.buySellIndicator = 'S';
    message.shares = 250;
    message.price = 1000100;

    state.onMessage(
        llt::itch::ItchMessage{message}
    );

    REQUIRE(
        state.orders().find(500) !=
        nullptr
    );

    const auto* book =
        state.books().find(42);

    REQUIRE(book != nullptr);

    const auto* level =
        book->asks().find(1000100);

    REQUIRE(level != nullptr);
    REQUIRE(level->quantity == 250);
    REQUIRE(level->orderCount == 1);

    const auto bbo =
        book->bbo();

    REQUIRE(bbo.hasAsk);
    REQUIRE(bbo.askPrice == 1000100);
    REQUIRE(bbo.askQuantity == 250);
}