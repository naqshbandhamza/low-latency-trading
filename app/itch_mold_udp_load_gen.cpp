// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <unistd.h>

// #include <chrono>
// #include <cstddef>
// #include <cstdint>
// #include <cstdlib>
// #include <fstream>
// #include <iomanip>
// #include <iostream>
// #include <string>
// #include <vector>

// #include "market_data/itch/ItchStreamReader.h"

// #include "market_data/moldudp64/MoldUdp64.h"
// #include "market_data/moldudp64/MoldUdp64Codec.h"

// namespace
// {

// struct PrebuiltMoldDatagram
// {
//     llt::moldudp64::Datagram
//         bytes{};

//     std::size_t
//         size{0};

//     std::uint16_t
//         messageCount{0};

//     std::uint64_t
//         firstSequence{0};
// };

// void printUsage(
//     const char* executable)
// {
//     std::cerr
//         << "Usage:\n"
//         << "  "
//         << executable
//         << " <ITCH-BinaryFILE> <udp-port> <message-count>\n"
//         << '\n'
//         << "Example:\n"
//         << "  "
//         << executable
//         << " data/itch/01302019.NASDAQ_ITCH50"
//         << " 19000"
//         << " 1000000\n";
// }

// llt::moldudp64::Session
// makeSession()
// {
//     llt::moldudp64::Session
//         session{};

//     //
//     // Historical BinaryFILE does not contain a MoldUDP64
//     // session identifier, so the replay/load generator
//     // supplies a fixed synthetic 10-byte session.
//     //
//     constexpr char value[] =
//         "20190130A ";

//     static_assert(
//         sizeof(value) - 1 ==
//         llt::moldudp64::SessionSize);

//     for (
//         std::size_t i = 0;
//         i < llt::moldudp64::SessionSize;
//         ++i)
//     {
//         session[i] =
//             value[i];
//     }

//     return session;
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

//     const auto requestedMessages =
//         std::strtoull(
//             argv[3],
//             nullptr,
//             10);

//     if (requestedMessages == 0)
//     {
//         std::cerr
//             << "Message count must be greater than zero.\n";

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
//         << "     ITCH MOLDUDP64 LOAD GENERATOR\n"
//         << "========================================\n"
//         << "File             : "
//         << filePath
//         << '\n'
//         << "Destination      : 127.0.0.1:"
//         << port
//         << '\n'
//         << "Requested        : "
//         << requestedMessages
//         << " ITCH messages\n"
//         << "Max datagram     : "
//         << llt::moldudp64::MaxDatagramSize
//         << " bytes\n"
//         << "Mold session     : 20190130A \n"
//         << "Delay            : 0 ns\n"
//         << "Mode             : PREBUILT MOLD DATAGRAMS\n"
//         << "Send API         : connected send()\n"
//         << "----------------------------------------\n"
//         << "Preloading and packing messages...\n";

//     //
//     // =====================================================
//     // Prebuild MoldUDP64 datagrams
//     // =====================================================
//     //
//     // Everything expensive happens BEFORE timing:
//     //
//     //   BinaryFILE reading
//     //   Mold header construction
//     //   ITCH message framing
//     //   payload copying
//     //   datagram packing
//     //
//     // The timed hot path therefore remains:
//     //
//     //   prebuilt datagram
//     //          ↓
//     //        send()
//     //
//     // This keeps the benchmark comparable to the existing
//     // connected-send custom UDP benchmark.
//     //

//     std::vector<PrebuiltMoldDatagram>
//         datagrams;

//     //
//     // We cannot know the exact datagram count until packing
//     // occurs.
//     //
//     // Reserve a conservative amount only to reduce vector
//     // reallocations.
//     //
//     datagrams.reserve(
//         static_cast<std::size_t>(
//             requestedMessages / 10 + 1));

//     const auto session =
//         makeSession();

//     std::uint64_t nextSequence{1};

//     std::uint64_t messagesRead{0};

//     std::uint64_t itchPayloadBytes{0};

//     std::uint64_t encodedBytes{0};

//     std::uint64_t minMessagesPerDatagram{0};

//     std::uint64_t maxMessagesPerDatagram{0};

//     PrebuiltMoldDatagram
//         current{};

//     bool packetOpen{false};

