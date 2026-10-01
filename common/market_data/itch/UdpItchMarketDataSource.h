#pragma once

#include <cstdint>

#include "market_data/itch/IItchMarketDataSource.h"
#include "market_data/itch/ItchUdpPacket.h"

namespace llt::itch
{

class UdpItchMarketDataSource final
    : public IItchMarketDataSource
{
public:
    explicit UdpItchMarketDataSource(
        std::uint16_t port,
        std::uint32_t receiveTimeoutMs = 1000
    );

    ~UdpItchMarketDataSource() override;

    UdpItchMarketDataSource(
        const UdpItchMarketDataSource&
    ) = delete;

    UdpItchMarketDataSource&
    operator=(
        const UdpItchMarketDataSource&
    ) = delete;

    [[nodiscard]]
    bool receive(
        ItchUdpPacket& packet
    ) noexcept override;

private:
    int socketFd_{-1};
};

} // namespace llt::itch