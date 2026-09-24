#include <atomic>
#include <csignal>
#include <cstdint>
#include <iostream>
#include <thread>
#include <type_traits>
#include <variant>

#include "FeedHandler.h"
#include "FeedHandlerState.h"

#include "logging/ConsoleLogger.h"

#include "market_data/MarketEvent.h"
#include "market_data/NoopMarketDataRecoverySource.h"
#include "market_data/SequenceRecovery.h"
#include "market_data/UdpMarketDataSource.h"

#include "ring_buffer/SpscRingBuffer.h"

namespace
{

std::atomic<bool> shutdownRequested{false};

void handleSignal(int)
{
    shutdownRequested.store(
        true,
        std::memory_order_relaxed
    );
}

void printEvent(
    const llt::MarketEvent& event
)
{
    std::visit(
        [](const auto& value)
        {
            using Event =
                std::decay_t<decltype(value)>;

            if constexpr (
                std::is_same_v<Event, llt::Quote>
            )
            {
                std::cout
                    << "QUOTE"
                    << " seq=" << value.sequence().value()
                    << " bid=" << value.bid().price().value()
                    << " bidQty=" << value.bid().quantity().value()
                    << " ask=" << value.ask().price().value()
                    << " askQty=" << value.ask().quantity().value()
                    << '\n';
            }
            else if constexpr (
                std::is_same_v<Event, llt::Trade>
            )
            {
                std::cout
                    << "TRADE"
                    << " seq=" << value.sequence().value()
                    << " price=" << value.price().value()
                    << " quantity=" << value.quantity().value()
                    << " side="
                    << (
                        value.side() == llt::Side::Buy
                            ? "BUY"
                            : "SELL"
                    )
                    << '\n';
            }
        },
        event
    );
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

    llt::MarketEventQueue marketEventQueue;

    llt::UdpMarketDataSource marketDataSource(
        marketDataPort
    );

    // Temporary production recovery source.
    //
    // Until a real exchange/provider recovery source is
    // implemented, any sequence gap will fail recovery safely.
    llt::NoopMarketDataRecoverySource recoverySource;

    llt::SequenceRecovery sequenceRecovery(
        logger,
        recoverySource
    );

    llt::FeedHandler feedHandler(
        logger,
        marketEventQueue,
        marketDataSource,
        sequenceRecovery
    );

    // ---------------------------------------------------------
    // Signal handling
    // ---------------------------------------------------------

    std::signal(
        SIGINT,
        handleSignal
    );

    std::signal(
        SIGTERM,
        handleSignal
    );

    std::cout
        << "Trading engine starting\n"
        << "Listening for UDP market data on port "
        << marketDataPort
        << '\n'
        << "Press Ctrl+C to stop\n";

    // ---------------------------------------------------------
    // Feed thread
    //
    // UDP
    //  ↓
    // UdpMarketDataSource
    //  ↓
    // FeedHandler
    //  ↓
    // SequenceRecovery
    //  ↓
    // MarketEventQueue
    // ---------------------------------------------------------

    std::thread feedThread(
        [&feedHandler]
        {
            feedHandler.run();
        }
    );

    // ---------------------------------------------------------
    // Temporary MarketEvent consumer
    //
    // This will eventually be replaced by the strategy layer.
    // ---------------------------------------------------------

    while (
        !shutdownRequested.load(
            std::memory_order_relaxed
        )
    )
    {
        // A fatal feed failure must stop the application.
        //
        // We don't want the rest of the trading engine running
        // after market-data sequence integrity has been lost.
        if (
            feedHandler.state()
            == llt::FeedHandlerState::Failed
        )
        {
            std::cerr
                << "\nFatal market data feed failure\n";

            break;
        }

        auto event =
            marketEventQueue.pop();

        if (!event.has_value())
        {
            std::this_thread::yield();
            continue;
        }

        printEvent(*event);
    }

    // ---------------------------------------------------------
    // Determine why we're shutting down.
    // ---------------------------------------------------------

    const bool feedFailed =
        feedHandler.state()
        == llt::FeedHandlerState::Failed;

    if (!feedFailed)
    {
        std::cout
            << "\nShutdown requested\n";
    }

    // ---------------------------------------------------------
    // Stop producer
    // ---------------------------------------------------------

    feedHandler.stop();

    if (feedThread.joinable())
    {
        feedThread.join();
    }

    // ---------------------------------------------------------
    // Drain anything that was successfully published before
    // FeedHandler stopped.
    // ---------------------------------------------------------

    while (true)
    {
        auto event =
            marketEventQueue.pop();

        if (!event.has_value())
        {
            break;
        }

        printEvent(*event);
    }

    // ---------------------------------------------------------
    // Exit status
    // ---------------------------------------------------------

    if (feedFailed)
    {
        std::cerr
            << "Trading engine stopped due to market data failure\n";

        return 1;
    }

    std::cout
        << "Trading engine stopped\n";

    return 0;
}