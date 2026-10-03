#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "market_data/itch/ItchStreamReader.h"
#include "market_data/itch/ItchUdpCodec.h"
#include "market_data/itch/ItchUdpPacket.h"

namespace
{

struct DropRange
{
    std::uint64_t first{0};
    std::uint64_t last{0};

    [[nodiscard]]
    bool contains(
        std::uint64_t sequence) const noexcept
    {
        return
            sequence >= first &&
            sequence <= last;
    }

    [[nodiscard]]
    std::uint64_t size() const noexcept
    {
        return
            last - first + 1;
    }
};


bool shouldDropSequence(
    std::uint64_t sequence,
    const std::vector<DropRange>& ranges) noexcept
{
    for (const auto& range : ranges)
    {
        if (range.contains(sequence))
        {
            return true;
        }
    }

    return false;
}


void printUsage(
    const char* executable)
{
    std::cerr
        << "Usage:\n"
        << "  "
        << executable
        << " <ITCH BinaryFILE> <udp-port> <record-count>"
        << " [--drop-range <first> <last>]...\n"
        << '\n'
        << "Examples:\n"
        << "  "
        << executable
        << " data/itch/01302019.NASDAQ_ITCH50"
        << " 19000 100000\n"
        << '\n'
        << "Single packet gap:\n"
        << "  "
        << executable
        << " data/itch/01302019.NASDAQ_ITCH50"
        << " 19000 1000000"
        << " --drop-range 100000 100000\n"
        << '\n'
        << "Multiple gaps:\n"
        << "  "
        << executable
        << " data/itch/01302019.NASDAQ_ITCH50"
        << " 19000 1000000"
        << " --drop-range 100000 100000"
        << " --drop-range 250000 250009"
        << " --drop-range 500000 500099"
        << " --drop-range 800000 800999\n";
}


bool sendPacket(
    int socketFd,
    const sockaddr_in& destination,
    const llt::itch::ItchUdpPacket& packet)
{
    llt::itch::ItchUdpCodec::Datagram
        datagram{};

    std::size_t encodedSize{0};

    if (
        !llt::itch::ItchUdpCodec::encode(
            packet,
            datagram,
            encodedSize))
    {
        return false;
    }

    const auto sent =
        ::sendto(
            socketFd,
            datagram.data(),
            encodedSize,
            0,
            reinterpret_cast<const sockaddr*>(
                &destination),
            sizeof(destination));

    return
        sent ==
        static_cast<ssize_t>(
            encodedSize);
}

} // namespace


