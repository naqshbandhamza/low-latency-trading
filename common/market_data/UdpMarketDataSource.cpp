#include "market_data/UdpMarketDataSource.h"
#include "market_data/UdpMarketDataPacket.h"
#include "market_data/MarketDataMessage.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

namespace llt
{

UdpMarketDataSource::UdpMarketDataSource(
    std::uint16_t port
)
{
    socket_ =
        ::socket(
            AF_INET,
            SOCK_DGRAM,
            0
        );

    if (socket_ < 0)
    {
        return;
    }

    sockaddr_in address{};

    address.sin_family =
        AF_INET;

    address.sin_addr.s_addr =
        htonl(INADDR_LOOPBACK);

    address.sin_port =
        htons(port);

    if (
        ::bind(
            socket_,
            reinterpret_cast<
                const sockaddr*
            >(&address),
            sizeof(address)
        ) < 0
    )
    {
        ::close(socket_);

        socket_ = -1;
    }
}

UdpMarketDataSource::~UdpMarketDataSource()
{
    if (socket_ >= 0)
    {
        ::close(socket_);
    }
}

bool UdpMarketDataSource::receive(
    MarketDataMessage& message
) noexcept
{
    if (socket_ < 0)
    {
        return false;
    }

    UdpMarketDataPacket packet{};

    const auto received =
        ::recvfrom(
            socket_,
            &packet,
            sizeof(packet),
            0,
            nullptr,
            nullptr
        );

    if (
        received
        != static_cast<ssize_t>(
            sizeof(packet)
        )
    )
    {
        return false;
    }

    message.type =
        packet.type
        == UdpMarketDataPacketType::Quote
            ? MarketDataMessageType::Quote
            : MarketDataMessageType::Trade;

    message.sequence =
        packet.sequence;

    message.timestamp =
        packet.timestamp;

    message.price =
        packet.price;

    message.quantity =
        packet.quantity;

    message.bidPrice =
        packet.bidPrice;

    message.bidQuantity =
        packet.bidQuantity;

    message.askPrice =
        packet.askPrice;

    message.askQuantity =
        packet.askQuantity;

    message.side =
        packet.side
        == UdpMarketDataPacketSide::Buy
            ? Side::Buy
            : Side::Sell;

    return true;
}

} // namespace llt