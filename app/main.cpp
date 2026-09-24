#include <atomic>
#include <csignal>
#include <cstdint>
#include <iostream>
#include <thread>

#include "FeedHandler.h"
#include "FeedHandlerState.h"

#include "logging/ConsoleLogger.h"

#include "market_data/NoopMarketDataRecoverySource.h"
#include "market_data/SequenceRecovery.h"
#include "market_data/UdpMarketDataSource.h"

#include "ring_buffer/SpscRingBuffer.h"

#include "strategy/OrderIntentQueue.h"
#include "strategy/SimpleStrategy.h"
#include "strategy/StrategyEngine.h"
#include "strategy/StrategyEngineState.h"

namespace
{

std::atomic<bool> shutdownRequested{
    false
};


void handleSignal(int)
{
    shutdownRequested.store(
        true,
        std::memory_order_relaxed
    );
}


void printOrderIntent(
    const llt::OrderIntent& intent
)
{
    std::cout
        << "ORDER INTENT"
        << " instrument="
        << intent.instrument.view()
        << " side="
        << (
            intent.side == llt::Side::Buy
                ? "BUY"
                : "SELL"
        )
        << " price="
        << intent.price.value()
        << " quantity="
        << intent.quantity.value()
        << '\n';
}

} // namespace


int main()
{
    constexpr std::uint16_t marketDataPort =
        19000;

    // ---------------------------------------------------------
    // Infrastructure
    // ---------------------------------------------------------

    llt::ConsoleLogger logger;

    llt::MarketEventQueue
        marketEventQueue;

    llt::OrderIntentQueue
        orderIntentQueue;


    // ---------------------------------------------------------
    // Market data
    // ---------------------------------------------------------

    llt::UdpMarketDataSource
        marketDataSource(
            marketDataPort
        );

    llt::NoopMarketDataRecoverySource
        recoverySource;

    llt::SequenceRecovery
        sequenceRecovery(
            logger,
            recoverySource
        );

    llt::FeedHandler
        feedHandler(
            logger,
            marketEventQueue,
            marketDataSource,
            sequenceRecovery
        );


    // ---------------------------------------------------------
    // Strategy
    // ---------------------------------------------------------

    llt::SimpleStrategy
        strategy;

    llt::StrategyEngine
        strategyEngine(
            marketEventQueue,
            orderIntentQueue,
            strategy
        );


    // ---------------------------------------------------------
    // Signals
    // ---------------------------------------------------------

    std::signal(
        SIGINT,
        handleSignal
    );

    std::signal(
        SIGTERM,
        handleSignal
    );


    // ---------------------------------------------------------
    // Startup
    // ---------------------------------------------------------

    std::cout
        << "Trading engine starting\n"
        << "Listening for UDP market data on port "
        << marketDataPort
        << '\n'
        << "Press Ctrl+C to stop\n";


    // ---------------------------------------------------------
    // Start Strategy first.
    //
    // This ensures the MarketEventQueue already has an active
    // consumer before FeedHandler begins publishing.
    // ---------------------------------------------------------

    std::thread strategyThread(
        [&strategyEngine]
        {
            strategyEngine.run();
        }
    );


    // ---------------------------------------------------------
    // Start market-data feed.
    // ---------------------------------------------------------

    std::thread feedThread(
        [&feedHandler]
        {
            feedHandler.run();
        }
    );


    // ---------------------------------------------------------
    // Main supervision / OrderIntent consumer
    // ---------------------------------------------------------

    bool fatalFailure =
        false;

    while (
        !shutdownRequested.load(
            std::memory_order_relaxed
        )
    )
    {
        // -----------------------------------------------------
        // Feed failure is fatal.
        // -----------------------------------------------------

        if (
            feedHandler.state()
            == llt::FeedHandlerState::Failed
        )
        {
            std::cerr
                << "\nFatal market data feed failure\n";

            fatalFailure =
                true;

            break;
        }


        // -----------------------------------------------------
        // Strategy failure is fatal.
        // -----------------------------------------------------

        if (
            strategyEngine.state()
            == llt::StrategyEngineState::Failed
        )
        {
            std::cerr
                << "\nFatal strategy engine failure\n";

            fatalFailure =
                true;

            break;
        }


        // -----------------------------------------------------
        // Consume generated OrderIntents.
        // -----------------------------------------------------

        auto intent =
            orderIntentQueue.pop();

        if (!intent.has_value())
        {
            std::this_thread::yield();
            continue;
        }

        printOrderIntent(
            *intent
        );
    }


    // ---------------------------------------------------------
    // Shutdown
    //
    // Stop the upstream producer first.
    // ---------------------------------------------------------

    feedHandler.stop();

    if (feedThread.joinable())
    {
        feedThread.join();
    }


    // ---------------------------------------------------------
    // Then stop Strategy.
    //
    // FeedHandler can no longer produce new MarketEvents.
    // ---------------------------------------------------------

    strategyEngine.stop();

    if (strategyThread.joinable())
    {
        strategyThread.join();
    }


    // ---------------------------------------------------------
    // Drain any OrderIntents that were already produced before
    // shutdown.
    // ---------------------------------------------------------

    while (true)
    {
        auto intent =
            orderIntentQueue.pop();

        if (!intent.has_value())
        {
            break;
        }

        printOrderIntent(
            *intent
        );
    }


    // ---------------------------------------------------------
    // Exit status
    // ---------------------------------------------------------

    if (fatalFailure)
    {
        std::cerr
            << "Trading engine stopped due to fatal failure\n";

        return 1;
    }

    std::cout
        << "\nShutdown requested\n"
        << "Trading engine stopped\n";

    return 0;
}