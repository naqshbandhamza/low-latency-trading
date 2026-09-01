#include <cstdint>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include "FeedHandler.h"
#include "logging/ILogger.h"
#include "market_data/MarketEvent.h"
#include "market_data/MockMarketDataSource.h"


namespace
{

class TestLogger : public llt::ILogger
{
public:

    void log(
        llt::LogLevel,
        std::string_view
    ) override
    {
    }
};

} // namespace


TEST_CASE(
    "FeedHandler publishes Quote and Trade market events"
)
{
    constexpr std::size_t eventCount = 100;

    TestLogger logger;

    llt::MarketEventQueue queue;

    llt::MockMarketDataSource source;

    llt::FeedHandler handler(
        logger,
        queue,
        source
    );

    handler.start(eventCount);

    REQUIRE(
        queue.size() == eventCount
    );

    for (
        std::uint64_t i = 0;
        i < eventCount;
        ++i
    )
    {
        auto event = queue.pop();

        REQUIRE(event.has_value());

        std::visit(
            [&](const auto& value)
            {
                REQUIRE(
                    value.sequence()
                    == llt::SequenceNumber(i)
                );
            },
            *event
        );
    }

    REQUIRE(queue.empty());
}


TEST_CASE(
    "FeedHandler converts Quote messages"
)
{
    TestLogger logger;

    llt::MarketEventQueue queue;

    llt::MockMarketDataSource source;

    llt::FeedHandler handler(
        logger,
        queue,
        source
    );

    handler.start(1);

    auto event = queue.pop();

    REQUIRE(event.has_value());

    REQUIRE(
        std::holds_alternative<llt::Quote>(*event)
    );

    const auto& quote =
        std::get<llt::Quote>(*event);

    REQUIRE(
        quote.instrument()
        == llt::Instrument("TXFU6")
    );

    REQUIRE(
        quote.sequence()
        == llt::SequenceNumber(0)
    );

    REQUIRE(
        quote.bid().price()
        == llt::Price(234500)
    );

    REQUIRE(
        quote.bid().quantity()
        == llt::Quantity(10)
    );

    REQUIRE(
        quote.ask().price()
        == llt::Price(234510)
    );

    REQUIRE(
        quote.ask().quantity()
        == llt::Quantity(12)
    );
}


TEST_CASE(
    "FeedHandler converts Trade messages"
)
{
    TestLogger logger;

    llt::MarketEventQueue queue;

    llt::MockMarketDataSource source;

    llt::FeedHandler handler(
        logger,
        queue,
        source
    );

    // First event = Quote
    // Second event = Trade
    handler.start(2);

    auto quoteEvent = queue.pop();
    auto tradeEvent = queue.pop();

    REQUIRE(quoteEvent.has_value());
    REQUIRE(tradeEvent.has_value());

    REQUIRE(
        std::holds_alternative<llt::Trade>(*tradeEvent)
    );

    const auto& trade =
        std::get<llt::Trade>(*tradeEvent);

    REQUIRE(
        trade.instrument()
        == llt::Instrument("TXFU6")
    );

    REQUIRE(
        trade.sequence()
        == llt::SequenceNumber(1)
    );

    REQUIRE(
        trade.price()
        == llt::Price(234505)
    );

    REQUIRE(
        trade.quantity()
        == llt::Quantity(3)
    );

    REQUIRE(
        trade.side()
        == llt::Side::Buy
    );

    REQUIRE(queue.empty());
}