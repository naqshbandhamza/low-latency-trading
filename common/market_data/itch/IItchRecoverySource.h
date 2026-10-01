#pragma once

#include <cstdint>
#include <vector>

#include "market_data/itch/ItchUdpPacket.h"

namespace llt::itch
{

class IItchRecoverySource
{
public:
    virtual ~IItchRecoverySource() = default;

    //
    // Recover the half-open range:
    //
    // [fromSequence, toSequence)
    //
    // Example:
    //
    // from = 102
    // to   = 105
    //
    // must return:
    //
    // 102, 103, 104
    //
    virtual bool recover(
        std::uint64_t fromSequence,
        std::uint64_t toSequence,
        std::vector<ItchUdpPacket>& packets
    ) = 0;
};

} // namespace llt::itch