#include "market_data/itch/ItchSequenceRecovery.h"

#include <cstddef>

#include "market_data/itch/IItchRecoverySource.h"

namespace llt::itch
{

ItchSequenceRecovery::ItchSequenceRecovery(
    IItchRecoverySource& source
) noexcept
    : source_(source)
{
}


llt::SequenceCheckResult
ItchSequenceRecovery::recover(
    std::uint64_t expectedSequence,
    std::uint64_t receivedSequence,
    std::vector<ItchUdpPacket>& recoveredPackets
)
{
    recoveredPackets.clear();

    //
    // Recovery only makes sense for a forward gap.
    //
    // expected = 102
    // received = 105
    //
    // Missing:
    // 102, 103, 104
    //
    if (receivedSequence <= expectedSequence)
    {
        return llt::SequenceCheckResult::Ignore;
    }

    //
    // Recovery range is half-open:
    //
    // [expectedSequence, receivedSequence)
    //
    if (
        !source_.recover(
            expectedSequence,
            receivedSequence,
            recoveredPackets))
    {
        recoveredPackets.clear();

        return llt::SequenceCheckResult::Stop;
    }

    const auto expectedCount =
        receivedSequence -
        expectedSequence;

    //
    // Recovery must be complete.
    //
    if (
        recoveredPackets.size() !=
        expectedCount)
    {
        recoveredPackets.clear();

        return llt::SequenceCheckResult::Stop;
    }

    //
    // Recovery must also be exactly ordered and
    // contiguous.
    //
    for (
        std::size_t i = 0;
        i < recoveredPackets.size();
        ++i)
    {
        const auto requiredSequence =
            expectedSequence +
            static_cast<std::uint64_t>(i);

        if (
            recoveredPackets[i].sequence !=
            requiredSequence)
        {
            recoveredPackets.clear();

            return llt::SequenceCheckResult::Stop;
        }
    }

    return llt::SequenceCheckResult::Process;
}

} // namespace llt::itch