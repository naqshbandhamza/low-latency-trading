#pragma once

//#include "market_data/MarketDataMessage.h"


namespace llt
{

class  MarketDataMessage;

class IMarketDataSource
{
public:

    virtual ~IMarketDataSource() = default;

    virtual bool receive(
        MarketDataMessage& message
    ) noexcept = 0;
};

} // namespace llt