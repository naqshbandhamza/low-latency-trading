// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <unistd.h>

// #include <chrono>
// #include <cstddef>
// #include <cstdint>
// #include <cstdlib>
// #include <cstring>
// #include <fstream>
// #include <iomanip>
// #include <iostream>
// #include <string>
// #include <vector>

// #include "market_data/itch/ItchStreamReader.h"
// #include "market_data/itch/ItchUdpCodec.h"
// #include "market_data/itch/ItchUdpPacket.h"

// namespace
// {

// //
// // Pre-encoded datagram.
// //
// // We keep both the fixed-size codec storage and the actual
// // number of bytes that should be passed to sendto().
// //
// struct PrebuiltDatagram
// {
//     llt::itch::ItchUdpCodec::Datagram
//         bytes{};

//     std::size_t
//         size{0};
// };


// void printUsage(
//     const char* executable)
// {
//     std::cerr
//         << "Usage:\n"
//         << "  "
//         << executable
//         << " <ITCH-BinaryFILE> <udp-port> <packet-count>\n"
//         << '\n'
//         << "Example:\n"
//         << "  "
//         << executable
//         << " data/itch/01302019.NASDAQ_ITCH50"
//         << " 19000"
//         << " 1000000\n";
// }

// } // namespace


// int main(
//     int argc,
//     char* argv[])
// {
//     //
//     // =====================================================
//     // CLI
//     // =====================================================
//     //

//     if (argc != 4)
//     {
//         printUsage(
//             argv[0]);

//         return 2;
//     }

//     const std::string filePath =
//         argv[1];

//     const auto parsedPort =
//         std::strtoul(
//             argv[2],
//             nullptr,
//             10);

//     if (
//         parsedPort == 0 ||
//         parsedPort > 65535)
//     {
//         std::cerr
//             << "Invalid UDP port: "
//             << argv[2]
//             << '\n';

//         return 2;
//     }

//     const auto requestedPackets =
//         std::strtoull(
//             argv[3],
//             nullptr,
//             10);

//     if (requestedPackets == 0)
//     {
//         std::cerr
//             << "Packet count must be greater than zero.\n";

//         return 2;
//     }

//     const auto port =
//         static_cast<std::uint16_t>(
//             parsedPort);

//     //
//     // =====================================================
//     // Open ITCH BinaryFILE
//     // =====================================================
//     //

//     std::ifstream file(
//         filePath,
//         std::ios::binary);

//     if (!file.is_open())
//     {
//         std::cerr
//             << "Failed to open ITCH file: "
//             << filePath
//             << '\n';

//         return 1;
//     }

//     llt::itch::ItchStreamReader
//         reader{
//             file};

//     //
//     // =====================================================
//     // Startup information
//     // =====================================================
//     //

//     std::cout
//         << "========================================\n"
//         << "        ITCH UDP LOAD GENERATOR\n"
//         << "========================================\n"
//         << "File          : "
//         << filePath
//         << '\n'
//         << "Destination   : 127.0.0.1:"
//         << port
//         << '\n'
//         << "Requested     : "
//         << requestedPackets
//         << " packets\n"
//         << "Delay         : 0 ns\n"
//         << "Mode          : PREBUILT DATAGRAMS\n"
//         << "Send API      : sendto() per packet\n"
//         << "----------------------------------------\n"
//         << "Preloading and encoding packets...\n";

//     //
//     // =====================================================
//     // Prebuild datagrams
//     // =====================================================
//     //
//     // IMPORTANT:
//     //
//     // This entire section happens BEFORE timing begins.
//     //
//     // We deliberately remove:
//     //
//     //   BinaryFILE reads
//     //   ItchUdpPacket construction
//     //   payload copying
//     //   ItchUdpCodec::encode()
//     //
//     // from the timed send loop.
//     //

//     std::vector<PrebuiltDatagram>
//         datagrams;

//     datagrams.reserve(
//         static_cast<std::size_t>(
//             requestedPackets));

//     std::uint64_t
//         nextSequence{1};

//     std::uint64_t
//         recordsRead{0};

//     std::uint64_t
//         itchPayloadBytes{0};

//     std::uint64_t
//         encodedBytes{0};

//     bool finishedReading{false};

//     while (
//         recordsRead <
//             requestedPackets &&
//         !finishedReading)
//     {
//         auto record =
//             reader.readNext();

//         switch (record.status)
//         {
//         case llt::itch::ItchStreamReadStatus::Message:
//         {
//             ++recordsRead;

