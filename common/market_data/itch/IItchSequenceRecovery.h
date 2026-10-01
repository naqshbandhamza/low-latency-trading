#pragma once

#include <cstdint>
#include <vector>

#include "market_data/itch/ItchUdpPacket.h"
#include "types/SequenceCheckResult.h"

namespace llt::itch
{

class IItchSequenceRecovery
{
public:
    virtual ~IItchSequenceRecovery() = default;

    virtual llt::SequenceCheckResult recover(
        std::uint64_t expectedSequence,
        std::uint64_t receivedSequence,
        std::vector<ItchUdpPacket>& recoveredPackets
    ) = 0;
};

} // namespace llt::itch