//     bool finishedReading{false};

//     auto beginCurrentPacket =
//         [&](std::uint64_t firstSequence)
//         {
//             current =
//                 PrebuiltMoldDatagram{};

//             current.firstSequence =
//                 firstSequence;

//             current.messageCount =
//                 0;

//             if (
//                 !llt::moldudp64::MoldUdp64Codec::
//                     beginPacket(
//                         session,
//                         firstSequence,
//                         current.bytes,
//                         current.size))
//             {
//                 return false;
//             }

//             packetOpen =
//                 true;

//             return true;
//         };

//     auto finalizeCurrentPacket =
//         [&]()
//         {
//             if (
//                 !packetOpen ||
//                 current.messageCount == 0)
//             {
//                 return;
//             }

//             llt::moldudp64::MoldUdp64Codec::
//                 finalizePacket(
//                     current.bytes,
//                     current.messageCount);

//             encodedBytes +=
//                 static_cast<std::uint64_t>(
//                     current.size);

//             if (
//                 minMessagesPerDatagram == 0 ||
//                 current.messageCount <
//                     minMessagesPerDatagram)
//             {
//                 minMessagesPerDatagram =
//                     current.messageCount;
//             }

//             if (
//                 current.messageCount >
//                 maxMessagesPerDatagram)
//             {
//                 maxMessagesPerDatagram =
//                     current.messageCount;
//             }

//             datagrams.emplace_back(
//                 std::move(
//                     current));

//             current =
//                 PrebuiltMoldDatagram{};

//             packetOpen =
//                 false;
//         };

//     while (
//         messagesRead <
//             requestedMessages &&
//         !finishedReading)
//     {
//         auto record =
//             reader.readNext();

//         switch (record.status)
//         {
//         case llt::itch::ItchStreamReadStatus::Message:
//         {
//             if (record.empty())
//             {
//                 std::cerr
//                     << "Encountered empty ITCH payload "
//                     << "at BinaryFILE message "
//                     << (messagesRead + 1)
//                     << ".\n";

//                 return 1;
//             }

//             //
//             // Mold uses a 16-bit message length prefix.
//             //
//             // In practice ITCH messages are far smaller,
//             // but validate the actual framing constraint.
//             //
//             if (
//                 record.size >
//                 0xFFFFu)
//             {
//                 std::cerr
//                     << "ITCH payload exceeds MoldUDP64 "
//                     << "16-bit message length at message "
//                     << (messagesRead + 1)
//                     << ". Payload size: "
//                     << record.size
//                     << '\n';

//                 return 1;
//             }

//             //
//             // A message must also be capable of fitting
//             // inside one of OUR configured datagrams:
//             //
//             // Mold header
//             // + 2-byte message length
//             // + message bytes
//             //
//             if (
//                 llt::moldudp64::HeaderSize +
//                     sizeof(std::uint16_t) +
//                     record.size >
//                 llt::moldudp64::MaxDatagramSize)
//             {
//                 std::cerr
//                     << "ITCH payload cannot fit inside "
//                     << "configured Mold datagram at message "
//                     << (messagesRead + 1)
//                     << ". Payload size: "
//                     << record.size
//                     << '\n';

//                 return 1;
//             }

//             if (!packetOpen)
//             {
//                 if (
//                     !beginCurrentPacket(
//                         nextSequence))
//                 {
//                     std::cerr
//                         << "Failed beginning Mold packet "
//                         << "at sequence "
//                         << nextSequence
//                         << ".\n";

//                     return 1;
//                 }
//             }

//             //
//             // Try to append the message to the current
//             // Mold datagram.
//             //
//             if (
//                 !llt::moldudp64::MoldUdp64Codec::
//                     appendMessage(
//                         record.data,
//                         record.size,
//                         current.bytes,
//                         current.size,
//                         current.messageCount))
//             {
//                 //
//                 // The current packet is full.
//                 //
//                 // IMPORTANT:
//                 //
//                 // Do NOT advance nextSequence.
//                 //
//                 // This ITCH message has NOT been appended
//                 // anywhere yet.
//                 //
//                 // Finalize the previous packet, begin a new
//                 // packet whose first sequence is THIS
//                 // message's sequence, then retry the SAME
//                 // record.
//                 //

