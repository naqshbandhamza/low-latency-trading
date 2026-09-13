#include "market_data/UdpMarketDataSource.h"
#include "market_data/UdpMarketDataPacket.h"
#include "market_data/MarketDataMessage.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <sys/time.h>
#include "market_data/UdpMarketDataCodec.h"

namespace llt
{

    UdpMarketDataSource::UdpMarketDataSource(
        std::uint16_t port,
        std::uint32_t receiveTimeoutMs)
    {
        socket_ =
            ::socket(
                AF_INET,
                SOCK_DGRAM,
                0);

        if (socket_ < 0)
        {
            return;
        }

        timeval timeout{};

        timeout.tv_sec =
            static_cast<time_t>(
                receiveTimeoutMs / 1000);

        timeout.tv_usec =
            static_cast<suseconds_t>(
                (receiveTimeoutMs % 1000) * 1000);

        if (
            ::setsockopt(
                socket_,
                SOL_SOCKET,
                SO_RCVTIMEO,
                &timeout,
                sizeof(timeout)) < 0)
        {
            ::close(socket_);

            socket_ = -1;

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
                    const sockaddr *>(&address),
                sizeof(address)) < 0)
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

    // bool UdpMarketDataSource::receive(
    //     MarketDataMessage& message
    // ) noexcept
    // {
    //     if (socket_ < 0)
    //     {
    //         return false;
    //     }

    //     // UdpMarketDataPacket packet{};

    //     // const auto received =
    //     //     ::recvfrom(
    //     //         socket_,
    //     //         &packet,
    //     //         sizeof(packet),
    //     //         0,
    //     //         nullptr,
    //     //         nullptr
    //     //     );

    //     UdpMarketDataCodec::Buffer buffer{};

    //     const auto received =
    //         ::recvfrom(
    //             socket_,
    //             buffer.data(),
    //             buffer.size(),
    //             0,
    //             nullptr,
    //             nullptr
    //         );

    //     if (
    //         received
    //         != static_cast<ssize_t>(
    //             buffer.size()
    //         )
    //     )
    //     {
    //         return false;
    //     }

    //     UdpMarketDataPacket packet{};

    //     if (
    //         !UdpMarketDataCodec::decode(
    //             buffer.data(),
    //             buffer.size(),
    //             packet
    //         )
    //     )
    //     {
    //         return false;
    //     }

    //     // if (
    //     //     received
    //     //     != static_cast<ssize_t>(
    //     //         sizeof(packet)
    //     //     )
    //     // )
    //     // {
    //     //     return false;
    //     // }

    //     message.type =
    //         packet.type
    //         == UdpMarketDataPacketType::Quote
    //             ? MarketDataMessageType::Quote
    //             : MarketDataMessageType::Trade;

    //     message.sequence =
    //         packet.sequence;

    //     message.timestamp =
    //         packet.timestamp;

    //     message.price =
    //         packet.price;

    //     message.quantity =
    //         packet.quantity;

    //     message.bidPrice =
    //         packet.bidPrice;

    //     message.bidQuantity =
    //         packet.bidQuantity;

    //     message.askPrice =
    //         packet.askPrice;

    //     message.askQuantity =
    //         packet.askQuantity;

    //     message.side =
    //         packet.side
    //         == UdpMarketDataPacketSide::Buy
    //             ? Side::Buy
    //             : Side::Sell;

    //     return true;
    // }

    bool UdpMarketDataSource::receive(
        MarketDataMessage& message
    ) noexcept
    {
        if (socket_ < 0)
        {
            return false;
        }
    
        //UdpMarketDataCodec::Buffer buffer{};
        std::array<
            std::uint8_t,
            UdpMarketDataCodec::WireSize + 1
        > buffer{};
    
        ssize_t received = 0;
    
        while (true)
        {
            received =
                ::recvfrom(
                    socket_,
                    buffer.data(),
                    buffer.size(),
                    0,
                    nullptr,
                    nullptr
                );
    
            // The system call was interrupted by a signal.
            // Retry the receive operation.
            if (
                received < 0
                && errno == EINTR
            )
            {
                continue;
            }
    
            break;
        }
    
        if (received < 0)
        {
            // SO_RCVTIMEO reports a receive timeout as
            // EAGAIN/EWOULDBLOCK.
            if (
                errno == EAGAIN
                || errno == EWOULDBLOCK
            )
            {
                return false;
            }
    
            // Any other recvfrom() failure is a socket error.
            return false;
        }
    
        // A valid market-data datagram must contain
        // exactly the codec wire size.
        if (
            received
            != static_cast<ssize_t>(
                UdpMarketDataCodec::WireSize
            )
        )
        {
            return false;
        }
    
        UdpMarketDataPacket packet{};
    
        if (
            !UdpMarketDataCodec::decode(
                buffer.data(),
                UdpMarketDataCodec::WireSize,
                packet
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