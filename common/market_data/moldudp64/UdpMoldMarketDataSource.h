#pragma once

#include <cstdint>

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
        const UdpMoldMarketDataSource&) = delete;

    UdpMoldMarketDataSource&
    operator=(
        const UdpMoldMarketDataSource&) = delete;

    [[nodiscard]]
    bool receive(
        ReceivedMoldDatagram& datagram
    ) noexcept override;

private:
    int socketFd_{-1};
};

} // namespace llt::moldudp64