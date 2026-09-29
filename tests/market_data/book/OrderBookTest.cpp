#include <catch2/catch_test_macros.hpp>

#include "market_data/book/OrderBook.h"

using namespace llt::market_data;


TEST_CASE(
    "OrderBook starts empty"
)
{
    OrderBook book;

    REQUIRE(book.empty());

    REQUIRE(book.bids().empty());
    REQUIRE(book.asks().empty());

    const auto bbo =
        book.bbo();

    REQUIRE_FALSE(bbo.hasBid);
    REQUIRE_FALSE(bbo.hasAsk);
}


TEST_CASE(
    "OrderBook stores bid"
)
{
    OrderBook book;

    book.bids().add(
        1000000,
        500
    );

    REQUIRE_FALSE(book.empty());

    const auto bbo =
        book.bbo();

    REQUIRE(bbo.hasBid);
    REQUIRE(bbo.bidPrice == 1000000);
    REQUIRE(bbo.bidQuantity == 500);

    REQUIRE_FALSE(bbo.hasAsk);
}


TEST_CASE(
    "OrderBook stores ask"
)
{
    OrderBook book;

    book.asks().add(
        1000100,
        300
    );

    const auto bbo =
        book.bbo();

    REQUIRE_FALSE(bbo.hasBid);

    REQUIRE(bbo.hasAsk);
    REQUIRE(bbo.askPrice == 1000100);
    REQUIRE(bbo.askQuantity == 300);
}


TEST_CASE(
    "OrderBook produces best bid and ask"
)
{
    OrderBook book;

    book.bids().add(
        999900,
        300
    );

    book.bids().add(
        1000000,
        500
    );

    book.asks().add(
        1000200,
        700
    );

    book.asks().add(
        1000100,
        400
    );

    const auto bbo =
        book.bbo();

    REQUIRE(bbo.hasBid);
    REQUIRE(bbo.bidPrice == 1000000);
    REQUIRE(bbo.bidQuantity == 500);

    REQUIRE(bbo.hasAsk);
    REQUIRE(bbo.askPrice == 1000100);
    REQUIRE(bbo.askQuantity == 400);
}


TEST_CASE(
    "OrderBook aggregates quantity at best bid"
)
{
    OrderBook book;

    book.bids().add(
        1000000,
        500
    );

    book.bids().add(
        1000000,
        300
    );

    const auto bbo =
        book.bbo();

    REQUIRE(bbo.hasBid);
    REQUIRE(bbo.bidPrice == 1000000);
    REQUIRE(bbo.bidQuantity == 800);
}


TEST_CASE(
    "OrderBook best bid changes when level disappears"
)
{
    OrderBook book;

    book.bids().add(
        1000000,
        500
    );

    book.bids().add(
        999900,
        300
    );

    REQUIRE(
        book.bbo().bidPrice ==
        1000000
    );

    REQUIRE(
        book.bids().removeOrder(
            1000000,
            500
        )
    );

    const auto bbo =
        book.bbo();

    REQUIRE(bbo.hasBid);
    REQUIRE(bbo.bidPrice == 999900);
    REQUIRE(bbo.bidQuantity == 300);
}


TEST_CASE(
    "OrderBook best ask changes when level disappears"
)
{
    OrderBook book;

    book.asks().add(
        1000100,
        500
    );

    book.asks().add(
        1000200,
        300
    );

    REQUIRE(
        book.bbo().askPrice ==
        1000100
    );

    REQUIRE(
        book.asks().removeOrder(
            1000100,
            500
        )
    );

    const auto bbo =
        book.bbo();

    REQUIRE(bbo.hasAsk);
    REQUIRE(bbo.askPrice == 1000200);
    REQUIRE(bbo.askQuantity == 300);
}


TEST_CASE(
    "OrderBook can have only bid side"
)
{
    OrderBook book;

    book.side(
        Side::Buy
    ).add(
        1000000,
        500
    );

    const auto bbo =
        book.bbo();

    REQUIRE(bbo.hasBid);
    REQUIRE_FALSE(bbo.hasAsk);
}


TEST_CASE(
    "OrderBook can have only ask side"
)
{
    OrderBook book;

    book.side(
        Side::Sell
    ).add(
        1000100,
        500
    );

    const auto bbo =
        book.bbo();

    REQUIRE_FALSE(bbo.hasBid);
    REQUIRE(bbo.hasAsk);
}


TEST_CASE(
    "OrderBook becomes empty after both sides removed"
)
{
    OrderBook book;

    book.bids().add(
        1000000,
        500
    );

    book.asks().add(
        1000100,
        300
    );

    REQUIRE_FALSE(book.empty());

    REQUIRE(
        book.bids().removeOrder(
            1000000,
            500
        )
    );

    REQUIRE_FALSE(book.empty());

    REQUIRE(
        book.asks().removeOrder(
            1000100,
            300
        )
    );

    REQUIRE(book.empty());

    const auto bbo =
        book.bbo();

    REQUIRE_FALSE(bbo.hasBid);
    REQUIRE_FALSE(bbo.hasAsk);
}