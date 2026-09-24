#pragma once

#include <optional>

#include "market_data/MarketEvent.h"
#include "strategy/OrderIntent.h"

namespace llt
{

class IStrategy
{
public:

    virtual ~IStrategy() = default;

    virtual std::optional<OrderIntent>
    onMarketEvent(
        const MarketEvent& event
    ) = 0;
};

} // namespace llt