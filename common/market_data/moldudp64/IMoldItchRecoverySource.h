#pragma once

#include <cstdint>
#include <vector>

#include "market_data/moldudp64/SequencedItchMessage.h"

namespace llt::moldudp64
{

//
// Recovery backend for missing ITCH message
// sequence ranges.
//
// Recovery operates on ITCH messages rather than
// MoldUDP64 datagrams.
//
// The requested sequence range is half-open:
//
//     [fromSequence, toSequence)
//
// Example:
//
//     recover(100, 103)
//
// must return:
//
//     100
//     101
//     102
//
class IMoldItchRecoverySource
{
public:
    virtual ~IMoldItchRecoverySource() = default;

    virtual bool recover(
        std::uint64_t fromSequence,
        std::uint64_t toSequence,
        std::vector<SequencedItchMessage>& messages
    ) = 0;
};

} // namespace llt::moldudp64