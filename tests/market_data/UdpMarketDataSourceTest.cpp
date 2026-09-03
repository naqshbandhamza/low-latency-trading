
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdint>
#include <cstring>

#include <catch2/catch_test_macros.hpp>

#include "market_data/UdpMarketDataPacket.h"
#include "market_data/UdpMarketDataSource.h"
#include "market_data/MarketDataMessage.h"


namespace
{

// struct UdpPacket
// {
//     std::uint8_t type;

//     std::uint64_t sequence;

//     std::uint64_t timestamp;

//     std::int64_t price;

//     std::uint64_t quantity;

//     std::int64_t bidPrice;

//     std::uint64_t bidQuantity;

//     std::int64_t askPrice;

//     std::uint64_t askQuantity;

//     std::uint8_t side;
// };

int createSender()
{
    return ::socket(
        AF_INET,
        SOCK_DGRAM,
        0
    );
}

void sendPacket(
    int socket,
    std::uint16_t port,
    const llt::UdpMarketDataPacket& packet
)
{
    sockaddr_in address{};

    address.sin_family =
        AF_INET;

    address.sin_addr.s_addr =
        htonl(INADDR_LOOPBACK);

    address.sin_port =
        htons(port);

    const auto result =
        ::sendto(
            socket,
            &packet,
            sizeof(packet),
            0,
            reinterpret_cast<
                const sockaddr*
            >(&address),
            sizeof(address)
        );

    REQUIRE(
        result
        == static_cast<
            ssize_t
        >(sizeof(packet))
    );
}

} // namespace

TEST_CASE(
    "UdpMarketDataSource receives Quote packet"
)
{
    constexpr std::uint16_t port =
        19001;

    llt::UdpMarketDataSource source(port);

    const int sender =
        createSender();

    REQUIRE(sender >= 0);

    //UdpPacket packet{};
    llt::UdpMarketDataPacket packet{};

    // packet.type =
    //     0;
    packet.type =
    llt::UdpMarketDataPacketType::Quote;

    packet.sequence =
        42;

    packet.timestamp =
        123456789;

    packet.bidPrice =
        234500;

    packet.bidQuantity =
        10;

    packet.askPrice =
        234510;

    packet.askQuantity =
        12;

    // packet.side =
    //     0;
    packet.side =
    llt::UdpMarketDataPacketSide::Buy;

    sendPacket(
        sender,
        port,
        packet
    );

    llt::MarketDataMessage message{};

    REQUIRE(
        source.receive(message)
    );

    REQUIRE(
        message.type
        == llt::MarketDataMessageType::Quote
    );

    REQUIRE(
        message.sequence
        == 42
    );

    REQUIRE(
        message.timestamp
        == 123456789
    );

    REQUIRE(
        message.bidPrice
        == 234500
    );

    REQUIRE(
        message.bidQuantity
        == 10
    );

    REQUIRE(
        message.askPrice
        == 234510
    );

    REQUIRE(
        message.askQuantity
        == 12
    );

    ::close(sender);
}

TEST_CASE(
    "UdpMarketDataSource receives Trade packet"
)
{
    constexpr std::uint16_t port =
        19002;

    llt::UdpMarketDataSource source(port);

    const int sender =
        createSender();

    REQUIRE(sender >= 0);

    // UdpPacket packet{};
    llt::UdpMarketDataPacket packet{};


    // packet.type =
    //     1;
    packet.type =
    llt::UdpMarketDataPacketType::Trade;

    packet.sequence =
        99;

    packet.timestamp =
        987654321;

    packet.price =
        234505;

    packet.quantity =
        3;

    // packet.side =
    //     0;
    packet.side =
    llt::UdpMarketDataPacketSide::Buy;

    sendPacket(
        sender,
        port,
        packet
    );

    llt::MarketDataMessage message{};

    REQUIRE(
        source.receive(message)
    );

    REQUIRE(
        message.type
        == llt::MarketDataMessageType::Trade
    );

    REQUIRE(
        message.sequence
        == 99
    );

    REQUIRE(
        message.timestamp
        == 987654321
    );

    REQUIRE(
        message.price
        == 234505
    );

    REQUIRE(
        message.quantity
        == 3
    );

    REQUIRE(
        message.side
        == llt::Side::Buy
    );

    ::close(sender);
}

