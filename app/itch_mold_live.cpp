#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <variant>

#include "FeedHandlerState.h"
#include "MoldItchFeedHandler.h"

#include "market_data/MarketEvent.h"
#include "market_data/MarketEventQueue.h"

#include "market_data/itch/ItchMarketState.h"
#include "market_data/itch/ItchQuotePublisher.h"
#include "market_data/itch/ItchTradePublisher.h"

#include "market_data/moldudp64/FileMoldItchRecoverySource.h"
#include "market_data/moldudp64/MoldItchSequenceRecovery.h"
#include "market_data/moldudp64/UdpMoldMarketDataSource.h"

#include "market_data/moldudp64/MoldDatagramQueue.h"
#include "market_data/moldudp64/MoldUdpReceiver.h"

namespace
{

    std::atomic<bool> running{
        true};

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
            << "Usage:\n"
            << "  "
            << executable
            << " <udp-port> <ITCH-recovery-file>\n"
            << '\n'
            << "Example:\n"
            << "  "
            << executable
            << " 19000"
            << " data/itch/01302019.NASDAQ_ITCH50\n";
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

    std::cerr
        << "[startup] itch_mold_live starting..."
        << std::endl;

    if (argc != 3)
    {
        printUsage(
            argv[0]);

        return 2;
    }

    std::cerr
        << "[startup] Parsing command-line arguments..."
        << std::endl;

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

    const std::string recoveryFilePath =
        argv[2];

    std::cerr
        << "[startup] UDP port: "
        << port
        << std::endl;

    std::cerr
        << "[startup] Recovery file: "
        << recoveryFilePath
        << std::endl;

    //
    // =====================================================
    // Signal handling
    // =====================================================
    //

    std::cerr
        << "[startup] Installing signal handlers..."
        << std::endl;

    std::signal(
        SIGINT,
        handleSignal);

    std::signal(
        SIGTERM,
        handleSignal);

    std::cerr
        << "[startup] Signal handlers installed."
        << std::endl;

    //
    // =====================================================
    // Pipeline components
    // =====================================================
    //

    constexpr std::uint32_t
        receiveTimeoutMs = 100;

    //
    // -----------------------------------------------------
    // UDP source
    // -----------------------------------------------------
    //

    std::cerr
        << "[startup] Creating MoldUDP64 UDP source..."
        << std::endl;

    llt::moldudp64::UdpMoldMarketDataSource
        source{
            port,
            receiveTimeoutMs};

    std::cerr
        << "[startup] MoldUDP64 UDP source ready."
        << std::endl;

    //
    // -----------------------------------------------------
    // Recovery source
    // -----------------------------------------------------
    //
    // IMPORTANT:
    //
    // If startup appears to pause after the next message,
    // FileMoldItchRecoverySource is spending time building
    // or loading its recovery index.
    //
    // Sequence numbers correspond to ITCH message
    // positions in the historical BinaryFILE:
    //
    //      1, 2, 3, ...
    //

    std::cerr
        << "[startup] Initializing Mold ITCH recovery source..."
        << std::endl;

    std::cerr
        << "[startup] This may take time if the recovery "
        << "index must be built from the BinaryFILE."
        << std::endl;

    const auto recoveryInitStart =
        std::chrono::steady_clock::now();

    llt::moldudp64::FileMoldItchRecoverySource
        recoverySource{
            recoveryFilePath};

    const auto recoveryInitEnd =
        std::chrono::steady_clock::now();

    const auto recoveryInitMs =
        std::chrono::duration_cast<
            std::chrono::milliseconds>(
            recoveryInitEnd -
            recoveryInitStart)
            .count();

    std::cerr
        << "[startup] Mold ITCH recovery source constructor "
        << "completed in "
        << recoveryInitMs
        << " ms."
        << std::endl;

    std::cerr
        << "[startup] Checking recovery index..."
        << std::endl;

    if (!recoverySource.indexReady())
    {
        std::cerr
            << "[startup] ERROR: Recovery index is not ready."
            << std::endl;

        std::cerr
            << "Failed to initialize Mold ITCH "
            << "recovery index for:\n"
            << "  "
            << recoveryFilePath
            << '\n';

        return 1;
    }

    std::cerr
        << "[startup] Recovery index ready."
        << std::endl;

    std::cerr
        << "[startup] Recovery index source: "
        << (recoverySource.indexLoadedFromDisk()
                ? "DISK"
                : "BUILT")
        << std::endl;

    std::cerr
        << "[startup] Recovery index path: "
        << recoverySource.indexPath()
        << std::endl;

    std::cerr
        << "[startup] Indexed messages: "
        << recoverySource.indexedMessages()
        << std::endl;

