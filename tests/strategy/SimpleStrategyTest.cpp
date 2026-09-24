#include <catch2/catch_test_macros.hpp>

#include "market_data/MarketEvent.h"
#include "strategy/OrderIntentQueue.h"
#include "strategy/SimpleStrategy.h"


TEST_CASE(
    "SimpleStrategy creates buy intent for tight quote"
)
{
    llt::SimpleStrategy strategy;

    llt::MarketEvent event =
        llt::Quote(
            llt::Instrument("TXFU6"),

            llt::SequenceNumber(1),

            llt::Timestamp(100),

            llt::Level(
                llt::Price(10000),
                llt::Quantity(10)
            ),

            llt::Level(
                llt::Price(10010),
                llt::Quantity(12)
            )
        );

    auto intent =
        strategy.onMarketEvent(
            event
        );

    REQUIRE(
        intent.has_value()
    );

    REQUIRE(
        intent->instrument
        == llt::Instrument("TXFU6")
    );

    REQUIRE(
        intent->side
        == llt::Side::Buy
    );

    REQUIRE(
        intent->price.value()
        == 10010
    );

    REQUIRE(
        intent->quantity.value()
        == 1
    );
}

TEST_CASE(
    "SimpleStrategy ignores wide quote"
)
{
    llt::SimpleStrategy strategy;

    llt::MarketEvent event =
        llt::Quote(
            llt::Instrument("TXFU6"),
            llt::SequenceNumber(1),
            llt::Timestamp(100),

            llt::Level(
                llt::Price(10000),
                llt::Quantity(10)
            ),

            llt::Level(
                llt::Price(10020),
                llt::Quantity(12)
            )
        );

    const auto intent =
        strategy.onMarketEvent(
            event
        );

    REQUIRE_FALSE(
        intent.has_value()
    );
}


TEST_CASE(
    "SimpleStrategy ignores trade event"
)
{
    llt::SimpleStrategy strategy;

    llt::MarketEvent event =
        llt::Trade(
            llt::Instrument("TXFU6"),
            llt::SequenceNumber(1),
            llt::Timestamp(100),
            llt::Price(10005),
            llt::Quantity(5),
            llt::Side::Buy
        );

    const auto intent =
        strategy.onMarketEvent(
            event
        );

    REQUIRE_FALSE(
        intent.has_value()
    );
}