//             if (record.empty())
//             {
//                 std::cerr
//                     << "Encountered empty ITCH payload "
//                     << "at record "
//                     << recordsRead
//                     << ".\n";

//                 return 1;
//             }

//             //
//             // Build the same packet used by
//             // itch_udp_replay_gap.
//             //

//             llt::itch::ItchUdpPacket
//                 packet{};

//             packet.sequence =
//                 nextSequence;

//             if (
//                 record.size >
//                 packet.payload.size())
//             {
//                 std::cerr
//                     << "ITCH payload too large for "
//                     << "ItchUdpPacket at record "
//                     << recordsRead
//                     << ". Payload size: "
//                     << record.size
//                     << '\n';

//                 return 1;
//             }

//             packet.payloadSize =
//                 record.size;

//             std::memcpy(
//                 packet.payload.data(),
//                 record.data,
//                 record.size);

//             //
//             // Encode the UDP datagram NOW.
//             //
//             // Encoding is intentionally outside
//             // the timed send loop.
//             //

//             PrebuiltDatagram
//                 prebuilt{};

//             if (
//                 !llt::itch::ItchUdpCodec::encode(
//                     packet,
//                     prebuilt.bytes,
//                     prebuilt.size))
//             {
//                 std::cerr
//                     << "Failed encoding UDP sequence "
//                     << nextSequence
//                     << " at ITCH record "
//                     << recordsRead
//                     << ".\n";

//                 return 1;
//             }

//             itchPayloadBytes +=
//                 record.size;

//             encodedBytes +=
//                 prebuilt.size;

//             datagrams.emplace_back(
//                 std::move(
//                     prebuilt));

//             ++nextSequence;

//             if (
//                 recordsRead %
//                     100000 ==
//                 0)
//             {
//                 std::cout
//                     << "Prebuilt "
//                     << recordsRead
//                     << " packets\n";
//             }

//             break;
//         }

//         case llt::itch::ItchStreamReadStatus::EndOfSession:
//         {
//             std::cout
//                 << "Reached ITCH end-of-session marker.\n";

//             finishedReading =
//                 true;

//             break;
//         }

//         case llt::itch::ItchStreamReadStatus::Incomplete:
//         {
//             std::cout
//                 << "Reached incomplete/end of BinaryFILE "
//                 << "after "
//                 << recordsRead
//                 << " records.\n";

//             finishedReading =
//                 true;

//             break;
//         }

//         case llt::itch::ItchStreamReadStatus::Error:
//         {
//             std::cerr
//                 << "I/O error while reading ITCH file.\n";

//             return 1;
//         }
//         }
//     }

//     if (datagrams.empty())
//     {
//         std::cerr
//             << "No ITCH packets were prepared.\n";

//         return 1;
//     }

//     std::cout
//         << "----------------------------------------\n"
//         << "Prebuild complete\n"
//         << "Records read       : "
//         << recordsRead
//         << '\n'
//         << "Packets prepared   : "
//         << datagrams.size()
//         << '\n'
//         << "ITCH payload bytes : "
//         << itchPayloadBytes
//         << '\n'
//         << "Encoded UDP bytes  : "
//         << encodedBytes
//         << '\n'
//         << "Last sequence      : "
//         << (nextSequence - 1)
//         << '\n';

//     //
//     // =====================================================
//     // Create UDP socket
//     // =====================================================
//     //

//     const int socketFd =
//         ::socket(
//             AF_INET,
//             SOCK_DGRAM,
//             0);

//     if (socketFd < 0)
//     {
//         std::cerr
//             << "Failed to create UDP socket.\n";

//         return 1;
//     }

//     //
//     // Request a larger sender-side kernel buffer.
//     //

//     constexpr int sendBufferBytes =
//         8 * 1024 * 1024;

//     if (
//         ::setsockopt(
//             socketFd,
//             SOL_SOCKET,
//             SO_SNDBUF,
//             &sendBufferBytes,
//             sizeof(sendBufferBytes))
//         < 0)
//     {
//         std::cerr
//             << "Warning: failed to increase "
//             << "UDP send buffer.\n";
//     }

//     //
//     // Destination = itch_live.
//     //

//     sockaddr_in destination{};

//     destination.sin_family =
//         AF_INET;

//     destination.sin_port =
//         htons(port);

//     destination.sin_addr.s_addr =
//         htonl(INADDR_LOOPBACK);

