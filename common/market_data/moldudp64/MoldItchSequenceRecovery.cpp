#include "market_data/moldudp64/MoldItchSequenceRecovery.h"

#include <cstddef>

#include "market_data/moldudp64/IMoldItchRecoverySource.h"

namespace llt::moldudp64
{

MoldItchSequenceRecovery::MoldItchSequenceRecovery(
    IMoldItchRecoverySource& source
) noexcept
    : source_(source)
{
}


llt::SequenceCheckResult
MoldItchSequenceRecovery::recover(
    std::uint64_t expectedSequence,
    std::uint64_t receivedSequence,
    std::vector<SequencedItchMessage>& recoveredMessages
)
{
    recoveredMessages.clear();

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
        return
            llt::SequenceCheckResult::Ignore;
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
            recoveredMessages))
    {
        recoveredMessages.clear();

        return
            llt::SequenceCheckResult::Stop;
    }

    const auto expectedCount =
        receivedSequence -
        expectedSequence;

    //
    // Recovery must be complete.
    //
    if (
        recoveredMessages.size() !=
        expectedCount)
    {
        recoveredMessages.clear();

        return
            llt::SequenceCheckResult::Stop;
    }

    //
    // Recovery must also be exactly ordered
    // and contiguous.
    //
    for (
        std::size_t i = 0;
        i < recoveredMessages.size();
        ++i)
    {
        const auto requiredSequence =
            expectedSequence +
            static_cast<std::uint64_t>(i);

        if (
            recoveredMessages[i].sequence !=
            requiredSequence)
        {
            recoveredMessages.clear();

            return
                llt::SequenceCheckResult::Stop;
        }
    }

    return
        llt::SequenceCheckResult::Process;
}

} // namespace llt::moldudp64