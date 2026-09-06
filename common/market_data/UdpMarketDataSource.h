#pragma once

#include <cstdint>

#include "market_data/IMarketDataSource.h"

namespace llt
{

class UdpMarketDataSource
    : public IMarketDataSource
{
public:

    explicit UdpMarketDataSource(
        std::uint16_t port,
        std::uint32_t receiveTimeoutMs = 100
    );

    ~UdpMarketDataSource() override;

    UdpMarketDataSource(
        const UdpMarketDataSource&
    ) = delete;

    UdpMarketDataSource& operator=(
        const UdpMarketDataSource&
    ) = delete;

    bool receive(
        MarketDataMessage& message
    ) noexcept override;

private:

    int socket_{-1};

};

} // namespace llt