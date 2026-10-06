#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <variant>

#include "market_data/MarketEvent.h"
#include "market_data/MarketEventQueue.h"

#include "market_data/itch/ItchMarketState.h"
#include "market_data/itch/ItchQuotePublisher.h"
#include "market_data/itch/ItchReplay.h"
#include "market_data/itch/ItchTradePublisher.h"

namespace
{

    using Clock =
        std::chrono::steady_clock;

    const char *statusToString(
        llt::itch::ItchReplayStatus status) noexcept
    {
        switch (status)
        {
        case llt::itch::ItchReplayStatus::Complete:
            return "COMPLETE";

        case llt::itch::ItchReplayStatus::IncompleteStream:
            return "INCOMPLETE";

        case llt::itch::ItchReplayStatus::StreamError:
            return "STREAM ERROR";
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
            << " <ITCH BinaryFILE>\n\n"
            << "Example:\n"
            << "  "
            << executable
            << " data/itch/01302019.NASDAQ_ITCH50\n";
    }

} // namespace

int main(
    int argc,
    char *argv[])
{
    // =====================================================
    // CLI
    // =====================================================

    if (argc != 2)
    {
        printUsage(
            argv[0]);

        return 2;
    }

    const std::string filePath =
        argv[1];

    // =====================================================
    // Open BinaryFILE
    // =====================================================

    std::ifstream file(
        filePath,
        std::ios::binary);

    if (!file.is_open())
    {
        std::cerr
            << "Failed to open ITCH file: "
            << filePath
            << '\n';

        return 2;
    }

    // =====================================================
    // Pipeline components
    // =====================================================

    //
    // Reconstructed NASDAQ ITCH market state.
    //
    llt::itch::ItchMarketState
        marketState;

    std::cout
        << "sizeof(MarketEvent)      : "
        << sizeof(llt::MarketEvent)
        << " bytes\n"
        << "alignof(MarketEvent)     : "
        << alignof(llt::MarketEvent)
        << " bytes\n"
        << "Queue payload @ 16384    : "
        << sizeof(llt::MarketEvent) * 16384
        << " bytes\n";

    //
    // Producer -> consumer normalized event queue.
    //
    llt::MarketEventQueue
        marketEventQueue;

    //
    // BBO -> Quote -> MarketEvent -> SPSC.
    //
    llt::itch::ItchQuotePublisher
        quotePublisher{
            marketState.instruments(),
            marketEventQueue};

    //
    // Executions/trade reports
    // -> Trade
    // -> MarketEvent
    // -> SPSC.
    //
    llt::itch::ItchTradePublisher
        tradePublisher{
            marketState.instruments(),
            marketEventQueue};

    // =====================================================
    // Market-state publication callbacks
    // =====================================================

    marketState.setBboChangeHandler(
        [&quotePublisher](
            llt::market_data::InstrumentId instrumentId,
            llt::market_data::Timestamp timestamp,
            const llt::market_data::Bbo &bbo)
        {
            quotePublisher.onBboChange(
                instrumentId,
                timestamp,
                bbo);
        });

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

    // =====================================================
    // Consumer state
    // =====================================================

    std::atomic<bool>
        producerDone{
            false};

    std::uint64_t
        consumedEvents{0};

    std::uint64_t
        consumedQuotes{0};

    std::uint64_t
        consumedTrades{0};

    // =====================================================
    // Consumer thread
    // =====================================================

    //
    // MarketEventQueue is SPSC:
    //
    // producer:
    //     replay / market-state thread
    //
    // consumer:
    //     this thread
    //
    // The consumer exits only when:
    //
    //     producerDone == true
    //
    // AND
    //
    //     the queue has been drained.
    //
    std::thread consumerThread(
        [&]()
        {
            while (true)
            {
                auto event =
                    marketEventQueue.pop();

                if (event.has_value())
                {
                    ++consumedEvents;

                    if (
                        std::holds_alternative<
                            llt::Quote>(*event))
                    {
                        ++consumedQuotes;
                    }
                    else if (
                        std::holds_alternative<
                            llt::Trade>(*event))
                    {
                        ++consumedTrades;
                    }

                    continue;
                }

                if (
                    producerDone.load(
                        std::memory_order_acquire))
                {
                    //
                    // Queue was observed empty after seeing
                    // producer completion.
                    //
                    // No more events can be published.
                    //
                    break;
                }

                //
                // Avoid hammering the core quite as aggressively
                // while waiting for the producer.
                //
                std::this_thread::yield();
            }
        });

    // =====================================================
    // Benchmark header
    // =====================================================

    std::cout
        << "========================================\n"
        << "       ITCH PIPELINE BENCHMARK\n"
        << "========================================\n"
        << "File                  : "
        << filePath
        << '\n'
        << "Queue capacity        : "
        << 16384
        << '\n'
        << "Consumer threads      : "
        << 1
        << '\n'
        << "Build recommendation  : RELEASE\n"
        << "----------------------------------------\n"
        << "Running...\n";

    // =====================================================
    // Timed pipeline
    // =====================================================

    //
    // Timer intentionally includes:
    //
    //   BinaryFILE reading
    //   ITCH decoding
    //   dispatch
    //   market-state updates
    //   order-book reconstruction
    //   Quote generation
    //   Trade generation
    //   SPSC publication
    //   consumer processing
    //
    // This is therefore an END-TO-END BinaryFILE replay
    // benchmark, not a pure decoder microbenchmark.
    //

    const auto start =
        Clock::now();

    const auto result =
        llt::itch::ItchReplay::run(
            file,
            [&marketState](
                const llt::itch::ItchMessage &message)
            {
                marketState.onMessage(
                    message);
            });

    //
    // No further MarketEvents can be produced.
    //
    producerDone.store(
        true,
        std::memory_order_release);

    //
    // Include complete downstream queue drain in the
    // end-to-end measurement.
    //
    consumerThread.join();

    const auto end =
        Clock::now();

    // =====================================================
    // Calculate results
    // =====================================================

    const double elapsedSeconds =
        std::chrono::duration<double>(
            end - start)
            .count();

    const std::uint64_t messagesProcessed =
        result.stats.decodedMessages;

    const double messagesPerSecond =
        elapsedSeconds > 0.0
            ? static_cast<double>(
                  messagesProcessed) /
                  elapsedSeconds
            : 0.0;

    const double millionMessagesPerSecond =
        messagesPerSecond /
        1'000'000.0;

    // =====================================================
    // Results
    // =====================================================

    std::cout
        << std::fixed
        << std::setprecision(3)

        << "----------------------------------------\n"
        << "RESULTS\n"
        << "----------------------------------------\n"

        << "Replay status         : "
        << statusToString(
               result.status)
        << '\n'

        << "Records read          : "
        << result.stats.recordsRead
        << '\n'

        << "Decoded messages      : "
        << result.stats.decodedMessages
        << '\n'

        << "Unsupported messages  : "
        << result.stats.unsupportedMessages
        << '\n'

        << "Malformed messages    : "
        << result.stats.malformedMessages
        << '\n'

        << '\n'

        << "Elapsed               : "
        << elapsedSeconds
        << " sec\n"

        << "Throughput            : "
        << millionMessagesPerSecond
        << " M messages/sec\n"

        << '\n'

        << "Instruments           : "
        << marketState.instruments().size()
        << '\n'

        << "Active orders         : "
        << marketState.orders().size()
        << '\n'

        << "Books                 : "
        << marketState.books().size()
        << '\n'

        << '\n'

        << "Published quotes      : "
        << quotePublisher.publishedQuotes()
        << '\n'

        << "Dropped quotes        : "
        << quotePublisher.droppedQuotes()
        << '\n'

        << "Quote unknown instr.  : "
        << quotePublisher.unknownInstruments()
        << '\n'

        << '\n'

        << "Published trades      : "
        << tradePublisher.publishedTrades()
        << '\n'

        << "Dropped trades        : "
        << tradePublisher.droppedTrades()
        << '\n'

        << "Trade unknown instr.  : "
        << tradePublisher.unknownInstruments()
        << '\n'

        << '\n'

        << "Consumed events       : "
        << consumedEvents
        << '\n'

        << "Consumed quotes       : "
        << consumedQuotes
        << '\n'

        << "Consumed trades       : "
        << consumedTrades
        << '\n'

        << "Queue remaining       : "
        << marketEventQueue.size()
        << '\n'

        << "========================================\n";

    // =====================================================
    // Validation
    // =====================================================

    bool valid =
        true;

    if (
        consumedEvents !=
        quotePublisher.publishedQuotes() +
            tradePublisher.publishedTrades())
    {
        std::cerr
            << "ERROR: consumed event count does not "
            << "match published event count.\n";

        valid =
            false;
    }

    if (
        consumedQuotes !=
        quotePublisher.publishedQuotes())
    {
        std::cerr
            << "ERROR: consumed Quote count does not "
            << "match published Quote count.\n";

        valid =
            false;
    }

    if (
        consumedTrades !=
        tradePublisher.publishedTrades())
    {
        std::cerr
            << "ERROR: consumed Trade count does not "
            << "match published Trade count.\n";

        valid =
            false;
    }

    if (
        marketEventQueue.size() != 0)
    {
        std::cerr
            << "ERROR: MarketEventQueue was not "
            << "completely drained.\n";

        valid =
            false;
    }

    if (
        quotePublisher.droppedQuotes() != 0)
    {
        std::cerr
            << "ERROR: Quote events were dropped.\n";

        valid =
            false;
    }

    if (
        tradePublisher.droppedTrades() != 0)
    {
        std::cerr
            << "ERROR: Trade events were dropped.\n";

        valid =
            false;
    }

    if (!valid)
    {
        return 1;
    }

    if (
        result.status ==
        llt::itch::ItchReplayStatus::StreamError)
    {
        return 1;
    }

    return 0;
}