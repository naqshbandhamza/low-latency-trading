#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <thread>

#include "market_data/itch/ItchUdpCodec.h"
#include "market_data/itch/ItchUdpPacket.h"

namespace
{

void writeU16(
    std::uint8_t* destination,
    std::uint16_t value) noexcept
{
    destination[0] =
        static_cast<std::uint8_t>(
            (value >> 8) & 0xFF);

    destination[1] =
        static_cast<std::uint8_t>(
            value & 0xFF);
}


void writeU32(
    std::uint8_t* destination,
    std::uint32_t value) noexcept
{
    destination[0] =
        static_cast<std::uint8_t>(
            (value >> 24) & 0xFF);

    destination[1] =
        static_cast<std::uint8_t>(
            (value >> 16) & 0xFF);

    destination[2] =
        static_cast<std::uint8_t>(
            (value >> 8) & 0xFF);

    destination[3] =
        static_cast<std::uint8_t>(
            value & 0xFF);
}


void writeU48(
    std::uint8_t* destination,
    std::uint64_t value) noexcept
{
    destination[0] =
        static_cast<std::uint8_t>(
            (value >> 40) & 0xFF);

    destination[1] =
        static_cast<std::uint8_t>(
            (value >> 32) & 0xFF);

    destination[2] =
        static_cast<std::uint8_t>(
            (value >> 24) & 0xFF);

    destination[3] =
        static_cast<std::uint8_t>(
            (value >> 16) & 0xFF);

    destination[4] =
        static_cast<std::uint8_t>(
            (value >> 8) & 0xFF);

    destination[5] =
        static_cast<std::uint8_t>(
            value & 0xFF);
}


void writeU64(
    std::uint8_t* destination,
    std::uint64_t value) noexcept
{
    for (
        std::size_t i = 0;
        i < 8;
        ++i)
    {
        const auto shift =
            static_cast<unsigned>(
                (7 - i) * 8);

        destination[i] =
            static_cast<std::uint8_t>(
                (value >> shift) & 0xFF);
    }
}


void writeSymbol(
    std::uint8_t* destination,
    const char* symbol) noexcept
{
    for (
        std::size_t i = 0;
        i < 8;
        ++i)
    {
        destination[i] =
            static_cast<std::uint8_t>(' ');
    }

    for (
        std::size_t i = 0;
        i < 8 &&
        symbol[i] != '\0';
        ++i)
    {
        destination[i] =
            static_cast<std::uint8_t>(
                symbol[i]);
    }
}


// =========================================================
// Stock Directory
// =========================================================

llt::itch::ItchUdpPacket
makeStockDirectory(
    std::uint64_t sequence,
    std::uint16_t stockLocate,
    std::uint64_t timestamp,
    const char* symbol)
{
    llt::itch::ItchUdpPacket packet{};

    packet.sequence =
        sequence;

    packet.payloadSize =
        39;

    auto* p =
        packet.payload.data();

    p[0] = 'R';

    writeU16(
        p + 1,
        stockLocate);

    writeU16(
        p + 3,
        0);

    writeU48(
        p + 5,
        timestamp);

    writeSymbol(
        p + 11,
        symbol);

    //
    // Remaining ITCH Stock Directory fields.
    //
    p[19] = 'Q'; // market category
    p[20] = 'N'; // financial status

    writeU32(
        p + 21,
        100);

    p[25] = 'N'; // round lots only
    p[26] = 'A'; // issue classification

    p[27] = ' ';
    p[28] = ' ';

    p[29] = 'P'; // authenticity
    p[30] = 'N'; // short sale threshold
    p[31] = 'N'; // IPO flag
    p[32] = '1'; // LULD tier
    p[33] = 'N'; // ETP flag

    writeU32(
        p + 34,
        1);

    p[38] = 'N'; // inverse indicator

    return packet;
}


// =========================================================
// Add Order
// =========================================================

llt::itch::ItchUdpPacket
makeAddOrder(
    std::uint64_t sequence,
    std::uint16_t stockLocate,
    std::uint64_t timestamp,
    std::uint64_t orderId,
    char side,
    std::uint32_t shares,
    std::uint32_t price,
    const char* symbol)
{
    llt::itch::ItchUdpPacket packet{};

    packet.sequence =
        sequence;

    packet.payloadSize =
        36;

    auto* p =
        packet.payload.data();

    p[0] = 'A';

    writeU16(
        p + 1,
        stockLocate);

    writeU16(
        p + 3,
        0);

    writeU48(
        p + 5,
        timestamp);

    writeU64(
        p + 11,
        orderId);

    p[19] =
        static_cast<std::uint8_t>(
            side);

    writeU32(
        p + 20,
        shares);

    writeSymbol(
        p + 24,
        symbol);

    writeU32(
        p + 32,
        price);

    return packet;
}


// =========================================================
// Delete Order
// =========================================================

llt::itch::ItchUdpPacket
makeDeleteOrder(
    std::uint64_t sequence,
    std::uint16_t stockLocate,
    std::uint64_t timestamp,
    std::uint64_t orderId)
{
    llt::itch::ItchUdpPacket packet{};

    packet.sequence =
        sequence;

    packet.payloadSize =
        19;

    auto* p =
        packet.payload.data();

    p[0] = 'D';

    writeU16(
        p + 1,
        stockLocate);

    writeU16(
        p + 3,
        0);

    writeU48(
        p + 5,
        timestamp);

    writeU64(
        p + 11,
        orderId);

    return packet;
}


// =========================================================
// UDP transmission
// =========================================================

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
            reinterpret_cast<
                const sockaddr*>(
                    &destination),
            sizeof(destination));

    return
        sent ==
        static_cast<ssize_t>(
            encodedSize);
}


