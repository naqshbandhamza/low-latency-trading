#include <catch2/catch_test_macros.hpp>

#include "market_data/Quote.h"

TEST_CASE("Quote stores all fields")
{
    llt::Quote quote(
        "TXFU6",
        llt::SequenceNumber(42),
        llt::Timestamp(1000000),
        llt::Level(
            llt::Price(234500),
            llt::Quantity(10)),
        llt::Level(
            llt::Price(234510),
            llt::Quantity(12))
    );

    REQUIRE(quote.instrument() == "TXFU6");

    REQUIRE(quote.sequence() == llt::SequenceNumber(42));

    REQUIRE(quote.bid().price() == llt::Price(234500));

    REQUIRE(quote.ask().price() == llt::Price(234510));
}