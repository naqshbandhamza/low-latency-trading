#include "market_data/moldudp64/UdpMoldMarketDataSource.h"

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <cstring>

#ifdef LLT_ENABLE_LATENCY_TRACE
#include "market_data/utility/LatencyClock.h"
#endif

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
                sizeof(receiveBufferBytes)) < 0)
        {
            ::close(
                socketFd_);

            socketFd_ = -1;

            throw std::runtime_error(
                "Failed to configure MoldUDP64 UDP receive buffer");
        }

        socklen_t optionLength =
            sizeof(actualReceiveBufferBytes_);

        if (
            ::getsockopt(
                socketFd_,
                SOL_SOCKET,
                SO_RCVBUF,
                &actualReceiveBufferBytes_,
                &optionLength) < 0)
        {
            const int savedError = errno;

            ::close(socketFd_);
            socketFd_ = -1;

            throw std::runtime_error(
                "Failed to read actual MoldUDP64 receive buffer: " +
                std::to_string(savedError));
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
                    const sockaddr *>(
                    &address),
                sizeof(address)) < 0)
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
                    1000) *
                1000);

        if (
            ::setsockopt(
                socketFd_,
                SOL_SOCKET,
                SO_RCVTIMEO,
                &timeout,
                sizeof(timeout)) < 0)
        {
            ::close(
                socketFd_);

            socketFd_ = -1;

            throw std::runtime_error(
                "Failed to configure MoldUDP64 UDP receive timeout");
        }

#if defined(__linux__)
        for (std::size_t i = 0; i < kRxBatchSize; ++i)
        {
            rxIovecs_[i].iov_base = rxBatch_[i].bytes.data();
            rxIovecs_[i].iov_len = rxBatch_[i].bytes.size();

            rxMsgs_[i].msg_hdr.msg_iov = &rxIovecs_[i];
            rxMsgs_[i].msg_hdr.msg_iovlen = 1;
        }
#endif
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

    // bool
    // UdpMoldMarketDataSource::receive(
    //     ReceivedMoldDatagram &datagram) noexcept
    // {
    //     datagram.size = 0;

    //     #ifdef LLT_ENABLE_LATENCY_TRACE
    //     datagram.receivedAtNs = 0;
    //     #endif

    //     while (true)
    //     {
    //         iovec buffer{};

    //         buffer.iov_base =
    //             datagram.bytes.data();

    //         buffer.iov_len =
    //             datagram.bytes.size();

    //         msghdr message{};

    //         message.msg_iov = &buffer;
    //         message.msg_iovlen = 1;

    //         const ssize_t receivedBytes =
    //             ::recvmsg(
    //                 socketFd_,
    //                 &message,
    //                 0);

    //         #ifdef LLT_ENABLE_LATENCY_TRACE
    //         const auto receiveCompletedNs =
    //             llt::latency::nowNs();
    //         #endif

    //         if (receivedBytes < 0)
    //         {
    //             const int error = errno;

    //             if (error == EINTR)
    //             {
    //                 continue;
    //             }

    //             if (
    //                 error == EAGAIN ||
    //                 error == EWOULDBLOCK)
    //             {
    //                 return false;
    //             }

    //             lastReceiveError_ = error;
    //             ++receiveErrorCount_;

    //             return false;
    //         }

    //         if (message.msg_flags & MSG_TRUNC)
    //         {
    //             ++truncatedDatagramCount_;

    //             return false;
    //         }

    //         if (receivedBytes == 0)
    //         {
    //             return false;
    //         }

    //         datagram.size =
    //             static_cast<std::size_t>(
    //                 receivedBytes);

    //         #ifdef LLT_ENABLE_LATENCY_TRACE
    //         datagram.receivedAtNs = receiveCompletedNs;
    //         #endif

    //         return true;
    //     }
    // }

    bool
    UdpMoldMarketDataSource::receive(
        ReceivedMoldDatagram &datagram) noexcept
    {
        datagram.size = 0;

#ifdef LLT_ENABLE_LATENCY_TRACE
        datagram.receivedAtNs = 0;
#endif

#if defined(__linux__)
        while (true)
        {
            while (rxBatchIndex_ < rxBatchCount_)
            {
                const std::size_t idx = rxBatchIndex_++;
                const auto &msg = rxMsgs_[idx];

                if (msg.msg_hdr.msg_flags & MSG_TRUNC)
                {
                    ++truncatedDatagramCount_;
                    continue;
                }

                if (msg.msg_len == 0)
                {
                    continue;
                }

                const auto receivedBytes =
                    static_cast<std::size_t>(msg.msg_len);

                std::memcpy(
                    datagram.bytes.data(),
                    rxBatch_[idx].bytes.data(),
                    receivedBytes);

                datagram.size = receivedBytes;

#ifdef LLT_ENABLE_LATENCY_TRACE
                datagram.receivedAtNs = rxBatchTimestampNs_;
#endif

                return true;
            }

            rxBatchIndex_ = 0;
            rxBatchCount_ = 0;

            for (std::size_t i = 0; i < kRxBatchSize; ++i)
            {
                rxMsgs_[i].msg_hdr.msg_flags = 0;
            }

            const int receivedCount =
                ::recvmmsg(
                    socketFd_,
                    rxMsgs_.data(),
                    static_cast<unsigned int>(kRxBatchSize),
                    MSG_DONTWAIT,
                    nullptr);

#ifdef LLT_ENABLE_LATENCY_TRACE
            rxBatchTimestampNs_ =
                llt::latency::nowNs();
#endif

            if (receivedCount < 0)
            {
                const int error = errno;

                if (error == EINTR)
                {
                    continue;
                }

                if (
                    error == EAGAIN ||
                    error == EWOULDBLOCK)
                {
                    return false;
                }

                lastReceiveError_ = error;
                ++receiveErrorCount_;

                return false;
            }

            if (receivedCount == 0)
            {
                return false;
            }

            rxBatchCount_ =
                static_cast<std::size_t>(receivedCount);
        }
#else
        while (true)
        {
            iovec buffer{};

            buffer.iov_base =
                datagram.bytes.data();

            buffer.iov_len =
                datagram.bytes.size();

            msghdr message{};

            message.msg_iov = &buffer;
            message.msg_iovlen = 1;

            const ssize_t receivedBytes =
                ::recvmsg(
                    socketFd_,
                    &message,
                    0);

#ifdef LLT_ENABLE_LATENCY_TRACE
            const auto receiveCompletedNs =
                llt::latency::nowNs();
#endif

            if (receivedBytes < 0)
            {
                const int error = errno;

                if (error == EINTR)
                {
                    continue;
                }

                if (
                    error == EAGAIN ||
                    error == EWOULDBLOCK)
                {
                    return false;
                }

                lastReceiveError_ = error;
                ++receiveErrorCount_;

                return false;
            }

            if (message.msg_flags & MSG_TRUNC)
            {
                ++truncatedDatagramCount_;

                return false;
            }

            if (receivedBytes == 0)
            {
                return false;
            }

            datagram.size =
                static_cast<std::size_t>(
                    receivedBytes);

#ifdef LLT_ENABLE_LATENCY_TRACE
            datagram.receivedAtNs = receiveCompletedNs;
#endif

            return true;
        }
#endif
    }

} // namespace llt::moldudp64