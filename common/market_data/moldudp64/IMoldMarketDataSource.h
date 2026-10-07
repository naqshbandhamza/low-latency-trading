#pragma once

#include "market_data/moldudp64/MoldUdp64.h"

namespace llt::moldudp64
{

class IMoldMarketDataSource
{
public:
    virtual ~IMoldMarketDataSource() = default;

    virtual bool receive(
        ReceivedMoldDatagram& datagram
    ) noexcept = 0;
};

} // namespace llt::moldudp64