    std::cerr
        << "[startup] Recovery checkpoints: "
        << recoverySource.checkpointCount()
        << std::endl;

    //
    // -----------------------------------------------------
    // Sequence recovery coordinator
    // -----------------------------------------------------
    //

    std::cerr
        << "[startup] Creating Mold ITCH sequence recovery..."
        << std::endl;

    llt::moldudp64::MoldItchSequenceRecovery
        recovery{
            recoverySource};

    std::cerr
        << "[startup] Mold ITCH sequence recovery ready."
        << std::endl;

    //
    // -----------------------------------------------------
    // Market state
    // -----------------------------------------------------
    //

    std::cerr
        << "[startup] Creating ITCH market state..."
        << std::endl;

    llt::itch::ItchMarketState
        marketState;

    std::cerr
        << "[startup] ITCH market state ready."
        << std::endl;

    //
    // -----------------------------------------------------
    // MarketEvent SPSC
    // -----------------------------------------------------
    //

    std::cerr
        << "[startup] Creating MarketEventQueue..."
        << std::endl;

    llt::MarketEventQueue
        marketEventQueue;

    std::cerr
        << "[startup] MarketEventQueue ready."
        << std::endl;

    //
    // -----------------------------------------------------
    // Quote publisher
    // -----------------------------------------------------
    //

    std::cerr
        << "[startup] Creating ITCH quote publisher..."
        << std::endl;

    llt::itch::ItchQuotePublisher
        publisher{
            marketState.instruments(),
            marketEventQueue};

    std::cerr
        << "[startup] ITCH quote publisher ready."
        << std::endl;

    //
    // -----------------------------------------------------
    // Trade publisher
    // -----------------------------------------------------
    //

    std::cerr
        << "[startup] Creating ITCH trade publisher..."
        << std::endl;

    llt::itch::ItchTradePublisher
        tradePublisher{
            marketState.instruments(),
            marketEventQueue};

    std::cerr
        << "[startup] ITCH trade publisher ready."
        << std::endl;

    //
    // -----------------------------------------------------
    // Market-state callbacks
    // -----------------------------------------------------
    //

    std::cerr
        << "[startup] Installing BBO callback..."
        << std::endl;

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

    std::cerr
        << "[startup] BBO callback installed."
        << std::endl;

    std::cerr
        << "[startup] Installing trade callback..."
        << std::endl;

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

    std::cerr
        << "[startup] Trade callback installed."
        << std::endl;

    //
    // -----------------------------------------------------
    // Feed handler
    // -----------------------------------------------------
    //

    std::cerr
        << "[startup] Creating Mold ITCH feed handler..."
        << std::endl;

    llt::itch::MoldItchFeedHandler
        feedHandler{
            source,
            recovery,
            marketState};

    // Dedicated UDP receive pipeline.
    llt::moldudp64::MoldDatagramQueue moldDatagramQueue;

    llt::moldudp64::MoldUdpReceiver udpReceiver{
        source,
        moldDatagramQueue};

    std::cerr
        << "[startup] MoldDatagramQueue capacity: "
        << llt::moldudp64::MoldDatagramQueueCapacity
        << '\n';

    std::cerr
        << "[startup] Mold ITCH feed handler ready."
        << std::endl;

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

    std::atomic<bool> consumerRunning{true};

    std::cerr
        << "[startup] Starting MarketEvent consumer thread..."
        << std::endl;

    std::thread consumerThread(
        [&]()
        {
            std::cerr
                << "[consumer] Consumer thread started."
                << std::endl;

            // while (
            //     running.load(
            //         std::memory_order_relaxed))
            while (consumerRunning.load(std::memory_order_acquire))
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

            std::cerr
                << "[consumer] Shutdown requested. "
                << "Draining MarketEventQueue..."
                << std::endl;

            //
            // Drain events published immediately before
            // shutdown.
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

            std::cerr
                << "[consumer] Consumer thread stopped."
                << std::endl;
        });

    std::cerr
        << "[startup] MarketEvent consumer thread created."
        << std::endl;

    //
    // =====================================================
    // Startup banner
    // =====================================================
    //

    std::cout
        << "========================================\n"
        << "       LIVE MOLDUDP64 ITCH PIPELINE\n"
        << "========================================\n"
        << "UDP port                  : "
        << port
        << '\n'
        << "Receive timeout           : "
        << receiveTimeoutMs
        << " ms\n"
        << "Recovery                  : "
        << "FILE-BACKED\n"
        << "Recovery messages indexed : "
        << recoverySource.indexedMessages()
        << '\n'
        << "Recovery checkpoints      : "
        << recoverySource.checkpointCount()
        << '\n'
        << "MoldDatagramQueue capacity: "
        << llt::moldudp64::MoldDatagramQueueCapacity
        << '\n'
        << "MarketEventQueue capacity : "
        << 65536
        << '\n'
        << "----------------------------------------\n"
        << "Waiting for MoldUDP64 ITCH packets...\n"
        << "Press Ctrl+C to stop.\n"
        << "========================================\n"
        << std::flush;