void printUsage(
    const char* executable)
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
    char* argv[])
{
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

    constexpr std::uint16_t
        stockLocate = 42;

    //
    // Synthetic market:
    //
    // seq 100 : directory AAPL
    //
    // seq 101 : bid
    //           100 @ 100.0000
    //
    // seq 102 : ask
    //           150 @ 101.0000
    //
    // seq 103 : improved bid
    //           200 @ 100.5000
    //
    // seq 104 : delete improved bid
    //
    // Final BBO:
    //
    // 100 @ 100.0000
    // 150 @ 101.0000
    //

    const auto directory =
        makeStockDirectory(
            100,
            stockLocate,
            1000,
            "AAPL");

    const auto bid100 =
        makeAddOrder(
            101,
            stockLocate,
            1001,
            1,
            'B',
            100,
            1000000,
            "AAPL");

    const auto ask101 =
        makeAddOrder(
            102,
            stockLocate,
            1002,
            2,
            'S',
            150,
            1010000,
            "AAPL");

    const auto bid100_50 =
        makeAddOrder(
            103,
            stockLocate,
            1003,
            3,
            'B',
            200,
            1005000,
            "AAPL");

    const auto deleteImprovedBid =
        makeDeleteOrder(
            104,
            stockLocate,
            1004,
            3);

    std::cout
        << "========================================\n"
        << "       SYNTHETIC ITCH UDP SENDER\n"
        << "========================================\n"
        << "Destination : 127.0.0.1:"
        << port
        << '\n'
        << "Instrument  : AAPL\n"
        << "Packets     : 5\n"
        << "Sequences   : 100 - 104\n"
        << "----------------------------------------\n";

    const auto send =
        [&](const llt::itch::ItchUdpPacket& packet,
            const char* description)
        {
            if (
                !sendPacket(
                    socketFd,
                    destination,
                    packet))
            {
                std::cerr
                    << "Failed sending sequence "
                    << packet.sequence
                    << '\n';

                return false;
            }

            std::cout
                << "Sent "
                << packet.sequence
                << " | "
                << description
                << '\n';

            //
            // Human-readable pacing only.
            //
            // This is NOT intended as a benchmark.
            //
            std::this_thread::sleep_for(
                std::chrono::milliseconds{
                    100});

            return true;
        };

    bool success = true;

    success =
        success &&
        send(
            directory,
            "R AAPL directory");

    success =
        success &&
        send(
            bid100,
            "A BID 100 @ 100.0000");

    success =
        success &&
        send(
            ask101,
            "A ASK 150 @ 101.0000");

    success =
        success &&
        send(
            bid100_50,
            "A BID 200 @ 100.5000");

    success =
        success &&
        send(
            deleteImprovedBid,
            "D delete improved bid");

    ::close(
        socketFd);

    if (!success)
    {
        return 1;
    }

    std::cout
        << "----------------------------------------\n"
        << "Synthetic stream complete.\n"
        << "========================================\n";

    return 0;
}