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
        llt::UdpMarketDataCodec::encode(
            packet
        );

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


bool sendQuote(
    int socket,
    const sockaddr_in& destination,
    const llt::UdpMarketDataPacket& packet
)
{
    if (
        !sendPacket(
            socket,
            destination,
            packet
        )
    )
    {
        std::cerr
            << "Failed to send QUOTE"
            << " seq="
            << packet.sequence
            << '\n';

        return false;
    }

    std::cout
        << "Sent QUOTE"
        << " seq="
        << packet.sequence
        << " bid="
        << packet.bidPrice
        << " bidQty="
        << packet.bidQuantity
        << " ask="
        << packet.askPrice
        << " askQty="
        << packet.askQuantity
        << '\n';

    return true;
}


bool sendTrade(
    int socket,
    const sockaddr_in& destination,
    const llt::UdpMarketDataPacket& packet
)
{
    if (
        !sendPacket(
            socket,
            destination,
            packet
        )
    )
    {
        std::cerr
            << "Failed to send TRADE"
            << " seq="
            << packet.sequence
            << '\n';

        return false;
    }

    std::cout
        << "Sent TRADE"
        << " seq="
        << packet.sequence
        << " price="
        << packet.price
        << " quantity="
        << packet.quantity
        << " side="
        << (
            packet.side
            == llt::UdpMarketDataPacketSide::Buy
                ? "BUY"
                : "SELL"
        )
        << '\n';

    return true;
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
        << "Sending UDP market data to "
        << "127.0.0.1:"
        << port
        << "\n\n";


    // =========================================================
    // Sequence 1
    //
    // Tight quote:
    //
    // bid = 10000
    // ask = 10010
    // spread = 10
    //
    // SimpleStrategy should generate:
    //
    // BUY @ 10010
    // =========================================================

    const auto quote1 =
        makeQuote(
            1,
            10000,
            10,
            10010,
            12
        );

    if (
        !sendQuote(
            socket,
            destination,
            quote1
        )
    )
    {
        ::close(socket);
        return 1;
    }


    std::this_thread::sleep_for(
        std::chrono::milliseconds(100)
    );


    // =========================================================
    // Sequence 2
    //
    // Trade event.
    //
    // SimpleStrategy currently ignores Trade events.
    // Therefore no OrderIntent should be generated.
    // =========================================================

    const auto trade =
        makeTrade(
            2,
            10005,
            5,
            llt::UdpMarketDataPacketSide::Buy
        );

    if (
        !sendTrade(
            socket,
            destination,
            trade
        )
    )
    {
        ::close(socket);
        return 1;
    }


    std::this_thread::sleep_for(
        std::chrono::milliseconds(100)
    );


    // =========================================================
    // Sequence 3
    //
    // Another tight quote:
    //
    // bid = 10002
    // ask = 10012
    // spread = 10
    //
    // SimpleStrategy should generate:
    //
    // BUY @ 10012
    // =========================================================

    const auto quote2 =
        makeQuote(
            3,
            10002,
            15,
            10012,
            20
        );

    if (
        !sendQuote(
            socket,
            destination,
            quote2
        )
    )
    {
        ::close(socket);
        return 1;
    }


    ::close(socket);


    std::cout
        << "\nMarket data simulation complete\n";

    return 0;
}