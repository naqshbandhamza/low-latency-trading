#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <variant>

#include "FeedHandlerState.h"

#include "market_data/MarketEvent.h"
#include "market_data/MarketEventQueue.h"

#include "market_data/itch/FailFastItchRecoverySource.h"
#include "ItchFeedHandler.h"
#include "market_data/itch/ItchMarketState.h"
#include "market_data/itch/ItchQuotePublisher.h"
#include "market_data/itch/ItchTradePublisher.h"
#include "market_data/itch/ItchSequenceRecovery.h"
#include "market_data/itch/UdpItchMarketDataSource.h"

namespace
{

    std::atomic<bool> running{
        true};

    //
    // Signal handler.
    //
    // Keep this extremely small.
    // Do not use std::cout here.
    //
    void handleSignal(
        int) noexcept
    {
        running.store(
            false,
            std::memory_order_relaxed);
    }

    const char *feedStateToString(
        llt::FeedHandlerState state) noexcept
    {
        switch (state)
        {
        case llt::FeedHandlerState::Stopped:
            return "STOPPED";

        case llt::FeedHandlerState::Running:
            return "RUNNING";

        case llt::FeedHandlerState::Failed:
            return "FAILED";
        }

        return "UNKNOWN";
    }

    void printUsage(
        const char *executable)
    {
        std::cerr
            << "Usage: "
            << executable
            << " <udp-port>\n"
            << '\n'
            << "Example:\n"
            << "  "
            << executable
            << " 19000\n";
    }

} // namespace

