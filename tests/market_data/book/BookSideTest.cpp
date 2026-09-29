#include <catch2/catch_test_macros.hpp>

#include "market_data/book/BookSide.h"

using namespace llt::market_data;


TEST_CASE(
    "BookSide starts empty"
)
{
    BookSide book(Side::Buy);

    REQUIRE(book.empty());
    REQUIRE(book.levelCount() == 0);
    REQUIRE(book.best() == nullptr);
}


TEST_CASE(
    "BookSide creates price level"
)
{
    BookSide book(Side::Buy);

    book.add(1000000, 500);

    REQUIRE_FALSE(book.empty());
    REQUIRE(book.levelCount() == 1);

    const auto* level =
        book.find(1000000);

    REQUIRE(level != nullptr);
    REQUIRE(level->price == 1000000);
    REQUIRE(level->quantity == 500);
    REQUIRE(level->orderCount == 1);
}


TEST_CASE(
    "BookSide aggregates orders at same price"
)
{
    BookSide book(Side::Buy);

    book.add(1000000, 500);
    book.add(1000000, 300);

    REQUIRE(book.levelCount() == 1);

    const auto* level =
        book.find(1000000);

    REQUIRE(level != nullptr);
    REQUIRE(level->quantity == 800);
    REQUIRE(level->orderCount == 2);
}


TEST_CASE(
    "BookSide creates independent price levels"
)
{
    BookSide book(Side::Buy);

    book.add(1000000, 500);
    book.add(999900, 300);

    REQUIRE(book.levelCount() == 2);

    REQUIRE(
        book.find(1000000)->quantity ==
        500
    );

    REQUIRE(
        book.find(999900)->quantity ==
        300
    );
}


TEST_CASE(
    "Buy BookSide chooses highest price as best"
)
{
    BookSide book(Side::Buy);

    book.add(999800, 100);
    book.add(1000000, 500);
    book.add(999900, 300);

    const auto* best =
        book.best();

    REQUIRE(best != nullptr);
    REQUIRE(best->price == 1000000);
    REQUIRE(best->quantity == 500);
}


TEST_CASE(
    "Sell BookSide chooses lowest price as best"
)
{
    BookSide book(Side::Sell);

    book.add(1000200, 100);
    book.add(1000000, 500);
    book.add(1000100, 300);

    const auto* best =
        book.best();

    REQUIRE(best != nullptr);
    REQUIRE(best->price == 1000000);
    REQUIRE(best->quantity == 500);
}


TEST_CASE(
    "BookSide reduces price level quantity"
)
{
    BookSide book(Side::Buy);

    book.add(1000000, 500);

    REQUIRE(
        book.reduce(
            1000000,
            200
        )
    );

    const auto* level =
        book.find(1000000);

    REQUIRE(level != nullptr);
    REQUIRE(level->quantity == 300);
    REQUIRE(level->orderCount == 1);
}


TEST_CASE(
    "BookSide rejects reduction for unknown price"
)
{
    BookSide book(Side::Buy);

    book.add(1000000, 500);

    REQUIRE_FALSE(
        book.reduce(
            999900,
            100
        )
    );

    REQUIRE(
        book.find(1000000)->quantity ==
        500
    );
}


TEST_CASE(
    "BookSide removes empty price level"
)
{
    BookSide book(Side::Buy);

    book.add(1000000, 500);

    REQUIRE(
        book.removeOrder(
            1000000,
            500
        )
    );

    REQUIRE(book.find(1000000) == nullptr);
    REQUIRE(book.levelCount() == 0);
    REQUIRE(book.empty());
}


TEST_CASE(
    "BookSide keeps price level while other orders remain"
)
{
    BookSide book(Side::Buy);

    book.add(1000000, 500);
    book.add(1000000, 300);

    REQUIRE(
        book.removeOrder(
            1000000,
            500
        )
    );

    const auto* level =
        book.find(1000000);

    REQUIRE(level != nullptr);
    REQUIRE(level->quantity == 300);
    REQUIRE(level->orderCount == 1);
    REQUIRE(book.levelCount() == 1);
}


TEST_CASE(
    "Buy BookSide best price changes after best level removed"
)
{
    BookSide book(Side::Buy);

    book.add(1000000, 500);
    book.add(999900, 300);
    book.add(999800, 200);

    REQUIRE(
        book.best()->price ==
        1000000
    );

    REQUIRE(
        book.removeOrder(
            1000000,
            500
        )
    );

    REQUIRE(
        book.best()->price ==
        999900
    );
}


TEST_CASE(
    "Sell BookSide best price changes after best level removed"
)
{
    BookSide book(Side::Sell);

    book.add(1000000, 500);
    book.add(1000100, 300);
    book.add(1000200, 200);

    REQUIRE(
        book.best()->price ==
        1000000
    );

    REQUIRE(
        book.removeOrder(
            1000000,
            500
        )
    );

    REQUIRE(
        book.best()->price ==
        1000100
    );
}