#include <catch2/catch_test_macros.hpp>

#include "market_data/Trade.h"

TEST_CASE("Trade stores all fields")
{
    llt::Trade trade(
        "TXFU6",
        llt::SequenceNumber(100),
        llt::Timestamp(123456789),
        llt::Price(234500),
        llt::Quantity(5),
        llt::Side::Buy
    );

    REQUIRE(trade.instrument() == "TXFU6");
    REQUIRE(trade.sequence() == llt::SequenceNumber(100));
    REQUIRE(trade.timestamp() == llt::Timestamp(123456789));
    REQUIRE(trade.price() == llt::Price(234500));
    REQUIRE(trade.quantity() == llt::Quantity(5));
    REQUIRE(trade.side() == llt::Side::Buy);
}