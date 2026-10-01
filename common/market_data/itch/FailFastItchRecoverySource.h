#pragma once

#include <cstdint>
#include <vector>

#include "market_data/itch/IItchRecoverySource.h"
#include "market_data/itch/ItchUdpPacket.h"

namespace llt::itch
{

class FailFastItchRecoverySource final
    : public IItchRecoverySource
{
public:
    bool recover(
        std::uint64_t,
        std::uint64_t,
        std::vector<ItchUdpPacket>& packets
    ) override
    {
        packets.clear();

        //
        // A real retransmission/recovery channel
        // is not configured.
        //
        // Never claim that missing market data
        // has been recovered.
        //
        return false;
    }
};

} // namespace llt::itch