//                 if (current.messageCount == 0)
//                 {
//                     std::cerr
//                         << "ITCH message could not fit into "
//                         << "an empty Mold datagram at sequence "
//                         << nextSequence
//                         << ".\n";

//                     return 1;
//                 }

//                 finalizeCurrentPacket();

//                 if (
//                     !beginCurrentPacket(
//                         nextSequence))
//                 {
//                     std::cerr
//                         << "Failed beginning Mold packet "
//                         << "at sequence "
//                         << nextSequence
//                         << ".\n";

//                     return 1;
//                 }

//                 if (
//                     !llt::moldudp64::MoldUdp64Codec::
//                         appendMessage(
//                             record.data,
//                             record.size,
//                             current.bytes,
//                             current.size,
//                             current.messageCount))
//                 {
//                     std::cerr
//                         << "ITCH message still could not fit "
//                         << "inside an empty Mold datagram "
//                         << "at sequence "
//                         << nextSequence
//                         << ". Payload size: "
//                         << record.size
//                         << '\n';

//                     return 1;
//                 }
//             }

//             //
//             // Only NOW has this ITCH message been
//             // successfully assigned to a Mold datagram.
//             //
//             ++messagesRead;

//             itchPayloadBytes +=
//                 static_cast<std::uint64_t>(
//                     record.size);

//             ++nextSequence;

//             if (
//                 messagesRead %
//                     100000 ==
//                 0)
//             {
//                 std::cout
//                     << "Packed "
//                     << messagesRead
//                     << " messages into "
//                     << datagrams.size()
//                     << " completed datagrams"
//                     << '\n';
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
//                 << messagesRead
//                 << " messages.\n";

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

//     //
//     // Flush the final partially filled Mold packet.
//     //
//     finalizeCurrentPacket();

//     if (datagrams.empty())
//     {
//         std::cerr
//             << "No MoldUDP64 datagrams were prepared.\n";

//         return 1;
//     }

//     const auto preparedDatagrams =
//         static_cast<std::uint64_t>(
//             datagrams.size());

//     const double averageMessagesPerDatagram =
//         preparedDatagrams > 0
//             ? static_cast<double>(
//                   messagesRead) /
//                   static_cast<double>(
//                       preparedDatagrams)
//             : 0.0;

//     const double averageDatagramBytes =
//         preparedDatagrams > 0
//             ? static_cast<double>(
//                   encodedBytes) /
//                   static_cast<double>(
//                       preparedDatagrams)
//             : 0.0;

//     std::cout
//         << "----------------------------------------\n"
//         << "Prebuild complete\n"
//         << "ITCH messages      : "
//         << messagesRead
//         << '\n'
//         << "Mold datagrams     : "
//         << preparedDatagrams
//         << '\n'
//         << "ITCH payload bytes : "
//         << itchPayloadBytes
//         << '\n'
//         << "Encoded Mold bytes : "
//         << encodedBytes
//         << '\n'
//         << "First sequence     : 1\n"
//         << "Last sequence      : "
//         << (nextSequence - 1)
//         << '\n'
//         << std::fixed
//         << std::setprecision(3)
//         << "Avg msg/datagram   : "
//         << averageMessagesPerDatagram
//         << '\n'
//         << "Min msg/datagram   : "
//         << minMessagesPerDatagram
//         << '\n'
//         << "Max msg/datagram   : "
//         << maxMessagesPerDatagram
//         << '\n'
//         << "Avg datagram bytes : "
//         << averageDatagramBytes
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
//     // =====================================================
//     // Sender socket buffer
//     // =====================================================
//     //
//     // Preserve the same requested sender-side buffer from
//     // the existing connected-send benchmark.
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
//     // =====================================================
//     // Destination
//     // =====================================================
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
//     // Connect UDP socket
//     // =====================================================
//     //
//     // UDP remains datagram based.
//     //
//     // connect() only associates the socket with a fixed
//     // peer, allowing the timed loop to use send().
//     //

//     if (
//         ::connect(
//             socketFd,
//             reinterpret_cast<
//                 const sockaddr*>(
//                     &destination),
//             sizeof(destination))
//         < 0)
//     {
//         std::cerr
//             << "Failed to connect UDP socket.\n";

//         ::close(
//             socketFd);

