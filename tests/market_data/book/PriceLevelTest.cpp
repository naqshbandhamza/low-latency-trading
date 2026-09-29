#include <catch2/catch_test_macros.hpp>

#include "market_data/book/PriceLevel.h"

using namespace llt::market_data;


TEST_CASE(
    "PriceLevel starts empty"
)
{
    PriceLevel level{
        .price = 1000000
    };

    REQUIRE(level.price == 1000000);
    REQUIRE(level.quantity == 0);
    REQUIRE(level.orderCount == 0);
    REQUIRE(level.empty());
}


TEST_CASE(
    "PriceLevel adds order quantity"
)
{
    PriceLevel level{
        .price = 1000000
    };

    level.add(500);

    REQUIRE(level.quantity == 500);
    REQUIRE(level.orderCount == 1);
    REQUIRE_FALSE(level.empty());
}


TEST_CASE(
    "PriceLevel aggregates multiple orders"
)
{
    PriceLevel level{
        .price = 1000000
    };

    level.add(500);
    level.add(300);
    level.add(200);

    REQUIRE(level.quantity == 1000);
    REQUIRE(level.orderCount == 3);
}


TEST_CASE(
    "PriceLevel reduces quantity without removing order"
)
{
    PriceLevel level{
        .price = 1000000
    };

    level.add(500);

    REQUIRE(level.reduce(200));

    REQUIRE(level.quantity == 300);
    REQUIRE(level.orderCount == 1);
}


TEST_CASE(
    "PriceLevel rejects reduction larger than quantity"
)
{
    PriceLevel level{
        .price = 1000000
    };

    level.add(500);

    REQUIRE_FALSE(
        level.reduce(501)
    );

    REQUIRE(level.quantity == 500);
    REQUIRE(level.orderCount == 1);
}


TEST_CASE(
    "PriceLevel removes remaining order quantity"
)
{
    PriceLevel level{
        .price = 1000000
    };

    level.add(500);
    level.add(300);

    REQUIRE(
        level.removeOrder(500)
    );

    REQUIRE(level.quantity == 300);
    REQUIRE(level.orderCount == 1);
    REQUIRE_FALSE(level.empty());
}


TEST_CASE(
    "PriceLevel becomes empty after last order removed"
)
{
    PriceLevel level{
        .price = 1000000
    };

    level.add(500);

    REQUIRE(
        level.removeOrder(500)
    );

    REQUIRE(level.quantity == 0);
    REQUIRE(level.orderCount == 0);
    REQUIRE(level.empty());
}


TEST_CASE(
    "PriceLevel rejects removing order when empty"
)
{
    PriceLevel level{
        .price = 1000000
    };

    REQUIRE_FALSE(
        level.removeOrder(100)
    );

    REQUIRE(level.quantity == 0);
    REQUIRE(level.orderCount == 0);
}


TEST_CASE(
    "PriceLevel handles partial execution then order removal"
)
{
    PriceLevel level{
        .price = 1000000
    };

    level.add(1000);

    // E: 300 shares execute.
    REQUIRE(
        level.reduce(300)
    );

    REQUIRE(level.quantity == 700);
    REQUIRE(level.orderCount == 1);

    // D: remaining 700 shares disappear.
    REQUIRE(
        level.removeOrder(700)
    );

    REQUIRE(level.quantity == 0);
    REQUIRE(level.orderCount == 0);
    REQUIRE(level.empty());
}