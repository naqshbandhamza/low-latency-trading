#pragma once

#include <cstdint>
#include <vector>

#include "market_data/itch/IItchRecoverySource.h"

namespace llt::itch::test
{

class MockItchRecoverySource final
    : public IItchRecoverySource
{
public:
    bool succeed{true};

    std::vector<ItchUdpPacket>
        packetsToReturn{};

    std::uint64_t lastFromSequence{0};
    std::uint64_t lastToSequence{0};

    std::size_t callCount{0};

    bool recover(
        std::uint64_t fromSequence,
        std::uint64_t toSequence,
        std::vector<ItchUdpPacket>& packets
    ) override
    {
        ++callCount;

        lastFromSequence =
            fromSequence;

        lastToSequence =
            toSequence;

        if (!succeed)
        {
            return false;
        }

        packets =
            packetsToReturn;

        return true;
    }
};

} // namespace llt::itch::test