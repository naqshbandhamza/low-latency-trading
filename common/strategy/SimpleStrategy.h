#pragma once

#include <optional>

#include "strategy/IStrategy.h"

namespace llt
{

class SimpleStrategy final
    : public IStrategy
{
public:

    std::optional<OrderIntent>
    onMarketEvent(
        const MarketEvent& event
    ) override;

private:

    std::optional<OrderIntent>
    onQuote(
        const Quote& quote
    );

    std::optional<OrderIntent>
    onTrade(
        const Trade& trade
    );
};

} // namespace llt