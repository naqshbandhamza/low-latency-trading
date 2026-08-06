#include <catch2/catch_test_macros.hpp>

#include "market_data/Level.h"

TEST_CASE("Level stores price and quantity")
{
    constexpr llt::Level level(
        llt::Price(234500),
        llt::Quantity(10)
    );

    STATIC_REQUIRE(level.price() == llt::Price(234500));

    STATIC_REQUIRE(level.quantity() == llt::Quantity(10));
}