//     //
//     // =====================================================
//     // Timed zero-delay send
//     // =====================================================
//     //
//     // This is the important part of this application.
//     //
//     // The timed hot path performs:
//     //
//     //   vector traversal
//     //   sendto()
//     //
//     // There is NO:
//     //
//     //   file reading
//     //   packet construction
//     //   memcpy of ITCH payload
//     //   encoding
//     //   sleep_for()
//     //   progress output
//     //

//     std::cout
//         << "----------------------------------------\n"
//         << "Starting zero-delay send...\n"
//         << "========================================\n";

//     std::uint64_t
//         packetsSent{0};

//     std::uint64_t
//         sendErrors{0};

//     std::uint64_t
//         bytesSent{0};

//     const auto startTime =
//         std::chrono::steady_clock::now();

//     for (
//         const auto& datagram :
//         datagrams)
//     {
//         const auto sent =
//             ::sendto(
//                 socketFd,
//                 datagram.bytes.data(),
//                 datagram.size,
//                 0,
//                 reinterpret_cast<
//                     const sockaddr*>(
//                         &destination),
//                 sizeof(destination));

//         if (
//             sent !=
//             static_cast<ssize_t>(
//                 datagram.size))
//         {
//             ++sendErrors;

//             continue;
//         }

//         ++packetsSent;

//         bytesSent +=
//             static_cast<std::uint64_t>(
//                 sent);
//     }

//     const auto endTime =
//         std::chrono::steady_clock::now();

//     ::close(
//         socketFd);

//     //
//     // =====================================================
//     // Results
//     // =====================================================
//     //

//     const auto elapsed =
//         std::chrono::duration<double>(
//             endTime -
//             startTime);

//     const double seconds =
//         elapsed.count();

//     const double packetRate =
//         seconds > 0.0
//             ? static_cast<double>(
//                   packetsSent) /
//                   seconds
//             : 0.0;

//     //
//     // Our current custom UDP transport carries
//     // exactly one ITCH record per UDP datagram.
//     //
//     const double messageRate =
//         packetRate;

//     const double mibPerSecond =
//         seconds > 0.0
//             ? (
//                   static_cast<double>(
//                       bytesSent) /
//                   (1024.0 * 1024.0)
//               ) /
//                   seconds
//             : 0.0;

//     const double averageNanoseconds =
//         packetsSent > 0
//             ? (
//                   seconds *
//                   1'000'000'000.0
//               ) /
//                   static_cast<double>(
//                       packetsSent)
//             : 0.0;

//     std::cout
//         << std::fixed
//         << std::setprecision(3)

//         << "\n========================================\n"
//         << "              RESULTS\n"
//         << "========================================\n"

//         << "Packets prepared : "
//         << datagrams.size()
//         << '\n'

//         << "Packets sent     : "
//         << packetsSent
//         << '\n'

//         << "Send errors      : "
//         << sendErrors
//         << '\n'

//         << "Bytes sent       : "
//         << bytesSent
//         << '\n'

//         << "Elapsed          : "
//         << seconds
//         << " sec\n"

//         << "Packet rate      : "
//         << packetRate
//         << " packets/sec\n"

//         << "Message rate     : "
//         << messageRate
//         << " messages/sec\n"

//         << "Avg send time    : "
//         << averageNanoseconds
//         << " ns/packet\n"

//         << "Throughput       : "
//         << mibPerSecond
//         << " MiB/sec\n"

//         << "========================================\n";

//     return
//         sendErrors == 0
//             ? 0
//             : 1;
// }



#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "market_data/itch/ItchStreamReader.h"
#include "market_data/itch/ItchUdpCodec.h"
#include "market_data/itch/ItchUdpPacket.h"

namespace
{

//
// Pre-encoded UDP datagram.
//
// The ITCH BinaryFILE has already been read and the custom
// UDP packet has already been encoded before timing begins.
//
struct PrebuiltDatagram
{
    llt::itch::ItchUdpCodec::Datagram
        bytes{};

