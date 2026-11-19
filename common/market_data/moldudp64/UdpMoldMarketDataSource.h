#pragma once

#include <cstdint>
#include <sys/socket.h> // provides iovec and mmsghdr on Linux

#include "market_data/moldudp64/IMoldMarketDataSource.h"

namespace llt::moldudp64
{

    class UdpMoldMarketDataSource final
        : public IMoldMarketDataSource
    {
    public:
        UdpMoldMarketDataSource(
            std::uint16_t port,
            std::uint32_t receiveTimeoutMs);

        ~UdpMoldMarketDataSource() override;

        UdpMoldMarketDataSource(
            const UdpMoldMarketDataSource &) = delete;

        UdpMoldMarketDataSource &
        operator=(
            const UdpMoldMarketDataSource &) = delete;

        [[nodiscard]]
        bool receive(
            ReceivedMoldDatagram &datagram) noexcept override;

        [[nodiscard]]
        int lastReceiveError() const noexcept
        {
            return lastReceiveError_;
        }

        [[nodiscard]]
        int actualReceiveBufferBytes() const noexcept
        {
            return actualReceiveBufferBytes_;
        }

        [[nodiscard]]
        std::uint64_t receiveErrorCount() const noexcept
        {
            return receiveErrorCount_;
        }

        [[nodiscard]]
        std::uint64_t truncatedDatagramCount() const noexcept
        {
            return truncatedDatagramCount_;
        }

    private:
        int socketFd_{-1};

        int lastReceiveError_{0};

        int actualReceiveBufferBytes_{0};

        std::uint64_t receiveErrorCount_{0};

        std::uint64_t truncatedDatagramCount_{0};

    private:
#if defined(__linux__)
        static constexpr std::size_t kRxBatchSize = 64;

        std::array<ReceivedMoldDatagram, kRxBatchSize> rxBatch_{};
        std::array<iovec, kRxBatchSize> rxIovecs_{};
        std::array<mmsghdr, kRxBatchSize> rxMsgs_{};
        std::size_t rxBatchCount_{0};
        std::size_t rxBatchIndex_{0};

#ifdef LLT_ENABLE_LATENCY_TRACE
        std::uint64_t rxBatchTimestampNs_{0};
#endif
#endif
    };

} // namespace llt::moldudp64