#include <catch2/catch_test_macros.hpp>

#include <variant>

#include <type_traits>

#include "market_data/MarketEvent.h"

TEST_CASE("MarketEvent can contain a Quote")
{
    llt::Quote quote(
        "TXFU6",
        llt::SequenceNumber(1),
        llt::Timestamp(1000),
        llt::Level(
            llt::Price(234500),
            llt::Quantity(10)
        ),
        llt::Level(
            llt::Price(234510),
            llt::Quantity(12)
        )
    );

    llt::MarketEvent event = quote;

    REQUIRE(std::holds_alternative<llt::Quote>(event));
}

TEST_CASE("MarketEvent can contain a Trade")
{
    llt::Trade trade(
        "TXFU6",
        llt::SequenceNumber(2),
        llt::Timestamp(2000),
        llt::Price(234505),
        llt::Quantity(3),
        llt::Side::Buy
    );

    llt::MarketEvent event = trade;

    REQUIRE(std::holds_alternative<llt::Trade>(event));
}


TEST_CASE("MarketEvent can be visited")
{
    llt::Trade trade(
        "TXFU6",
        llt::SequenceNumber(10),
        llt::Timestamp(5000),
        llt::Price(234500),
        llt::Quantity(2),
        llt::Side::Buy
    );

    llt::MarketEvent event = trade;

    bool visitedTrade = false;

    std::visit(
        [&](const auto& message)
        {
            using T = std::decay_t<decltype(message)>;

            if constexpr (std::is_same_v<T, llt::Trade>)
            {
                visitedTrade = true;

                REQUIRE(message.price() == llt::Price(234500));
            }
        },
        event
    );

    REQUIRE(visitedTrade);
}