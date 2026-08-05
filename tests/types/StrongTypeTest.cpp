#include <catch2/catch_test_macros.hpp>

#include "types/Price.h"

TEST_CASE("Price comparison")
{
    constexpr llt::Price bid(100);
    constexpr llt::Price ask(101);

    STATIC_REQUIRE(bid < ask);

    STATIC_REQUIRE(ask > bid);

    STATIC_REQUIRE(bid != ask);

    STATIC_REQUIRE(bid == llt::Price(100));
}