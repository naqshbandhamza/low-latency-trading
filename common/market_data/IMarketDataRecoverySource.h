#pragma once

#include <cstdint>
#include <vector>

namespace llt
{

class MarketDataMessage;

class IMarketDataRecoverySource
{
public:

    virtual ~IMarketDataRecoverySource() = default;

    virtual bool recover(
        std::uint64_t fromSequence,
        std::uint64_t toSequence,
        std::vector<MarketDataMessage>& messages
    ) = 0;
};

} // namespace llt