//         return 1;
//     }

//     //
//     // =====================================================
//     // Timed zero-delay send
//     // =====================================================
//     //
//     // HOT PATH:
//     //
//     //     prebuilt Mold datagram
//     //              ↓
//     //            send()
//     //
//     // No:
//     //
//     //   BinaryFILE I/O
//     //   ITCH parsing
//     //   Mold construction
//     //   payload copying
//     //   destination construction
//     //   sleeping
//     //   progress printing
//     //

//     std::cout
//         << "----------------------------------------\n"
//         << "Starting zero-delay connected Mold send...\n"
//         << "========================================\n";

//     std::uint64_t datagramsSent{0};

//     std::uint64_t messagesSent{0};

//     std::uint64_t sendErrors{0};

//     std::uint64_t bytesSent{0};

//     const auto startTime =
//         std::chrono::steady_clock::now();

//     for (
//         const auto& datagram :
//         datagrams)
//     {
//         const auto sent =
//             ::send(
//                 socketFd,
//                 datagram.bytes.data(),
//                 datagram.size,
//                 0);

//         if (
//             sent !=
//             static_cast<ssize_t>(
//                 datagram.size))
//         {
//             ++sendErrors;

//             continue;
//         }

//         ++datagramsSent;

//         messagesSent +=
//             datagram.messageCount;

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

//     const double datagramRate =
//         seconds > 0.0
//             ? static_cast<double>(
//                   datagramsSent) /
//                   seconds
//             : 0.0;

//     const double messageRate =
//         seconds > 0.0
//             ? static_cast<double>(
//                   messagesSent) /
//                   seconds
//             : 0.0;

//     const double mibPerSecond =
//         seconds > 0.0
//             ? (
//                   static_cast<double>(
//                       bytesSent) /
//                   (1024.0 * 1024.0)
//               ) /
//                   seconds
//             : 0.0;

//     const double averageNanosecondsPerDatagram =
//         datagramsSent > 0
//             ? (
//                   seconds *
//                   1'000'000'000.0
//               ) /
//                   static_cast<double>(
//                       datagramsSent)
//             : 0.0;

//     const double averageNanosecondsPerMessage =
//         messagesSent > 0
//             ? (
//                   seconds *
//                   1'000'000'000.0
//               ) /
//                   static_cast<double>(
//                       messagesSent)
//             : 0.0;

//     const double sentAverageMessagesPerDatagram =
//         datagramsSent > 0
//             ? static_cast<double>(
//                   messagesSent) /
//                   static_cast<double>(
//                       datagramsSent)
//             : 0.0;

//     //
//     // =====================================================
//     // Final output
//     // =====================================================
//     //

//     std::cout
//         << std::fixed
//         << std::setprecision(3)

//         << "\n========================================\n"
//         << "          MOLDUDP64 RESULTS\n"
//         << "========================================\n"

//         << "Send API          : connected send()\n"

//         << "Messages prepared : "
//         << messagesRead
//         << '\n'

//         << "Datagrams prepared: "
//         << preparedDatagrams
//         << '\n'

//         << "Datagrams sent    : "
//         << datagramsSent
//         << '\n'

//         << "Messages sent     : "
//         << messagesSent
//         << '\n'

//         << "Send errors       : "
//         << sendErrors
//         << '\n'

//         << "Bytes sent        : "
//         << bytesSent
//         << '\n'

//         << "Elapsed           : "
//         << seconds
//         << " sec\n"

//         << "Datagram rate     : "
//         << datagramRate
//         << " datagrams/sec\n"

//         << "ITCH message rate : "
//         << messageRate
//         << " messages/sec\n"

//         << "Avg msg/datagram  : "
//         << sentAverageMessagesPerDatagram
//         << '\n'

//         << "Avg send time     : "
//         << averageNanosecondsPerDatagram
//         << " ns/datagram\n"

//         << "Effective msg time: "
//         << averageNanosecondsPerMessage
//         << " ns/message\n"

//         << "Throughput        : "
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
#include <fstream>
#include <iomanip>
#include <thread>
#include <iostream>
#include <string>
#include <vector>

#include "market_data/itch/ItchStreamReader.h"

#include "market_data/moldudp64/MoldUdp64.h"
#include "market_data/moldudp64/MoldUdp64Codec.h"

