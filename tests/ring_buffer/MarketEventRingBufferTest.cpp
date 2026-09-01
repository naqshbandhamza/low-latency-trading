#include <catch2/catch_test_macros.hpp>

#include "market_data/MarketEvent.h"
#include "ring_buffer/SpscRingBuffer.h"

TEST_CASE("SPSC ring buffer transports MarketEvent")
{
    llt::SpscRingBuffer<
        llt::MarketEvent,
        8
    > queue;

    llt::Quote quote(
        llt::Instrument("TXFU6"),
        llt::SequenceNumber(100),
        llt::Timestamp(123456),
        llt::Level(
            llt::Price(234500),
            llt::Quantity(10)
        ),
        llt::Level(
            llt::Price(234510),
            llt::Quantity(12)
        )
    );

    REQUIRE(queue.push(
        llt::MarketEvent{std::move(quote)}
    ));

    auto event = queue.pop();

    REQUIRE(event.has_value());

    REQUIRE(
        std::holds_alternative<llt::Quote>(*event)
    );

    const auto& receivedQuote =
        std::get<llt::Quote>(*event);

    REQUIRE(
        receivedQuote.instrument() == llt::Instrument("TXFU6")
    );

    REQUIRE(
        receivedQuote.sequence()
        == llt::SequenceNumber(100)
    );

    REQUIRE(
        receivedQuote.bid().price()
        == llt::Price(234500)
    );
}


TEST_CASE("SPSC ring buffer transports Trade")
{
    llt::SpscRingBuffer<
        llt::MarketEvent,
        8
    > queue;

    llt::Trade trade(
        llt::Instrument("TXFU6"),
        llt::SequenceNumber(200),
        llt::Timestamp(200000),
        llt::Price(234505),
        llt::Quantity(3),
        llt::Side::Buy
    );

    REQUIRE(queue.push(
        llt::MarketEvent{std::move(trade)}
    ));

    auto event = queue.pop();

    REQUIRE(event.has_value());

    REQUIRE(
        std::holds_alternative<llt::Trade>(*event)
    );

    const auto& receivedTrade =
        std::get<llt::Trade>(*event);

    REQUIRE(
        receivedTrade.instrument() == llt::Instrument("TXFU6")
    );

    REQUIRE(
        receivedTrade.price()
        == llt::Price(234505)
    );

    REQUIRE(
        receivedTrade.quantity()
        == llt::Quantity(3)
    );

    REQUIRE(
        receivedTrade.side()
        == llt::Side::Buy
    );
}