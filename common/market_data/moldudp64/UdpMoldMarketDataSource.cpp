#include "market_data/moldudp64/UdpMoldMarketDataSource.h"

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

namespace llt::moldudp64
{

UdpMoldMarketDataSource::
UdpMoldMarketDataSource(
    std::uint16_t port,
    std::uint32_t receiveTimeoutMs)
{
    socketFd_ =
        ::socket(
            AF_INET,
            SOCK_DGRAM,
            0);

    if (socketFd_ < 0)
    {
        throw std::runtime_error(
            "Failed to create MoldUDP64 UDP socket");
    }


    //
    // Increase kernel receive buffering.
    //
    // This was important in our previous UDP experiment:
    // the larger receive buffer eliminated recovery/gaps
    // at the sender rate we were testing.
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
        ::close(
            socketFd_);

        socketFd_ = -1;

        throw std::runtime_error(
            "Failed to configure MoldUDP64 UDP receive buffer");
    }


    //
    // Bind to loopback for our current local replay/
    // benchmark environment.
    //
    sockaddr_in address{};

    address.sin_family =
        AF_INET;

    address.sin_addr.s_addr =
        htonl(
            INADDR_LOOPBACK);

    address.sin_port =
        htons(
            port);


    if (
        ::bind(
            socketFd_,
            reinterpret_cast<
                const sockaddr*>(
                    &address),
            sizeof(address))
        < 0)
    {
        ::close(
            socketFd_);

        socketFd_ = -1;

        throw std::runtime_error(
            "Failed to bind MoldUDP64 UDP socket");
    }


    //
    // Receive timeout.
    //
    timeval timeout{};

    timeout.tv_sec =
        static_cast<time_t>(
            receiveTimeoutMs /
            1000);

    timeout.tv_usec =
        static_cast<suseconds_t>(
            (
                receiveTimeoutMs %
                1000
            ) *
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
        ::close(
            socketFd_);

        socketFd_ = -1;

        throw std::runtime_error(
            "Failed to configure MoldUDP64 UDP receive timeout");
    }
}


UdpMoldMarketDataSource::
~UdpMoldMarketDataSource()
{
    if (socketFd_ >= 0)
    {
        ::close(
            socketFd_);
    }
}


bool
UdpMoldMarketDataSource::receive(
    ReceivedMoldDatagram& datagram
) noexcept
{
    ssize_t receivedBytes{0};


    while (true)
    {
        receivedBytes =
            ::recvfrom(
                socketFd_,
                datagram.bytes.data(),
                datagram.bytes.size(),
                0,
                nullptr,
                nullptr);


        if (receivedBytes >= 0)
        {
            break;
        }


        if (errno == EINTR)
        {
            continue;
        }


        if (
            errno == EAGAIN ||
            errno == EWOULDBLOCK)
        {
            datagram.size = 0;

            return false;
        }


        datagram.size = 0;

        return false;
    }


    if (receivedBytes == 0)
    {
        datagram.size = 0;

        return false;
    }


    datagram.size =
        static_cast<std::size_t>(
            receivedBytes);


    return true;
}

} // namespace llt::moldudp64