    //
    // =====================================================
    // Feed thread
    // =====================================================
    //

    std::cerr
        << "[startup] Starting dedicated UDP receiver..."
        << std::endl;

    udpReceiver.start();

    std::cerr
        << "[startup] Starting Mold ITCH feed thread..."
        << std::endl;

    std::thread feedThread(
        [&]()
        {
            std::cerr
                << "[feed] Feed thread started."
                << std::endl;

            std::cerr
                << "[feed] Entering feedHandler.run()."
                << std::endl;

            // feedHandler.run();
            feedHandler.runQueued(
                moldDatagramQueue,
                udpReceiver);

            std::cerr
                << "[feed] feedHandler.run() returned."
                << std::endl;

            std::cerr
                << "[feed] Feed state: "
                << feedStateToString(
                       feedHandler.state())
                << std::endl;

            //
            // Fail-stop.
            //
            // If framing, sequencing, recovery or ITCH
            // processing makes the feed unsafe, terminate
            // the application rather than continue with an
            // invalid market state.
            //
            if (
                feedHandler.state() ==
                llt::FeedHandlerState::Failed)
            {
                std::cerr
                    << "[feed] Feed entered FAILED state. "
                    << "Requesting application shutdown."
                    << std::endl;

                running.store(
                    false,
                    std::memory_order_relaxed);
            }

            std::cerr
                << "[feed] Feed thread stopped."
                << std::endl;
        });

    std::cerr
        << "[startup] Mold ITCH feed thread created."
        << std::endl;

    std::cerr
        << "[startup] Application startup complete."
        << std::endl;

    //
    // =====================================================
    // Runtime monitoring
    // =====================================================
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

    // std::cerr
    //     << "[shutdown] Shutdown requested."
    //     << std::endl;

    // std::cerr
    //     << "[shutdown] Stopping feed handler..."
    //     << std::endl;

    // feedHandler.stop();

    // std::cerr
    //     << "[shutdown] Feed handler stop requested."
    //     << std::endl;

    // if (feedThread.joinable())
    // {
    //     std::cerr
    //         << "[shutdown] Waiting for feed thread..."
    //         << std::endl;

    //     feedThread.join();

    //     // Now safely read diagnostics.
    //     std::cout
    //         << "Actual SO_RCVBUF: "
    //         << source.actualReceiveBufferBytes() << '\n'
    //         << "Receive errors: "
    //         << source.receiveErrorCount() << '\n'
    //         << "Truncated datagrams: "
    //         << source.truncatedDatagramCount() << '\n'
    //         << "Last receive errno: "
    //         << source.lastReceiveError() << '\n';

    //     std::cerr
    //         << "[shutdown] Feed thread joined."
    //         << std::endl;
    // }

    // if (consumerThread.joinable())
    // {
    //     std::cerr
    //         << "[shutdown] Waiting for consumer thread..."
    //         << std::endl;

    //     consumerThread.join();

    //     std::cerr
    //         << "[shutdown] Consumer thread joined."
    //         << std::endl;
    // }

    //
    // =====================================================
    // Shutdown
    // =====================================================
    //

    std::cerr
        << "[shutdown] Shutdown requested."
        << std::endl;

    // 1. Stop the UDP producer first.
    std::cerr
        << "[shutdown] Stopping UDP receiver..."
        << std::endl;

    udpReceiver.stop();
    udpReceiver.join();

    // temp
    const auto timing = udpReceiver.timingStats();

    std::cout << "\n";
    std::cout << "Post-receive work:\n";
    std::cout << "  Max duration (us) : "
              << timing.maxPostReceiveWorkNs / 1000.0 << '\n';
    std::cout << "  Over 10 us        : "
              << timing.postWorkOver10us << '\n';
    std::cout << "  Over 100 us       : "
              << timing.postWorkOver100us << '\n';
    std::cout << "  Over 1 ms         : "
              << timing.postWorkOver1ms << '\n';

    std::cout << "\n";
    std::cout << "SPSC queue push:\n";
    std::cout << "  Over 10 us        : "
              << timing.pushesOver10us << '\n';
    std::cout << "  Over 100 us       : "
              << timing.pushesOver100us << '\n';
    std::cout << "  Over 1 ms         : "
              << timing.pushesOver1ms << '\n';

    std::cout << "\n";
    std::cout << "Receive calls:\n";
    std::cout << "  Successful > 1 ms : "
              << timing.successfulReceivesOver1ms << '\n';
    std::cout << "  Unsuccessful calls: "
              << timing.unsuccessfulReceiveCalls << '\n';
    //

