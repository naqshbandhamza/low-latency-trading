#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "market_data/itch/ItchMarketState.h"
#include "market_data/itch/ItchReplay.h"

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
    // Core market-data state
    // =====================================================

    //
    // IMPORTANT:
    //
    // There are intentionally NO Quote/Trade publication
    // callbacks attached to this market state.
    //
    // Therefore this benchmark measures:
    //
    // BinaryFILE
    //     ->
    // framing / ITCH decoding
    //     ->
    // ItchMarketState
    //     ->
    // instrument/order/book reconstruction
    //
    // It does NOT measure:
    //
    // Quote publication
    // Trade publication
    // SPSC transport
    // consumer-thread processing
    //
    llt::itch::ItchMarketState
        marketState;

    marketState.orders().reserve(
    2'000'000);

    // =====================================================
    // Header
    // =====================================================

    std::cout
        << "========================================\n"
        << "     ITCH CORE PIPELINE BENCHMARK\n"
        << "========================================\n"

        << "File                  : "
        << filePath
        << '\n'

        << "Pipeline              : "
        << "BinaryFILE -> Decode -> MarketState\n"

        << "Quote publication     : DISABLED\n"
        << "Trade publication     : DISABLED\n"
        << "SPSC                   : DISABLED\n"

        << "Build recommendation  : RELEASE\n"

        << "----------------------------------------\n"
        << "Running...\n";

    // =====================================================
    // Timed region
    // =====================================================

    

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

    const auto end =
        Clock::now();

    // =====================================================
    // Calculate throughput
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

        << "Peak active orders    : "
        << marketState.orders().peakSize()
        << '\n'

        << "Active orders         : "
        << marketState.orders().size()
        << '\n'

        << "Books                 : "
        << marketState.books().size()
        << '\n'

        << "========================================\n";

    const auto removeCalls =
        marketState.bookSideRemoveCalls();

    const auto levelErases =
        marketState.priceLevelEraseCount();

    const auto nonErasingRemovals =
        removeCalls >= levelErases
            ? removeCalls - levelErases
            : 0;

    const double eraseRate =
        removeCalls == 0
            ? 0.0
            : (static_cast<double>(levelErases) /
               static_cast<double>(removeCalls)) *
                  100.0;

    std::cout
        << "\n----------------------------------------\n"
        << "BOOK LEVEL CHURN\n"
        << "----------------------------------------\n"
        << "removeOrder calls       : "
        << removeCalls << '\n'
        << "Price levels erased     : "
        << levelErases << '\n'
        << "Non-erasing removals    : "
        << nonErasingRemovals << '\n'
        << "Level erase rate        : "
        << std::fixed
        << std::setprecision(2)
        << eraseRate
        << "%\n";

    // =====================================================
    // Validation
    // =====================================================

    if (
        result.status ==
        llt::itch::ItchReplayStatus::StreamError)
    {
        std::cerr
            << "ERROR: stream error during benchmark.\n";

        return 1;
    }

    if (
        result.stats.malformedMessages != 0)
    {
        std::cerr
            << "ERROR: malformed ITCH messages encountered.\n";

        return 1;
    }

    return 0;
}