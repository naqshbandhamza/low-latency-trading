#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "market_data/itch/ItchReplay.h"

namespace
{

using Clock =
    std::chrono::steady_clock;


const char* statusToString(
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
    const char* executable)
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
    char* argv[])
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
    // Benchmark header
    // =====================================================

    std::cout
        << "========================================\n"
        << "       ITCH DECODE BENCHMARK\n"
        << "========================================\n"
        << "File                  : "
        << filePath
        << '\n'
        << "Pipeline              : "
        << "BinaryFILE -> Framing -> Decode\n"
        << "Market state          : DISABLED\n"
        << "Quote publication     : DISABLED\n"
        << "Trade publication     : DISABLED\n"
        << "SPSC                   : DISABLED\n"
        << "Build recommendation  : RELEASE\n"
        << "----------------------------------------\n"
        << "Running...\n";


    // =====================================================
    // Timed region
    // =====================================================

    //
    // ItchReplay performs:
    //
    //   BinaryFILE reading
    //       ->
    //   record framing
    //       ->
    //   ITCH message decoding
    //
    // The callback intentionally performs no market-state
    // processing.
    //
    // This therefore isolates the replay/read/decode path
    // from order-book reconstruction and publication.
    //

    const auto start =
        Clock::now();


    const auto result =
        llt::itch::ItchReplay::run(
            file,
            [](
                const llt::itch::ItchMessage&)
            {
                //
                // Intentionally empty.
                //
                // Decoded message has reached the end of
                // the decode-only pipeline.
                //
            });


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

        << "========================================\n";


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