int main(
    int argc,
    char* argv[])
{
    //
    // =====================================================
    // CLI
    // =====================================================
    //
    if (argc < 4)
    {
        printUsage(
            argv[0]);

        return 2;
    }

    const std::string filePath =
        argv[1];

    const auto parsedPort =
        std::strtoul(
            argv[2],
            nullptr,
            10);

    if (
        parsedPort == 0 ||
        parsedPort > 65535)
    {
        std::cerr
            << "Invalid UDP port: "
            << argv[2]
            << '\n';

        return 2;
    }

    const auto requestedRecords =
        std::strtoull(
            argv[3],
            nullptr,
            10);

    if (requestedRecords == 0)
    {
        std::cerr
            << "Record count must be greater than zero.\n";

        return 2;
    }

    const auto port =
        static_cast<std::uint16_t>(
            parsedPort);

    //
    // =====================================================
    // Gap injection configuration
    // =====================================================
    //
    std::vector<DropRange>
        dropRanges;

    int argumentIndex = 4;

    while (argumentIndex < argc)
    {
        const std::string option{
            argv[argumentIndex]};

        if (option != "--drop-range")
        {
            std::cerr
                << "Unknown option: "
                << option
                << '\n';

            printUsage(
                argv[0]);

            return 2;
        }

        if (
            argumentIndex + 2 >=
            argc)
        {
            std::cerr
                << "--drop-range requires "
                << "<first> <last>.\n";

            return 2;
        }

        const auto first =
            std::strtoull(
                argv[argumentIndex + 1],
                nullptr,
                10);

        const auto last =
            std::strtoull(
                argv[argumentIndex + 2],
                nullptr,
                10);

        if (
            first == 0 ||
            last == 0 ||
            first > last ||
            last > requestedRecords)
        {
            std::cerr
                << "Invalid drop range ["
                << first
                << ", "
                << last
                << "]. Must satisfy 1 <= first <= last <= "
                << requestedRecords
                << ".\n";

            return 2;
        }

        dropRanges.push_back(
            DropRange{
                first,
                last});

        argumentIndex += 3;
    }

    std::sort(
        dropRanges.begin(),
        dropRanges.end(),
        [](
            const DropRange& lhs,
            const DropRange& rhs)
        {
            return
                lhs.first <
                rhs.first;
        });

    for (
        std::size_t i = 1;
        i < dropRanges.size();
        ++i)
    {
        if (
            dropRanges[i].first <=
            dropRanges[i - 1].last)
        {
            std::cerr
                << "Drop ranges overlap: ["
                << dropRanges[i - 1].first
                << ", "
                << dropRanges[i - 1].last
                << "] and ["
                << dropRanges[i].first
                << ", "
                << dropRanges[i].last
                << "].\n";

            return 2;
        }
    }

    std::uint64_t expectedDroppedPackets{0};

    for (const auto& range : dropRanges)
    {
        expectedDroppedPackets +=
            range.size();
    }

    const auto expectedGapCount =
        static_cast<std::uint64_t>(
            dropRanges.size());

    //
    // =====================================================
    // Open ITCH BinaryFILE
    // =====================================================
    //
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

    llt::itch::ItchStreamReader
        reader{
            file};

    //
    // =====================================================
    // UDP socket
    // =====================================================
    //
    const int socketFd =
        ::socket(
            AF_INET,
            SOCK_DGRAM,
            0);

    if (socketFd < 0)
    {
        std::cerr
            << "Failed to create UDP socket.\n";

        return 1;
    }

    sockaddr_in destination{};

    destination.sin_family =
        AF_INET;

    destination.sin_port =
        htons(port);

    destination.sin_addr.s_addr =
        htonl(INADDR_LOOPBACK);

    //
    // =====================================================
    // Replay configuration
    // =====================================================
    //
    // Start at sequence 1.
    //
    // This sequence belongs to OUR UDP transport framing.
    // It is not a NASDAQ ITCH message field.
    //
    std::uint64_t nextSequence{1};

    std::uint64_t recordsRead{0};
    std::uint64_t packetsSent{0};
    std::uint64_t packetsDropped{0};
    std::uint64_t bytesSent{0};

    //
    // Conservative correctness replay speed.
    //
    constexpr auto packetDelay =
        std::chrono::microseconds{50};

    std::cout
        << "========================================\n"
        << "          ITCH UDP FILE REPLAY\n"
        << "========================================\n"
        << "File        : "
        << filePath
        << '\n'
        << "Destination : 127.0.0.1:"
        << port
        << '\n'
        << "Record limit: "
        << requestedRecords
        << '\n'
        << "Start seq   : "
        << nextSequence
        << '\n'
        << "Packet delay: "
        << packetDelay.count()
        << " us\n"
        << "Drop ranges : ";

    if (dropRanges.empty())
    {
        std::cout
            << "NONE\n";
    }
    else
    {
        std::cout
            << dropRanges.size()
            << '\n';

        for (
            std::size_t i = 0;
            i < dropRanges.size();
            ++i)
        {
            std::cout
                << "  Gap "
                << (i + 1)
                << "       : ["
                << dropRanges[i].first
                << ", "
                << dropRanges[i].last
                << "] ("
                << dropRanges[i].size()
                << " packets)\n";
        }

        std::cout
            << "Expected gaps       : "
            << expectedGapCount
            << '\n'
            << "Expected recovery   : "
            << expectedDroppedPackets
            << " packets\n";
    }

    std::cout
        << "----------------------------------------\n";

    const auto startTime =
        std::chrono::steady_clock::now();

    //
    // =====================================================
    // File -> UDP replay
    // =====================================================
    //
    while (
        recordsRead <
        requestedRecords)
    {
        auto record =
            reader.readNext();

        switch (record.status)
        {
        case llt::itch::ItchStreamReadStatus::Message:
        {
            ++recordsRead;

            if (record.payload.empty())
            {
                std::cerr
                    << "Encountered empty ITCH payload "
                    << "at record "
                    << recordsRead
                    << ".\n";

                ::close(
                    socketFd);

                return 1;
            }

            llt::itch::ItchUdpPacket
                packet{};

            packet.sequence =
                nextSequence;

            if (
                record.payload.size() >
                packet.payload.size())
            {
                std::cerr
                    << "ITCH payload too large for "
                    << "ItchUdpPacket at record "
                    << recordsRead
                    << ". Payload size: "
                    << record.payload.size()
                    << '\n';

                ::close(
                    socketFd);

                return 1;
            }

            packet.payloadSize =
                record.payload.size();

            std::memcpy(
                packet.payload.data(),
                record.payload.data(),
                record.payload.size());

            const bool shouldDrop =
                shouldDropSequence(
                    nextSequence,
                    dropRanges);

            if (shouldDrop)
            {
                ++packetsDropped;

                //
                // Print once at the beginning of each
                // intentionally dropped range.
                //
                for (
                    std::size_t i = 0;
                    i < dropRanges.size();
                    ++i)
                {
                    if (
                        nextSequence ==
                        dropRanges[i].first)
                    {
                        std::cout
                            << "*** INJECTING GAP "
                            << (i + 1)
                            << ": UDP sequences "
                            << dropRanges[i].first
                            << "-"
                            << dropRanges[i].last
                            << " ("
                            << dropRanges[i].size()
                            << " packets) ***\n";

                        break;
                    }
                }
            }
            else
            {
                if (
                    !sendPacket(
                        socketFd,
                        destination,
                        packet))
                {
                    std::cerr
                        << "Failed sending UDP sequence "
                        << nextSequence
                        << " at ITCH record "
                        << recordsRead
                        << ".\n";

                    ::close(
                        socketFd);

                    return 1;
                }

                ++packetsSent;

                bytesSent +=
                    record.payload.size();
            }

            //
            // Always advance. A dropped packet therefore
            // creates a real transport sequence gap.
            //
            ++nextSequence;

            if (
                recordsRead %
                    10000 ==
                0)
            {
                std::cout
                    << "Processed "
                    << recordsRead
                    << " records | sent "
                    << packetsSent
                    << " | dropped "
                    << packetsDropped
                    << '\n';
            }

            std::this_thread::sleep_for(
                packetDelay);

            break;
        }

        case llt::itch::ItchStreamReadStatus::EndOfSession:
        {
            std::cout
                << "Reached ITCH end-of-session marker.\n";

            goto replay_complete;
        }

        case llt::itch::ItchStreamReadStatus::Incomplete:
        {
            std::cout
                << "Reached incomplete/end of BinaryFILE "
                << "after "
                << recordsRead
                << " records.\n";

            goto replay_complete;
        }

        case llt::itch::ItchStreamReadStatus::Error:
        {
            std::cerr
                << "I/O error while reading ITCH file.\n";

            ::close(
                socketFd);

            return 1;
        }
        }
    }

replay_complete:

    const auto endTime =
        std::chrono::steady_clock::now();

    const auto elapsed =
        std::chrono::duration<double>(
            endTime - startTime);

    const double seconds =
        elapsed.count();

    const double sentPacketsPerSecond =
        seconds > 0.0
            ? static_cast<double>(
                  packetsSent) /
                  seconds
            : 0.0;

    ::close(
        socketFd);

    //
    // =====================================================
    // Summary
    // =====================================================
    //
    std::cout
        << "----------------------------------------\n"
        << "Replay complete\n"
        << "----------------------------------------\n"
        << "Records read     : "
        << recordsRead
        << '\n'
        << "Packets sent     : "
        << packetsSent
        << '\n'
        << "Packets dropped  : "
        << packetsDropped
        << '\n'
        << "Expected dropped : "
        << expectedDroppedPackets
        << '\n'
        << "Expected gaps    : "
        << expectedGapCount
        << '\n'
        << "Payload bytes    : "
        << bytesSent
        << '\n'
        << "Last sequence    : ";

    if (recordsRead == 0)
    {
        std::cout
            << "N/A\n";
    }
    else
    {
        std::cout
            << (nextSequence - 1)
            << '\n';
    }

    std::cout
        << "Elapsed          : "
        << seconds
        << " sec\n"
        << "Send rate        : "
        << static_cast<std::uint64_t>(
               sentPacketsPerSecond)
        << " packets/sec\n"
        << "========================================\n";

    return 0;
}
