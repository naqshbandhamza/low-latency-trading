#pragma once

#include <cstdint>
#include <vector>

#include "market_data/IMarketDataRecoverySource.h"
#include "market_data/MarketDataMessage.h"

namespace llt
{

class NoopMarketDataRecoverySource final
    : public IMarketDataRecoverySource
{
public:
    bool recover(
        std::uint64_t,
        std::uint64_t,
        std::vector<MarketDataMessage>& messages
    ) override
    {
        messages.clear();
        return false;
    }
};

} // namespace llt