    std::cerr
        << "[shutdown] UDP receiver joined."
        << std::endl;

    // 2. Let the feed processor drain MoldDatagramQueue.
    // runQueued() exits once the producer has stopped
    // and all queued datagrams have been consumed.
    if (feedThread.joinable())
    {
        std::cerr
            << "[shutdown] Draining MoldDatagramQueue..."
            << std::endl;

        feedThread.join();

        std::cerr
            << "[shutdown] Feed thread joined."
            << std::endl;
    }

    // 3. The feed processor can no longer publish events.
    // Now stop and drain the downstream consumer.
    consumerRunning.store(
        false,
        std::memory_order_release);

    if (consumerThread.joinable())
    {
        consumerThread.join();
    }

    // All threads have stopped. Reading diagnostics is safe.
    std::cout
        << "\n"
        << "========================================\n"
        << "       UDP RECEIVER DIAGNOSTICS\n"
        << "========================================\n"
        << "Actual SO_RCVBUF         : "
        << source.actualReceiveBufferBytes() << '\n'
        << "Receive errors           : "
        << source.receiveErrorCount() << '\n'
        << "Truncated datagrams      : "
        << source.truncatedDatagramCount() << '\n'
        << "Last receive errno       : "
        << source.lastReceiveError() << '\n'
        << "Received UDP datagrams   : "
        << udpReceiver.receivedDatagrams() << '\n'
        << "Queued UDP datagrams     : "
        << udpReceiver.queuedDatagrams() << '\n'
        << "Queue-full drops         : "
        << udpReceiver.queueFullDrops() << '\n'
        << "Maximum queue occupancy  : "
        << udpReceiver.maxQueueOccupancy() << '\n'
        << "Remaining UDP datagrams  : "
        << moldDatagramQueue.size() << '\n'
        << "========================================\n";

    //
    // =====================================================
    // Final statistics
    // =====================================================
    //

    std::cout
        << '\n'
        << "========================================\n"
        << "     LIVE MOLDUDP64 ITCH FINAL STATS\n"
        << "========================================\n"

        << "Feed state                : "
        << feedStateToString(
               feedHandler.state())
        << '\n'

        << '\n'

        << "Processed datagrams       : "
        << feedHandler.processedDatagrams()
        << '\n'

        << "Processed live messages   : "
        << feedHandler.processedMessages()
        << '\n'

        << "Recovered messages        : "
        << feedHandler.recoveredMessages()
        << '\n'

        << "Sequence gaps             : "
        << feedHandler.gapsDetected()
        << '\n'

        << "Ignored messages          : "
        << feedHandler.ignoredMessages()
        << '\n'

        << "Malformed datagrams       : "
        << feedHandler.malformedDatagrams()
        << '\n'

        << "Next expected sequence    : "
        << feedHandler.expectedSequence()
        << '\n'

        << '\n'

        << "Recovery requests         : "
        << recoverySource.recoveryRequests()
        << '\n'

        << "File recovered messages   : "
        << recoverySource.recoveredMessages()
        << '\n'

        << '\n'

        << "Instruments               : "
        << marketState.instruments().size()
        << '\n'

        << "Active orders             : "
        << marketState.orders().size()
        << '\n'

        << "Books                     : "
        << marketState.books().size()
        << '\n'

        << '\n'

        << "Published quotes          : "
        << publisher.publishedQuotes()
        << '\n'

        << "Dropped quotes            : "
        << publisher.droppedQuotes()
        << '\n'

        << "Quote unknown instruments : "
        << publisher.unknownInstruments()
        << '\n'

        << '\n'

        << "Published trades          : "
        << tradePublisher.publishedTrades()
        << '\n'

        << "Dropped trades            : "
        << tradePublisher.droppedTrades()
        << '\n'

        << "Trade unknown instruments : "
        << tradePublisher.unknownInstruments()
        << '\n'

        << '\n'

        << "Consumed events           : "
        << consumedEvents.load(
               std::memory_order_relaxed)
        << '\n'

        << "Consumed quotes           : "
        << consumedQuotes.load(
               std::memory_order_relaxed)
        << '\n'

        << "Consumed trades           : "
        << consumedTrades.load(
               std::memory_order_relaxed)
        << '\n'

        << "Events remaining in SPSC  : "
        << marketEventQueue.size()
        << '\n'

        << "========================================\n"
        << std::flush;

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
            << "MoldUDP64 ITCH feed terminated because "
            << "the market-data stream could not be "
            << "safely reconstructed.\n";

        return 1;
    }

    std::cerr
        << "[shutdown] Clean shutdown complete."
        << std::endl;

    return 0;
}