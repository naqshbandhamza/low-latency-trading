#include <catch2/catch_test_macros.hpp>

#include "types/Price.h"

TEST_CASE("Price stores its value")
{
    constexpr llt::Price price(12345);

    STATIC_REQUIRE(price.value() == 12345);
}