#pragma once

#include <cstdint>
#include <vector>

#include "market_data/moldudp64/SequencedItchMessage.h"
#include "types/SequenceCheckResult.h"

namespace llt::moldudp64
{

class IMoldItchSequenceRecovery
{
public:
    virtual ~IMoldItchSequenceRecovery() = default;

    virtual llt::SequenceCheckResult recover(
        std::uint64_t expectedSequence,
        std::uint64_t receivedSequence,
        std::vector<SequencedItchMessage>& recoveredMessages
    ) = 0;
};

} // namespace llt::moldudp64