#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "market_data/itch/IItchSequenceRecovery.h"
#include "market_data/itch/ItchUdpPacket.h"
#include "types/SequenceCheckResult.h"

namespace llt::itch::test
{

class MockItchSequenceRecovery final
    : public IItchSequenceRecovery
{
public:
    llt::SequenceCheckResult result{
        llt::SequenceCheckResult::Process};

    std::vector<ItchUdpPacket>
        packetsToReturn{};

    std::size_t callCount{0};

    std::uint64_t lastExpectedSequence{0};

    std::uint64_t lastReceivedSequence{0};

    llt::SequenceCheckResult recover(
        std::uint64_t expectedSequence,
        std::uint64_t receivedSequence,
        std::vector<ItchUdpPacket>& recoveredPackets
    ) override
    {
        ++callCount;

        lastExpectedSequence =
            expectedSequence;

        lastReceivedSequence =
            receivedSequence;

        recoveredPackets =
            packetsToReturn;

        return result;
    }
};

} // namespace llt::itch::test