#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <limits>

#include "market_data/itch/FileItchRecoverySource.h"
#include "market_data/itch/ItchUdpPacket.h"

namespace
{

using Clock =
    std::chrono::steady_clock;

template <typename Duration>
double toMicroseconds(
    const Duration& duration)
{
    return std::chrono::duration<
        double,
        std::micro>(
            duration)
        .count();
}

void benchmarkRecovery(
    llt::itch::FileItchRecoverySource& source,
    std::uint64_t fromSequence,
    std::uint64_t packetCount,
    std::size_t iterations)
{
    std::vector<
        llt::itch::ItchUdpPacket
    > packets;

    //
    // Reserve once so we're mostly measuring:
    //
    // index lookup
    // seekg
    // BinaryFILE reads
    // packet reconstruction
    //
    packets.reserve(
        static_cast<std::size_t>(
            packetCount));

    const auto toSequence =
        fromSequence +
        packetCount;

    //
    // Warm-up.
    //
    // This also gives the OS a chance to populate
    // filesystem/page caches before measurement.
    //
    if (
        !source.recover(
            fromSequence,
            toSequence,
            packets))
    {
        std::cerr
            << "Warm-up recovery failed for ["
            << fromSequence
            << ", "
            << toSequence
            << ").\n";

        return;
    }

    double totalMicroseconds{0.0};
    double minimumMicroseconds{
        std::numeric_limits<double>::max()};
    double maximumMicroseconds{0.0};

    for (
        std::size_t i = 0;
        i < iterations;
        ++i)
    {
        packets.clear();

        const auto start =
            Clock::now();

        const bool recovered =
            source.recover(
                fromSequence,
                toSequence,
                packets);

        const auto end =
            Clock::now();

        if (!recovered)
        {
            std::cerr
                << "Recovery failed during benchmark.\n";

            return;
        }

        const double elapsed =
            toMicroseconds(
                end - start);

        totalMicroseconds +=
            elapsed;

        if (
            elapsed <
            minimumMicroseconds)
        {
            minimumMicroseconds =
                elapsed;
        }

        if (
            elapsed >
            maximumMicroseconds)
        {
            maximumMicroseconds =
                elapsed;
        }
    }

    const double averageMicroseconds =
        totalMicroseconds /
        static_cast<double>(
            iterations);

    std::cout
        << std::left
        << std::setw(14)
        << packetCount
        << std::setw(18)
        << averageMicroseconds
        << std::setw(18)
        << minimumMicroseconds
        << std::setw(18)
        << maximumMicroseconds
        << '\n';
}

} // namespace


int main(
    int argc,
    char* argv[])
{
    if (argc != 3)
    {
        std::cerr
            << "Usage:\n"
            << "  "
            << argv[0]
            << " <ITCH BinaryFILE>"
            << " <start-sequence>\n\n"
            << "Example:\n"
            << "  "
            << argv[0]
            << " data/itch/01302019.NASDAQ_ITCH50"
            << " 500000\n";

        return 2;
    }

    const std::string filePath =
        argv[1];

    const auto startSequence =
        std::strtoull(
            argv[2],
            nullptr,
            10);

    if (startSequence == 0)
    {
        std::cerr
            << "Start sequence must be greater than zero.\n";

        return 2;
    }

    std::cout
        << "========================================\n"
        << "       ITCH RECOVERY BENCHMARK\n"
        << "========================================\n"
        << "File          : "
        << filePath
        << '\n'
        << "Start sequence: "
        << startSequence
        << '\n'
        << "----------------------------------------\n"
        << "Building recovery index...\n";

    //
    // =====================================================
    // INDEX BUILD BENCHMARK
    // =====================================================
    //

    const auto indexStart =
        Clock::now();

    llt::itch::FileItchRecoverySource
        recoverySource{
            filePath};

    const auto indexEnd =
        Clock::now();

    const auto indexMilliseconds =
        std::chrono::duration<
            double,
            std::milli>(
                indexEnd -
                indexStart)
            .count();

    if (
        !recoverySource.indexReady())
    {
        std::cerr
            << "Failed to build recovery index.\n";

        return 1;
    }

    std::cout
        << "Index ready     : YES\n"
        << "Indexed records : "
        << recoverySource.indexedRecords()
        << '\n'
        << "Index build time: "
        << indexMilliseconds
        << " ms\n"
        << "----------------------------------------\n";


    //
    // =====================================================
    // RECOVERY BENCHMARK
    // =====================================================
    //

    constexpr std::size_t iterations =
        1000;

    std::cout
        << "Recovery iterations: "
        << iterations
        << '\n'
        << '\n'
        << std::left
        << std::setw(14)
        << "Gap packets"
        << std::setw(18)
        << "Average us"
        << std::setw(18)
        << "Minimum us"
        << std::setw(18)
        << "Maximum us"
        << '\n'
        << "------------------------------------------------------------\n";

    benchmarkRecovery(
        recoverySource,
        startSequence,
        1,
        iterations);

    benchmarkRecovery(
        recoverySource,
        startSequence,
        10,
        iterations);

    benchmarkRecovery(
        recoverySource,
        startSequence,
        100,
        iterations);

    std::cout
        << "------------------------------------------------------------\n"
        << "Recovery requests: "
        << recoverySource.recoveryRequests()
        << '\n'
        << "Recovered packets: "
        << recoverySource.recoveredPackets()
        << '\n'
        << "========================================\n";

    return 0;
}