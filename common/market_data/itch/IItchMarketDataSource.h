#pragma once

#include "market_data/itch/ItchUdpPacket.h"

namespace llt::itch
{

class IItchMarketDataSource
{
public:
    virtual ~IItchMarketDataSource() = default;

    [[nodiscard]]
    virtual bool receive(
        ItchUdpPacket& packet
    ) noexcept = 0;
};

} // namespace llt::itch