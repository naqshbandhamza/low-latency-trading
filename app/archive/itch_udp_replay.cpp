#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include "market_data/itch/ItchStreamReader.h"
#include "market_data/itch/ItchUdpCodec.h"
#include "market_data/itch/ItchUdpPacket.h"

namespace
{

void printUsage(
    const char* executable)
{
    std::cerr
        << "Usage:\n"
        << "  "
        << executable
        << " <ITCH BinaryFILE> <udp-port> <record-count>\n"
        << '\n'
        << "Example:\n"
        << "  "
        << executable
        << " data/itch/01302019.NASDAQ_ITCH50"
        << " 19000 100000\n";
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

    if (argc != 4)
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
    std::uint64_t bytesSent{0};

    //
    // First correctness run:
    //
    // ~20,000 packets / second.
    //
    // 50 us between packets keeps this intentionally
    // conservative so we validate correctness before
    // stress-testing UDP.
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
        << "----------------------------------------\n";

    const auto startTime =
        std::chrono::steady_clock::now();

    //
    // =====================================================
    // File -> UDP replay
    // =====================================================
    //

    while (
        packetsSent <
        requestedRecords)
    {
        auto record =
            reader.readNext();

        switch (record.status)
        {
        case llt::itch::ItchStreamReadStatus::Message:
        {
            ++recordsRead;

            if (record.empty())
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

            //
            // Make sure the original ITCH message fits
            // inside our UDP packet payload.
            //
            if (
                record.size >
                packet.payload.size())
            {
                std::cerr
                    << "ITCH payload too large for "
                    << "ItchUdpPacket at record "
                    << recordsRead
                    << ". Payload size: "
                    << record.size
                    << '\n';

                ::close(
                    socketFd);

                return 1;
            }

            packet.payloadSize =
                record.size;

            std::memcpy(
                packet.payload.data(),
                record.data,
                record.size);

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
                record.size;

            ++nextSequence;

            //
            // Progress output is deliberately infrequent.
            // We don't want console I/O in the hot path
            // for every packet.
            //
            if (
                packetsSent % 10000 ==
                0)
            {
                std::cout
                    << "Sent "
                    << packetsSent
                    << " packets\n";
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

    const double packetsPerSecond =
        seconds > 0.0
            ? static_cast<double>(
                  packetsSent) /
                  seconds
            : 0.0;

    //
    // =====================================================
    // Cleanup
    // =====================================================
    //

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
        << "Records read : "
        << recordsRead
        << '\n'
        << "Packets sent : "
        << packetsSent
        << '\n'
        << "Payload bytes: "
        << bytesSent
        << '\n'
        << "Last sequence: ";

    if (packetsSent == 0)
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
        << "Elapsed      : "
        << seconds
        << " sec\n"
        << "Rate         : "
        << static_cast<std::uint64_t>(
               packetsPerSecond)
        << " packets/sec\n"
        << "========================================\n";

    return 0;
}