int main(
    int argc,
    char *argv[])
{
    //
    // =====================================================
    // CLI
    // =====================================================
    //

    if (argc != 2)
    {
        printUsage(
            argv[0]);

        return 2;
    }

    const auto parsedPort =
        std::strtoul(
            argv[1],
            nullptr,
            10);

    if (
        parsedPort == 0 ||
        parsedPort > 65535)
    {
        std::cerr
            << "Invalid UDP port: "
            << argv[1]
            << '\n';

        return 2;
    }

    const auto port =
        static_cast<std::uint16_t>(
            parsedPort);

    //
    // =====================================================
    // Signal handling
    // =====================================================
    //

    std::signal(
        SIGINT,
        handleSignal);

    std::signal(
        SIGTERM,
        handleSignal);

    //
    // =====================================================
    // Pipeline components
    // =====================================================
    //

    constexpr std::uint32_t
        receiveTimeoutMs = 100;

    //
    // Live UDP transport.
    //
    llt::itch::UdpItchMarketDataSource
        source{
            port,
            receiveTimeoutMs};

    //
    // Until a real exchange/provider recovery
    // channel exists, fail on packet loss.
    //
    llt::itch::FailFastItchRecoverySource
        recoverySource;

    llt::itch::ItchSequenceRecovery
        recovery{
            recoverySource};

    //
    // Reconstructed ITCH market state.
    //
    llt::itch::ItchMarketState
        marketState;

    //
    // Producer -> consumer SPSC.
    //
    llt::MarketEventQueue
        marketEventQueue;

    //
    // Converts BBO changes into the generic
    // downstream Quote representation.
    //
    llt::itch::ItchQuotePublisher
        publisher{
            marketState.instruments(),
            marketEventQueue};

    //
    // Converts ITCH executions/trade reports into
    // the generic downstream Trade representation.
    //
    llt::itch::ItchTradePublisher
        tradePublisher{
            marketState.instruments(),
            marketEventQueue};

    //
    // Market state -> publisher boundary.
    //
    marketState.setBboChangeHandler(
        [&publisher](
            llt::market_data::InstrumentId instrumentId,
            llt::market_data::Timestamp timestamp,
            const llt::market_data::Bbo &bbo)
        {
            publisher.onBboChange(
                instrumentId,
                timestamp,
                bbo);
        });

    //
    // Market state -> trade publisher boundary.
    //
    marketState.setTradeHandler(
        [&tradePublisher](
            llt::market_data::InstrumentId instrumentId,
            llt::market_data::Timestamp timestamp,
            llt::market_data::Price price,
            llt::market_data::Quantity quantity,
            llt::market_data::Side side)
        {
            tradePublisher.onOrderExecution(
                instrumentId,
                timestamp,
                price,
                quantity,
                side);
        });

    //
    // Live transport / sequencing / recovery /
    // decoding / dispatch.
    //
    llt::itch::ItchFeedHandler
        feedHandler{
            source,
            recovery,
            marketState};

    //
    // =====================================================
    // Consumer counters
    // =====================================================
    //

    std::atomic<std::uint64_t>
        consumedEvents{0};

    std::atomic<std::uint64_t>
        consumedQuotes{0};

    std::atomic<std::uint64_t>
        consumedTrades{0};

    //
    // =====================================================
    // Consumer thread
    // =====================================================
    //
    // MarketEventQueue is SPSC:
    //
    // producer:
    //     ItchQuotePublisher
    //
    // consumer:
    //     this thread
    //
    // Do NOT add another consumer to this queue.
    //

    std::thread consumerThread(
        [&]()
        {
            while (
                running.load(
                    std::memory_order_relaxed))
            {
                auto event =
                    marketEventQueue.pop();

                if (!event.has_value())
                {
                    std::this_thread::yield();
                    continue;
                }

                consumedEvents.fetch_add(
                    1,
                    std::memory_order_relaxed);

                if (
                    std::holds_alternative<
                        llt::Quote>(*event))
                {
                    consumedQuotes.fetch_add(
                        1,
                        std::memory_order_relaxed);
                }
                else if (
                    std::holds_alternative<
                        llt::Trade>(*event))
                {
                    consumedTrades.fetch_add(
                        1,
                        std::memory_order_relaxed);
                }
            }

            //
            // Drain anything published immediately
            // before shutdown.
            //
            while (true)
            {
                auto event =
                    marketEventQueue.pop();

                if (!event.has_value())
                {
                    break;
                }

                consumedEvents.fetch_add(
                    1,
                    std::memory_order_relaxed);

                if (
                    std::holds_alternative<
                        llt::Quote>(*event))
                {
                    consumedQuotes.fetch_add(
                        1,
                        std::memory_order_relaxed);
                }
                else if (
                    std::holds_alternative<
                        llt::Trade>(*event))
                {
                    consumedTrades.fetch_add(
                        1,
                        std::memory_order_relaxed);
                }
            }
        });

    //
    // =====================================================
    // Startup
    // =====================================================
    //

    std::cout
        << "========================================\n"
        << "          LIVE ITCH PIPELINE\n"
        << "========================================\n"
        << "UDP port                 : "
        << port
        << '\n'
        << "Receive timeout          : "
        << receiveTimeoutMs
        << " ms\n"
        << "Recovery                 : "
        << "FAIL-FAST\n"
        << "MarketEventQueue capacity: "
        << 4096
        << '\n'
        << "----------------------------------------\n"
        << "Waiting for ITCH UDP packets...\n"
        << "Press Ctrl+C to stop.\n"
        << "========================================\n";

    //
    // =====================================================
    // Feed thread
    // =====================================================
    //
    // run() owns the continuous receive loop.
    //
    // stop() is called by the main thread during shutdown.
    // Because the UDP source has a receive timeout, run()
    // periodically regains control and can observe the stop
    // request.
    //

    std::thread feedThread(
        [&]()
        {
            feedHandler.run();

            //
            // If sequencing/recovery fails, terminate the
            // entire live application rather than continue
            // with an invalid market state.
            //
            if (
                feedHandler.state() ==
                llt::FeedHandlerState::Failed)
            {
                running.store(
                    false,
                    std::memory_order_relaxed);
            }
        });

    //
    // =====================================================
    // Runtime monitoring
    // =====================================================
    //
    // Keep this interval short so Ctrl+C is responsive.
    //

    while (
        running.load(
            std::memory_order_relaxed))
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds{50});
    }

    //
    // =====================================================
    // Shutdown
    // =====================================================
    //

    //
    // Tell ItchFeedHandler's own run loop to stop.
    //
    feedHandler.stop();

    //
    // The UDP receive timeout ensures that a currently
    // blocked receive returns shortly, after which run()
    // can observe its stop flag and exit.
    //
    if (feedThread.joinable())
    {
        feedThread.join();
    }

    //
    // The consumer observes the application-level running
    // flag, exits, and drains any events remaining in the
    // SPSC before terminating.
    //
    if (consumerThread.joinable())
    {
        consumerThread.join();
    }

    //
    // =====================================================
    // Final statistics
    // =====================================================
    //

    std::cout
        << '\n'
        << "========================================\n"
        << "        LIVE ITCH FINAL STATS\n"
        << "========================================\n"
        << "Feed state               : "
        << feedStateToString(
               feedHandler.state())
        << '\n'
        << '\n'
        << "Processed live packets   : "
        << feedHandler.processedPackets()
        << '\n'
        << "Recovered packets        : "
        << feedHandler.recoveredPackets()
        << '\n'
        << "Sequence gaps            : "
        << feedHandler.gapsDetected()
        << '\n'
        << "Ignored packets          : "
        << feedHandler.ignoredPackets()
        << '\n'
        << '\n'
        << "Instruments              : "
        << marketState.instruments().size()
        << '\n'
        << "Active orders            : "
        << marketState.orders().size()
        << '\n'
        << "Books                    : "
        << marketState.books().size()
        << '\n'
        << '\n'
        << "Published quotes         : "
        << publisher.publishedQuotes()
        << '\n'
        << "Dropped quotes           : "
        << publisher.droppedQuotes()
        << '\n'
        << "Unknown instruments      : "
        << publisher.unknownInstruments()
        << '\n'
        << '\n'
        << "Consumed events          : "
        << consumedEvents.load(
               std::memory_order_relaxed)
        << '\n'
        << "Consumed quotes          : "
        << consumedQuotes.load(
               std::memory_order_relaxed)
        << '\n'
        << "Consumed trades          : "
        << consumedTrades.load(
               std::memory_order_relaxed)
        << '\n'
        << "Events remaining in SPSC : "
        << marketEventQueue.size()
        << '\n'
        << "Published trades         : "
        << tradePublisher.publishedTrades()
        << '\n'
        << "Dropped trades           : "
        << tradePublisher.droppedTrades()
        << '\n'
        << "Trade unknown instruments: "
        << tradePublisher.unknownInstruments()
        << '\n'
        << "========================================\n";

    //
    // =====================================================
    // Exit status
    // =====================================================
    //

    if (
        feedHandler.state() ==
        llt::FeedHandlerState::Failed)
    {
        std::cerr
            << '\n'
            << "ITCH feed terminated because the "
            << "market-data stream could not be "
            << "safely reconstructed.\n";

        return 1;
    }

    return 0;
}