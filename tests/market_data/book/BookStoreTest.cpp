#include <catch2/catch_test_macros.hpp>

#include "market_data/book/BookStore.h"

using namespace llt::market_data;


TEST_CASE(
    "BookStore starts empty"
)
{
    BookStore store;

    REQUIRE(store.size() == 0);
    REQUIRE_FALSE(store.contains(42));
    REQUIRE(store.find(42) == nullptr);
}


TEST_CASE(
    "BookStore creates order book"
)
{
    BookStore store;

    auto& book =
        store.getOrCreate(42);

    REQUIRE(store.size() == 1);
    REQUIRE(store.contains(42));

    REQUIRE(
        store.find(42) ==
        &book
    );

    REQUIRE(book.empty());
}


TEST_CASE(
    "BookStore returns existing order book"
)
{
    BookStore store;

    auto& first =
        store.getOrCreate(42);

    first.bids().add(
        1000000,
        500
    );

    auto& second =
        store.getOrCreate(42);

    REQUIRE(store.size() == 1);

    REQUIRE(
        &first ==
        &second
    );

    const auto bbo =
        second.bbo();

    REQUIRE(bbo.hasBid);
    REQUIRE(bbo.bidPrice == 1000000);
    REQUIRE(bbo.bidQuantity == 500);
}


TEST_CASE(
    "BookStore keeps instruments independent"
)
{
    BookStore store;

    auto& first =
        store.getOrCreate(42);

    auto& second =
        store.getOrCreate(77);

    first.bids().add(
        1000000,
        500
    );

    second.asks().add(
        2000000,
        300
    );

    REQUIRE(store.size() == 2);

    const auto firstBbo =
        store.find(42)->bbo();

    REQUIRE(firstBbo.hasBid);
    REQUIRE(firstBbo.bidPrice == 1000000);
    REQUIRE(firstBbo.bidQuantity == 500);
    REQUIRE_FALSE(firstBbo.hasAsk);

    const auto secondBbo =
        store.find(77)->bbo();

    REQUIRE_FALSE(secondBbo.hasBid);
    REQUIRE(secondBbo.hasAsk);
    REQUIRE(secondBbo.askPrice == 2000000);
    REQUIRE(secondBbo.askQuantity == 300);
}


TEST_CASE(
    "BookStore find does not create missing book"
)
{
    BookStore store;

    REQUIRE(
        store.find(999) ==
        nullptr
    );

    REQUIRE(store.size() == 0);
}


TEST_CASE(
    "BookStore aggregates orders independently per instrument"
)
{
    BookStore store;

    auto& first =
        store.getOrCreate(42);

    auto& second =
        store.getOrCreate(77);

    first.bids().add(
        1000000,
        500
    );

    first.bids().add(
        1000000,
        300
    );

    second.bids().add(
        1000000,
        200
    );

    const auto firstBbo =
        first.bbo();

    const auto secondBbo =
        second.bbo();

    REQUIRE(firstBbo.bidQuantity == 800);
    REQUIRE(secondBbo.bidQuantity == 200);
}


TEST_CASE(
    "BookStore maintains independent bid and ask BBO"
)
{
    BookStore store;

    auto& first =
        store.getOrCreate(42);

    first.bids().add(
        1000000,
        500
    );

    first.bids().add(
        999900,
        700
    );

    first.asks().add(
        1000200,
        300
    );

    first.asks().add(
        1000100,
        400
    );

    auto& second =
        store.getOrCreate(77);

    second.bids().add(
        2000000,
        100
    );

    second.asks().add(
        2000500,
        200
    );

    const auto firstBbo =
        first.bbo();

    REQUIRE(firstBbo.bidPrice == 1000000);
    REQUIRE(firstBbo.bidQuantity == 500);
    REQUIRE(firstBbo.askPrice == 1000100);
    REQUIRE(firstBbo.askQuantity == 400);

    const auto secondBbo =
        second.bbo();

    REQUIRE(secondBbo.bidPrice == 2000000);
    REQUIRE(secondBbo.bidQuantity == 100);
    REQUIRE(secondBbo.askPrice == 2000500);
    REQUIRE(secondBbo.askQuantity == 200);
}