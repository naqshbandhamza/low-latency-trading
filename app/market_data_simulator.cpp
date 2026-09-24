#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

#include "market_data/UdpMarketDataCodec.h"
#include "market_data/UdpMarketDataPacket.h"

namespace
{

bool sendPacket(
    int socket,
    const sockaddr_in& destination,
    const llt::UdpMarketDataPacket& packet
)
{
    const auto buffer =
        llt::UdpMarketDataCodec::encode(packet);

    const auto sent =
        ::sendto(
            socket,
            buffer.data(),
            buffer.size(),
            0,
            reinterpret_cast<const sockaddr*>(
                &destination
            ),
            sizeof(destination)
        );

    return sent ==
        static_cast<ssize_t>(
            buffer.size()
        );
}

llt::UdpMarketDataPacket makeQuote(
    std::uint64_t sequence,
    std::int64_t bidPrice,
    std::uint64_t bidQuantity,
    std::int64_t askPrice,
    std::uint64_t askQuantity
)
{
    llt::UdpMarketDataPacket packet{};

    packet.type =
        llt::UdpMarketDataPacketType::Quote;

    packet.sequence =
        sequence;

    packet.timestamp =
        sequence * 1000;

    packet.bidPrice =
        bidPrice;

    packet.bidQuantity =
        bidQuantity;

    packet.askPrice =
        askPrice;

    packet.askQuantity =
        askQuantity;

    packet.side =
        llt::UdpMarketDataPacketSide::Buy;

    return packet;
}

llt::UdpMarketDataPacket makeTrade(
    std::uint64_t sequence,
    std::int64_t price,
    std::uint64_t quantity,
    llt::UdpMarketDataPacketSide side
)
{
    llt::UdpMarketDataPacket packet{};

    packet.type =
        llt::UdpMarketDataPacketType::Trade;

    packet.sequence =
        sequence;

    packet.timestamp =
        sequence * 1000;

    packet.price =
        price;

    packet.quantity =
        quantity;

    packet.side =
        side;

    return packet;
}

} // namespace

int main()
{
    constexpr std::uint16_t port =
        19000;

    const int socket =
        ::socket(
            AF_INET,
            SOCK_DGRAM,
            0
        );

    if (socket < 0)
    {
        std::cerr
            << "Failed to create UDP socket\n";

        return 1;
    }

    sockaddr_in destination{};

    destination.sin_family =
        AF_INET;

    destination.sin_port =
        htons(port);

    destination.sin_addr.s_addr =
        htonl(INADDR_LOOPBACK);

    std::cout
        << "Market data simulator starting\n"
        << "Sending UDP market data to 127.0.0.1:"
        << port
        << "\n\n";

    // ---------------------------------------------------------
    // Sequence 1 - Quote
    // ---------------------------------------------------------

    const auto quote1 =
        makeQuote(
            1,
            10000,
            10,
            10010,
            12
        );

    if (!sendPacket(
            socket,
            destination,
            quote1
        ))
    {
        std::cerr
            << "Failed to send sequence 1\n";

        ::close(socket);
        return 1;
    }

    std::cout
        << "Sent QUOTE"
        << " seq=1"
        << " bid=10000"
        << " ask=10010"
        << '\n';

    std::this_thread::sleep_for(
        std::chrono::milliseconds(100)
    );

    // ---------------------------------------------------------
    // Sequence 2 - Trade
    // ---------------------------------------------------------

    const auto trade =
        makeTrade(
            2,
            10005,
            5,
            llt::UdpMarketDataPacketSide::Buy
        );

    if (!sendPacket(
            socket,
            destination,
            trade
        ))
    {
        std::cerr
            << "Failed to send sequence 2\n";

        ::close(socket);
        return 1;
    }

    std::cout
        << "Sent TRADE"
        << " seq=2"
        << " price=10005"
        << " quantity=5"
        << '\n';

    std::this_thread::sleep_for(
        std::chrono::milliseconds(100)
    );

    // ---------------------------------------------------------
    // Sequence 3 - Quote
    // ---------------------------------------------------------

    const auto quote2 =
    makeQuote(
        3,
        10002,
        15,
        10012,
        20
    );

    if (!sendPacket(
            socket,
            destination,
            quote2
        ))
    {
        std::cerr
            << "Failed to send sequence 3\n";

        ::close(socket);
        return 1;
    }

    std::cout
    << "Sent QUOTE"
    << " seq=4"
    << " bid=10002"
    << " ask=10012"
    << '\n';

    ::close(socket);

    std::cout
        << "\nMarket data simulation complete\n";

    return 0;
}