    std::size_t
        size{0};
};


void printUsage(
    const char* executable)
{
    std::cerr
        << "Usage:\n"
        << "  "
        << executable
        << " <ITCH-BinaryFILE> <udp-port> <packet-count>\n"
        << '\n'
        << "Example:\n"
        << "  "
        << executable
        << " data/itch/01302019.NASDAQ_ITCH50"
        << " 19000"
        << " 1000000\n";
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

    const auto requestedPackets =
        std::strtoull(
            argv[3],
            nullptr,
            10);

    if (requestedPackets == 0)
    {
        std::cerr
            << "Packet count must be greater than zero.\n";

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

        return 1;
    }

    llt::itch::ItchStreamReader
        reader{
            file};

    //
    // =====================================================
    // Startup information
    // =====================================================
    //

    std::cout
        << "========================================\n"
        << "        ITCH UDP LOAD GENERATOR\n"
        << "========================================\n"
        << "File          : "
        << filePath
        << '\n'
        << "Destination   : 127.0.0.1:"
        << port
        << '\n'
        << "Requested     : "
        << requestedPackets
        << " packets\n"
        << "Delay         : 0 ns\n"
        << "Mode          : PREBUILT DATAGRAMS\n"
        << "Send API      : connected send() per packet\n"
        << "----------------------------------------\n"
        << "Preloading and encoding packets...\n";

    //
    // =====================================================
    // Prebuild datagrams
    // =====================================================
    //
    // Everything expensive happens BEFORE the timer:
    //
    //   BinaryFILE read
    //   ItchUdpPacket construction
    //   payload memcpy
    //   ItchUdpCodec::encode()
    //
    // The timed hot path therefore measures primarily
    // connected UDP send() performance.
    //

    std::vector<PrebuiltDatagram>
        datagrams;

    datagrams.reserve(
        static_cast<std::size_t>(
            requestedPackets));

    std::uint64_t
        nextSequence{1};

    std::uint64_t
        recordsRead{0};

    std::uint64_t
        itchPayloadBytes{0};

    std::uint64_t
        encodedBytes{0};

    bool finishedReading{false};

    while (
        recordsRead <
            requestedPackets &&
        !finishedReading)
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

                return 1;
            }

            //
            // Build exactly the same custom UDP packet
            // expected by UdpItchMarketDataSource.
            //

            llt::itch::ItchUdpPacket
                packet{};

            packet.sequence =
                nextSequence;

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

