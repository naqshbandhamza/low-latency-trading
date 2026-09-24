#include <catch2/catch_test_macros.hpp>

#include <thread>

#include "market_data/MarketEvent.h"
#include "strategy/OrderIntentQueue.h"
#include "strategy/SimpleStrategy.h"
#include "strategy/StrategyEngine.h"
#include "strategy/StrategyEngineState.h"

TEST_CASE(
    "StrategyEngine consumes market event and publishes order intent"
)
{
    llt::MarketEventQueue
        marketEventQueue;

    llt::OrderIntentQueue
        orderIntentQueue;

    llt::SimpleStrategy strategy;

    llt::StrategyEngine engine(
        marketEventQueue,
        orderIntentQueue,
        strategy
    );

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

    REQUIRE(
        marketEventQueue.push(
            std::move(event)
        )
    );

    std::thread strategyThread(
        [&engine]
        {
            engine.run();
        }
    );

    while (
        engine.state()
        != llt::StrategyEngineState::Running
    )
    {
        std::this_thread::yield();
    }

    std::optional<llt::OrderIntent>
        intent;

    while (!intent.has_value())
    {
        intent =
            orderIntentQueue.pop();

        std::this_thread::yield();
    }

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

    engine.stop();

    strategyThread.join();

    REQUIRE(
        engine.state()
        == llt::StrategyEngineState::Stopped
    );
}