namespace
{

    struct PrebuiltMoldDatagram
    {
        llt::moldudp64::Datagram
            bytes{};

        std::size_t
            size{0};

        std::uint16_t
            messageCount{0};

        std::uint64_t
            firstSequence{0};
    };

    void printUsage(
        const char *executable)
    {
        std::cerr
            << "Usage:\n  " << executable
            << " <ITCH-BinaryFILE> <udp-port> <message-count> [target-messages-per-second]\n\n"
            << "Examples:\n  Unthrottled:\n    " << executable
            << " data/itch/01302019.NASDAQ_ITCH50 19000 1000000\n\n"
            << "  Controlled 1M msg/sec:\n    " << executable
            << " data/itch/01302019.NASDAQ_ITCH50 19000 10000000 1000000\n";
    }

    llt::moldudp64::Session
    makeSession()
    {
        llt::moldudp64::Session
            session{};

        //
        // Historical BinaryFILE does not contain a MoldUDP64
        // session identifier, so the replay/load generator
        // supplies a fixed synthetic 10-byte session.
        //
        constexpr char value[] =
            "20190130A ";

        static_assert(
            sizeof(value) - 1 ==
            llt::moldudp64::SessionSize);

        for (
            std::size_t i = 0;
            i < llt::moldudp64::SessionSize;
            ++i)
        {
            session[i] =
                value[i];
        }

        return session;
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

    if (argc != 4 && argc != 5)
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

    const auto requestedMessages =
        std::strtoull(
            argv[3],
            nullptr,
            10);

    if (requestedMessages == 0)
    {
        std::cerr
            << "Message count must be greater than zero.\n";

        return 2;
    }

    std::uint64_t targetMessagesPerSecond{0};

    if (argc == 5)
    {
        targetMessagesPerSecond = std::strtoull(argv[4], nullptr, 10);

        if (targetMessagesPerSecond == 0)
        {
            std::cerr << "Target message rate must be greater than zero.\n";
            return 2;
        }
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
        << "     ITCH MOLDUDP64 LOAD GENERATOR\n"
        << "========================================\n"
        << "File             : " << filePath << '\n'
        << "Destination      : 127.0.0.1:" << port << '\n'
        << "Requested        : " << requestedMessages << " ITCH messages\n"
        << "Max datagram     : " << llt::moldudp64::MaxDatagramSize << " bytes\n"
        << "Mold session     : 20190130A \n"
        << "Mode             : "
        << (targetMessagesPerSecond == 0 ? "UNTHROTTLED" : "CONTROLLED RATE")
        << '\n'
        << "Target rate      : ";

    if (targetMessagesPerSecond == 0)
        std::cout << "MAXIMUM\n";
    else
        std::cout << targetMessagesPerSecond << " ITCH messages/sec\n";

    std::cout
        << "Prebuild mode    : PREBUILT MOLD DATAGRAMS\n"
        << "Send API         : connected send()\n"
        << "----------------------------------------\n"
        << "Preloading and packing messages...\n";

    //
    // =====================================================
    // Prebuild MoldUDP64 datagrams
    // =====================================================
    //
    // Everything expensive happens BEFORE timing:
    //
    //   BinaryFILE reading
    //   Mold header construction
    //   ITCH message framing
    //   payload copying
    //   datagram packing
    //
    // The timed hot path therefore remains:
    //
    //   prebuilt datagram
    //          ↓
    //        send()
    //
    // This keeps the benchmark comparable to the existing
    // connected-send custom UDP benchmark.
    //

    std::vector<PrebuiltMoldDatagram>
        datagrams;

    //
    // We cannot know the exact datagram count until packing
    // occurs.
    //
    // Reserve a conservative amount only to reduce vector
    // reallocations.
    //
    datagrams.reserve(
        static_cast<std::size_t>(
            requestedMessages / 10 + 1));

    const auto session =
        makeSession();

    std::uint64_t nextSequence{1};

    std::uint64_t messagesRead{0};

    std::uint64_t itchPayloadBytes{0};

    std::uint64_t encodedBytes{0};

    std::uint64_t minMessagesPerDatagram{0};

    std::uint64_t maxMessagesPerDatagram{0};

    PrebuiltMoldDatagram
        current{};

    bool packetOpen{false};

    bool finishedReading{false};

    auto beginCurrentPacket =
        [&](std::uint64_t firstSequence)
    {
        current =
            PrebuiltMoldDatagram{};

        current.firstSequence =
            firstSequence;

        current.messageCount =
            0;

        if (
            !llt::moldudp64::MoldUdp64Codec::
                beginPacket(
                    session,
                    firstSequence,
                    current.bytes,
                    current.size))
        {
            return false;
        }

        packetOpen =
            true;

        return true;
    };

    auto finalizeCurrentPacket =
        [&]()
    {
        if (
            !packetOpen ||
            current.messageCount == 0)
        {
            return;
        }

        llt::moldudp64::MoldUdp64Codec::
            finalizePacket(
                current.bytes,
                current.messageCount);

        encodedBytes +=
            static_cast<std::uint64_t>(
                current.size);

        if (
            minMessagesPerDatagram == 0 ||
            current.messageCount <
                minMessagesPerDatagram)
        {
            minMessagesPerDatagram =
                current.messageCount;
        }

        if (
            current.messageCount >
            maxMessagesPerDatagram)
        {
            maxMessagesPerDatagram =
                current.messageCount;
        }

        datagrams.emplace_back(
            std::move(
                current));

        current =
            PrebuiltMoldDatagram{};

        packetOpen =
            false;
    };

    while (
        messagesRead <
            requestedMessages &&
        !finishedReading)
    {
        auto record =
            reader.readNext();

        switch (record.status)
        {
        case llt::itch::ItchStreamReadStatus::Message:
        {
            if (record.empty())
            {
                std::cerr
                    << "Encountered empty ITCH payload "
                    << "at BinaryFILE message "
                    << (messagesRead + 1)
                    << ".\n";

                return 1;
            }

            //
            // Mold uses a 16-bit message length prefix.
            //
            // In practice ITCH messages are far smaller,
            // but validate the actual framing constraint.
            //
            if (
                record.size >
                0xFFFFu)
            {
                std::cerr
                    << "ITCH payload exceeds MoldUDP64 "
                    << "16-bit message length at message "
                    << (messagesRead + 1)
                    << ". Payload size: "
                    << record.size
                    << '\n';

                return 1;
            }

            //
            // A message must also be capable of fitting
            // inside one of OUR configured datagrams:
            //
            // Mold header
            // + 2-byte message length
            // + message bytes
            //
            if (
                llt::moldudp64::HeaderSize +
                    sizeof(std::uint16_t) +
                    record.size >
                llt::moldudp64::MaxDatagramSize)
            {
                std::cerr
                    << "ITCH payload cannot fit inside "
                    << "configured Mold datagram at message "
                    << (messagesRead + 1)
                    << ". Payload size: "
                    << record.size
                    << '\n';

                return 1;
            }

            if (!packetOpen)
            {
                if (
                    !beginCurrentPacket(
                        nextSequence))
                {
                    std::cerr
                        << "Failed beginning Mold packet "
                        << "at sequence "
                        << nextSequence
                        << ".\n";

                    return 1;
                }
            }

            //
            // Try to append the message to the current
            // Mold datagram.
            //
            if (
                !llt::moldudp64::MoldUdp64Codec::
                    appendMessage(
                        record.data,
                        record.size,
                        current.bytes,
                        current.size,
                        current.messageCount))
            {
                //
                // The current packet is full.
                //
                // IMPORTANT:
                //
                // Do NOT advance nextSequence.
                //
                // This ITCH message has NOT been appended
                // anywhere yet.
                //
                // Finalize the previous packet, begin a new
                // packet whose first sequence is THIS
                // message's sequence, then retry the SAME
                // record.
                //

                if (current.messageCount == 0)
                {
                    std::cerr
                        << "ITCH message could not fit into "
                        << "an empty Mold datagram at sequence "
                        << nextSequence
                        << ".\n";

                    return 1;
                }

                finalizeCurrentPacket();

                if (
                    !beginCurrentPacket(
                        nextSequence))
                {
                    std::cerr
                        << "Failed beginning Mold packet "
                        << "at sequence "
                        << nextSequence
                        << ".\n";

                    return 1;
                }

                if (
                    !llt::moldudp64::MoldUdp64Codec::
                        appendMessage(
                            record.data,
                            record.size,
                            current.bytes,
                            current.size,
                            current.messageCount))
                {
                    std::cerr
                        << "ITCH message still could not fit "
                        << "inside an empty Mold datagram "
                        << "at sequence "
                        << nextSequence
                        << ". Payload size: "
                        << record.size
                        << '\n';

                    return 1;
                }
            }

            //
            // Only NOW has this ITCH message been
            // successfully assigned to a Mold datagram.
            //
            ++messagesRead;

            itchPayloadBytes +=
                static_cast<std::uint64_t>(
                    record.size);

            ++nextSequence;

            if (
                messagesRead %
                    100000 ==
                0)
            {
                std::cout
                    << "Packed "
                    << messagesRead
                    << " messages into "
                    << datagrams.size()
                    << " completed datagrams"
                    << '\n';
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
                << messagesRead
                << " messages.\n";

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

    //
    // Flush the final partially filled Mold packet.
    //
    finalizeCurrentPacket();

    if (datagrams.empty())
    {
        std::cerr
            << "No MoldUDP64 datagrams were prepared.\n";

        return 1;
    }

    const auto preparedDatagrams =
        static_cast<std::uint64_t>(
            datagrams.size());

    const double averageMessagesPerDatagram =
        preparedDatagrams > 0
            ? static_cast<double>(
                  messagesRead) /
                  static_cast<double>(
                      preparedDatagrams)
            : 0.0;

    const double averageDatagramBytes =
        preparedDatagrams > 0
            ? static_cast<double>(
                  encodedBytes) /
                  static_cast<double>(
                      preparedDatagrams)
            : 0.0;

    std::cout
        << "----------------------------------------\n"
        << "Prebuild complete\n"
        << "ITCH messages      : "
        << messagesRead
        << '\n'
        << "Mold datagrams     : "
        << preparedDatagrams
        << '\n'
        << "ITCH payload bytes : "
        << itchPayloadBytes
        << '\n'
        << "Encoded Mold bytes : "
        << encodedBytes
        << '\n'
        << "First sequence     : 1\n"
        << "Last sequence      : "
        << (nextSequence - 1)
        << '\n'
        << std::fixed
        << std::setprecision(3)
        << "Avg msg/datagram   : "
        << averageMessagesPerDatagram
        << '\n'
        << "Min msg/datagram   : "
        << minMessagesPerDatagram
        << '\n'
        << "Max msg/datagram   : "
        << maxMessagesPerDatagram
        << '\n'
        << "Avg datagram bytes : "
        << averageDatagramBytes
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
    // Sender socket buffer
    // =====================================================
    //
    // Preserve the same requested sender-side buffer from
    // the existing connected-send benchmark.
    //

    constexpr int sendBufferBytes =
        8 * 1024 * 1024;

    if (
        ::setsockopt(
            socketFd,
            SOL_SOCKET,
            SO_SNDBUF,
            &sendBufferBytes,
            sizeof(sendBufferBytes)) < 0)
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
    // UDP remains datagram based.
    //
    // connect() only associates the socket with a fixed
    // peer, allowing the timed loop to use send().
    //

    if (
        ::connect(
            socketFd,
            reinterpret_cast<
                const sockaddr *>(
                &destination),
            sizeof(destination)) < 0)
    {
        std::cerr
            << "Failed to connect UDP socket.\n";

        ::close(
            socketFd);

        return 1;
    }

    //
    // =====================================================
    // Timed send
    // =====================================================
    //
    // Controlled mode paces by cumulative ITCH message count,
    // not datagram count, because Mold datagrams contain a
    // varying number of messages.
    //

    std::cout << "----------------------------------------\n";

    if (targetMessagesPerSecond == 0)
        std::cout << "Starting unthrottled connected Mold send...\n";
    else
        std::cout << "Starting controlled connected Mold send at "
                  << targetMessagesPerSecond << " ITCH messages/sec...\n";

    std::cout << "========================================\n";

    using Clock = std::chrono::steady_clock;

    std::uint64_t datagramsSent{0};
    std::uint64_t messagesSent{0};
    std::uint64_t sendErrors{0};
    std::uint64_t bytesSent{0};
    std::uint64_t scheduledMessages{0};

    const auto startTime = Clock::now();

    for (const auto &datagram : datagrams)
    {
        if (targetMessagesPerSecond != 0)
        {
            const auto targetNanoseconds =
                static_cast<std::uint64_t>(
                    (static_cast<long double>(scheduledMessages) *
                     1'000'000'000.0L) /
                    static_cast<long double>(targetMessagesPerSecond));

            const auto deadline =
                startTime + std::chrono::nanoseconds{targetNanoseconds};

            while (true)
            {
                const auto now = Clock::now();

                if (now >= deadline)
                    break;

                const auto remaining = deadline - now;

                if (remaining > std::chrono::microseconds{200})
                {
                    std::this_thread::sleep_for(
                        remaining - std::chrono::microseconds{100});
                }
                else
                {
                    std::this_thread::yield();
                }
            }
        }

        const auto sent =
            ::send(
                socketFd,
                datagram.bytes.data(),
                datagram.size,
                0);

        // Pace according to offered load even if send() fails.
        scheduledMessages +=
            static_cast<std::uint64_t>(datagram.messageCount);

        if (sent != static_cast<ssize_t>(datagram.size))
        {
            ++sendErrors;
            continue;
        }

        ++datagramsSent;
        messagesSent += static_cast<std::uint64_t>(datagram.messageCount);
        bytesSent += static_cast<std::uint64_t>(sent);
    }

    const auto endTime = Clock::now();

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

    const double datagramRate =
        seconds > 0.0
            ? static_cast<double>(
                  datagramsSent) /
                  seconds
            : 0.0;

    const double messageRate =
        seconds > 0.0
            ? static_cast<double>(
                  messagesSent) /
                  seconds
            : 0.0;

    const double offeredMessageRate =
        seconds > 0.0
            ? static_cast<double>(scheduledMessages) / seconds
            : 0.0;

    const double mibPerSecond =
        seconds > 0.0
            ? (
                  static_cast<double>(
                      bytesSent) /
                  (1024.0 * 1024.0)) /
                  seconds
            : 0.0;

    const double averageNanosecondsPerDatagram =
        datagramsSent > 0
            ? (
                  seconds *
                  1'000'000'000.0) /
                  static_cast<double>(
                      datagramsSent)
            : 0.0;

    const double averageNanosecondsPerMessage =
        messagesSent > 0
            ? (
                  seconds *
                  1'000'000'000.0) /
                  static_cast<double>(
                      messagesSent)
            : 0.0;

    const double sentAverageMessagesPerDatagram =
        datagramsSent > 0
            ? static_cast<double>(
                  messagesSent) /
                  static_cast<double>(
                      datagramsSent)
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
        << "          MOLDUDP64 RESULTS\n"
        << "========================================\n"
        << "Send API          : connected send()\n"
        << "Mode              : "
        << (targetMessagesPerSecond == 0 ? "UNTHROTTLED" : "CONTROLLED")
        << '\n'
        << "Target msg rate   : ";

    if (targetMessagesPerSecond == 0)
        std::cout << "MAXIMUM\n";
    else
        std::cout << targetMessagesPerSecond << " messages/sec\n";

    std::cout
        << "Messages prepared : " << messagesRead << '\n'
        << "Datagrams prepared: " << preparedDatagrams << '\n'
        << "Datagrams sent    : " << datagramsSent << '\n'
        << "Messages sent     : " << messagesSent << '\n'
        << "Send errors       : " << sendErrors << '\n'
        << "Bytes sent        : " << bytesSent << '\n'
        << "Elapsed           : " << seconds << " sec\n"
        << "Datagram rate     : " << datagramRate << " datagrams/sec\n"
        << "ITCH message rate : " << messageRate << " messages/sec\n"
        << "Offered msg rate  : " << offeredMessageRate << " messages/sec\n"
        << "Avg msg/datagram  : " << sentAverageMessagesPerDatagram << '\n'
        << "Avg send time     : " << averageNanosecondsPerDatagram << " ns/datagram\n"
        << "Effective msg time: " << averageNanosecondsPerMessage << " ns/message\n"
        << "Throughput        : " << mibPerSecond << " MiB/sec\n"
        << "========================================\n";

    return sendErrors == 0
               ? 0
               : 1;
}