                return 1;
            }

            packet.payloadSize =
                record.size;

            std::memcpy(
                packet.payload.data(),
                record.data,
                record.size);

            //
            // Encode now so encoding does NOT occur
            // inside the timed send loop.
            //

            PrebuiltDatagram
                prebuilt{};

            if (
                !llt::itch::ItchUdpCodec::encode(
                    packet,
                    prebuilt.bytes,
                    prebuilt.size))
            {
                std::cerr
                    << "Failed encoding UDP sequence "
                    << nextSequence
                    << " at ITCH record "
                    << recordsRead
                    << ".\n";

                return 1;
            }

            itchPayloadBytes +=
                record.size;

            encodedBytes +=
                prebuilt.size;

            datagrams.emplace_back(
                std::move(
                    prebuilt));

            ++nextSequence;

            if (
                recordsRead %
                    100000 ==
                0)
            {
                std::cout
                    << "Prebuilt "
                    << recordsRead
                    << " packets\n";
            }

            break;
        }

        case llt::itch::ItchStreamReadStatus::EndOfSession:
        {
            std::cout
                << "Reached ITCH end-of-session marker.\n";

            finishedReading =
                true;

            break;
        }

        case llt::itch::ItchStreamReadStatus::Incomplete:
        {
            std::cout
                << "Reached incomplete/end of BinaryFILE "
                << "after "
                << recordsRead
                << " records.\n";

            finishedReading =
                true;

            break;
        }

        case llt::itch::ItchStreamReadStatus::Error:
        {
            std::cerr
                << "I/O error while reading ITCH file.\n";

            return 1;
        }
        }
    }

    if (datagrams.empty())
    {
        std::cerr
            << "No ITCH packets were prepared.\n";

        return 1;
    }

    std::cout
        << "----------------------------------------\n"
        << "Prebuild complete\n"
        << "Records read       : "
        << recordsRead
        << '\n'
        << "Packets prepared   : "
        << datagrams.size()
        << '\n'
        << "ITCH payload bytes : "
        << itchPayloadBytes
        << '\n'
        << "Encoded UDP bytes  : "
        << encodedBytes
        << '\n'
        << "Last sequence      : "
        << (nextSequence - 1)
        << '\n';

    //
    // =====================================================
    // Create UDP socket
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

    //
    // =====================================================
    // Configure sender socket buffer
    // =====================================================
    //

    constexpr int sendBufferBytes =
        8 * 1024 * 1024;

    if (
        ::setsockopt(
            socketFd,
            SOL_SOCKET,
            SO_SNDBUF,
            &sendBufferBytes,
            sizeof(sendBufferBytes))
        < 0)
    {
        std::cerr
            << "Warning: failed to increase "
            << "UDP send buffer.\n";
    }

    //
    // =====================================================
    // Destination
    // =====================================================
    //

    sockaddr_in destination{};

    destination.sin_family =
        AF_INET;

    destination.sin_port =
        htons(port);

    destination.sin_addr.s_addr =
        htonl(INADDR_LOOPBACK);

    //
    // =====================================================
    // Connect UDP socket
    // =====================================================
    //
    // This is NOT a TCP-style connection.
    //
    // UDP remains datagram based.
    //
    // connect() simply associates this socket with a fixed
    // destination so that send() can be used instead of
    // supplying the destination to sendto() on every packet.
    //
    // This happens ONCE before timing starts.
    //

    if (
        ::connect(
            socketFd,
            reinterpret_cast<
                const sockaddr*>(
                    &destination),
            sizeof(destination))
        < 0)
    {
        std::cerr
            << "Failed to connect UDP socket.\n";

        ::close(
            socketFd);

        return 1;
    }

    //
    // =====================================================
    // Timed zero-delay send
    // =====================================================
    //
    // HOT PATH:
    //
    //      prebuilt datagram
    //             ↓
    //           send()
    //
    // No:
    //
    //   file I/O
    //   ITCH record parsing
    //   packet construction
    //   memcpy
    //   UDP encoding
    //   destination construction
    //   sleep
    //   progress printing
    //

    std::cout
        << "----------------------------------------\n"
        << "Starting zero-delay connected send...\n"
        << "========================================\n";

    std::uint64_t
        packetsSent{0};

    std::uint64_t
        sendErrors{0};

    std::uint64_t
        bytesSent{0};

    const auto startTime =
        std::chrono::steady_clock::now();

    for (
        const auto& datagram :
        datagrams)
    {
        const auto sent =
            ::send(
                socketFd,
                datagram.bytes.data(),
                datagram.size,
                0);

        if (
            sent !=
            static_cast<ssize_t>(
                datagram.size))
        {
            ++sendErrors;

            continue;
        }

        ++packetsSent;

        bytesSent +=
            static_cast<std::uint64_t>(
                sent);
    }

    const auto endTime =
        std::chrono::steady_clock::now();

    ::close(
        socketFd);

    //
    // =====================================================
    // Results
    // =====================================================
    //

    const auto elapsed =
        std::chrono::duration<double>(
            endTime -
            startTime);

    const double seconds =
        elapsed.count();

    const double packetRate =
        seconds > 0.0
            ? static_cast<double>(
                  packetsSent) /
                  seconds
            : 0.0;

    //
    // Current custom transport:
    //
    //     1 UDP datagram
    //          =
    //     1 ITCH message
    //
    const double messageRate =
        packetRate;

    const double mibPerSecond =
        seconds > 0.0
            ? (
                  static_cast<double>(
                      bytesSent) /
                  (1024.0 * 1024.0)
              ) /
                  seconds
            : 0.0;

    const double averageNanoseconds =
        packetsSent > 0
            ? (
                  seconds *
                  1'000'000'000.0
              ) /
                  static_cast<double>(
                      packetsSent)
            : 0.0;

    //
    // =====================================================
    // Final output
    // =====================================================
    //

    std::cout
        << std::fixed
        << std::setprecision(3)

        << "\n========================================\n"
        << "              RESULTS\n"
        << "========================================\n"

        << "Send API         : connected send()\n"

        << "Packets prepared : "
        << datagrams.size()
        << '\n'

        << "Packets sent     : "
        << packetsSent
        << '\n'

        << "Send errors      : "
        << sendErrors
        << '\n'

        << "Bytes sent       : "
        << bytesSent
        << '\n'

        << "Elapsed          : "
        << seconds
        << " sec\n"

        << "Packet rate      : "
        << packetRate
        << " packets/sec\n"

        << "Message rate     : "
        << messageRate
        << " messages/sec\n"

        << "Avg send time    : "
        << averageNanoseconds
        << " ns/packet\n"

        << "Throughput       : "
        << mibPerSecond
        << " MiB/sec\n"

        << "========================================\n";

    return
        sendErrors == 0
            ? 0
            : 1;
}