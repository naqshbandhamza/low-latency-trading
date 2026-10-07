// #include "market_data/itch/UdpItchMarketDataSource.h"

// #include <array>
// #include <cerrno>
// #include <cstddef>
// #include <cstdint>
// #include <stdexcept>

// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <sys/time.h>
// #include <unistd.h>

// #include "market_data/itch/ItchUdpCodec.h"

// namespace llt::itch
// {

// UdpItchMarketDataSource::
// UdpItchMarketDataSource(
//     std::uint16_t port,
//     std::uint32_t receiveTimeoutMs
// )
// {
//     socketFd_ =
//         ::socket(
//             AF_INET,
//             SOCK_DGRAM,
//             0);

//     if (socketFd_ < 0)
//     {
//         throw std::runtime_error(
//             "Failed to create ITCH UDP socket");
//     }

//     sockaddr_in address{};

//     address.sin_family =
//         AF_INET;

//     address.sin_addr.s_addr =
//         htonl(INADDR_LOOPBACK);

//     address.sin_port =
//         htons(port);

//     if (
//         ::bind(
//             socketFd_,
//             reinterpret_cast<
//                 const sockaddr*>(
//                     &address),
//             sizeof(address))
//         < 0)
//     {
//         ::close(socketFd_);

//         socketFd_ = -1;

//         throw std::runtime_error(
//             "Failed to bind ITCH UDP socket");
//     }

//     timeval timeout{};

//     timeout.tv_sec =
//         static_cast<time_t>(
//             receiveTimeoutMs / 1000);

//     timeout.tv_usec =
//         static_cast<suseconds_t>(
//             (receiveTimeoutMs % 1000) *
//             1000);

//     if (
//         ::setsockopt(
//             socketFd_,
//             SOL_SOCKET,
//             SO_RCVTIMEO,
//             &timeout,
//             sizeof(timeout))
//         < 0)
//     {
//         ::close(socketFd_);

//         socketFd_ = -1;

//         throw std::runtime_error(
//             "Failed to configure ITCH UDP receive timeout");
//     }
// }


// UdpItchMarketDataSource::
// ~UdpItchMarketDataSource()
// {
//     if (socketFd_ >= 0)
//     {
//         ::close(socketFd_);
//     }
// }


// bool UdpItchMarketDataSource::receive(
//     ItchUdpPacket& packet
// ) noexcept
// {
//     std::array<
//         std::uint8_t,
//         ItchUdpCodec::MaxDatagramSize
//     > buffer{};

//     ssize_t receivedBytes{0};

//     while (true)
//     {
//         receivedBytes =
//             ::recvfrom(
//                 socketFd_,
//                 buffer.data(),
//                 buffer.size(),
//                 0,
//                 nullptr,
//                 nullptr);

//         if (receivedBytes >= 0)
//         {
//             break;
//         }

//         if (errno == EINTR)
//         {
//             continue;
//         }

//         if (
//             errno == EAGAIN ||
//             errno == EWOULDBLOCK)
//         {
//             return false;
//         }

//         return false;
//     }

//     if (receivedBytes == 0)
//     {
//         return false;
//     }

//     return ItchUdpCodec::decode(
//         buffer.data(),
//         static_cast<std::size_t>(
//             receivedBytes),
//         packet);
// }

// } // namespace llt::itch




#include "market_data/itch/UdpItchMarketDataSource.h"

#include <array>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include "market_data/itch/ItchUdpCodec.h"

namespace llt::itch
{

UdpItchMarketDataSource::
UdpItchMarketDataSource(
    std::uint16_t port,
    std::uint32_t receiveTimeoutMs
)
{
    //
    // =====================================================
    // Create UDP socket
    // =====================================================
    //

    socketFd_ =
        ::socket(
            AF_INET,
            SOCK_DGRAM,
            0);

    if (socketFd_ < 0)
    {
        throw std::runtime_error(
            "Failed to create ITCH UDP socket");
    }

    //
    // =====================================================
    // Increase kernel receive buffer
    // =====================================================
    //
    // Market-data traffic can arrive in bursts.
    //
    // A larger socket receive buffer gives the kernel more
    // room to queue incoming UDP datagrams while the feed
    // thread is processing previous packets.
    //
    // This does NOT make recvfrom() itself faster.
    // It primarily reduces packet loss during bursts.
    //

    constexpr int receiveBufferBytes =
        8 * 1024 * 1024;

    if (
        ::setsockopt(
            socketFd_,
            SOL_SOCKET,
            SO_RCVBUF,
            &receiveBufferBytes,
            sizeof(receiveBufferBytes))
        < 0)
    {
        ::close(socketFd_);

        socketFd_ = -1;

        throw std::runtime_error(
            "Failed to configure ITCH UDP receive buffer");
    }

    //
    // =====================================================
    // Bind socket
    // =====================================================
    //

    sockaddr_in address{};

    address.sin_family =
        AF_INET;

    address.sin_addr.s_addr =
        htonl(INADDR_LOOPBACK);

    address.sin_port =
        htons(port);

    if (
        ::bind(
            socketFd_,
            reinterpret_cast<
                const sockaddr*>(
                    &address),
            sizeof(address))
        < 0)
    {
        ::close(socketFd_);

        socketFd_ = -1;

        throw std::runtime_error(
            "Failed to bind ITCH UDP socket");
    }

    //
    // =====================================================
    // Receive timeout
    // =====================================================
    //

    timeval timeout{};

    timeout.tv_sec =
        static_cast<time_t>(
            receiveTimeoutMs / 1000);

    timeout.tv_usec =
        static_cast<suseconds_t>(
            (receiveTimeoutMs % 1000) *
            1000);

    if (
        ::setsockopt(
            socketFd_,
            SOL_SOCKET,
            SO_RCVTIMEO,
            &timeout,
            sizeof(timeout))
        < 0)
    {
        ::close(socketFd_);

        socketFd_ = -1;

        throw std::runtime_error(
            "Failed to configure ITCH UDP receive timeout");
    }
}


UdpItchMarketDataSource::
~UdpItchMarketDataSource()
{
    if (socketFd_ >= 0)
    {
        ::close(socketFd_);
    }
}


bool UdpItchMarketDataSource::receive(
    ItchUdpPacket& packet
) noexcept
{
    std::array<
        std::uint8_t,
        ItchUdpCodec::MaxDatagramSize
    > buffer{};

    ssize_t receivedBytes{0};

    while (true)
    {
        receivedBytes =
            ::recvfrom(
                socketFd_,
                buffer.data(),
                buffer.size(),
                0,
                nullptr,
                nullptr);

        if (receivedBytes >= 0)
        {
            break;
        }

        //
        // Interrupted by a signal.
        // Retry recvfrom().
        //
        if (errno == EINTR)
        {
            continue;
        }

        //
        // SO_RCVTIMEO causes recvfrom() to return
        // EAGAIN/EWOULDBLOCK when the timeout expires.
        //
        if (
            errno == EAGAIN ||
            errno == EWOULDBLOCK)
        {
            return false;
        }

        //
        // Other socket error.
        //
        return false;
    }

    if (receivedBytes == 0)
    {
        return false;
    }

    return ItchUdpCodec::decode(
        buffer.data(),
        static_cast<std::size_t>(
            receivedBytes),
        packet);
}

} // namespace llt::itch