#include "market_data/MarketDataMessage.h"
#include "SequenceRecovery.h"
#include "IMarketDataRecoverySource.h"
#include "logging/ILogger.h"
#include "types/SequenceCheckResult.h"

#include <string>

namespace llt
{

SequenceCheckResult SequenceRecovery::recover(
    std::uint64_t expectedSequence,
    std::uint64_t receivedSequence,
    std::vector<MarketDataMessage>& recoveredMessages
)
{
    logger_.warning(
        "Sequence recovery required: expected="
        + std::to_string(expectedSequence)
        + " received="
        + std::to_string(receivedSequence)
    );

    recoveredMessages.clear();

    if (receivedSequence <= expectedSequence)
    {
        logger_.error(
            "Invalid sequence for recovery: expected="
            + std::to_string(expectedSequence)
            + " received="
            + std::to_string(receivedSequence)
        );

        return SequenceCheckResult::Ignore;
    }

    const bool recovered =
        source_.recover(
            expectedSequence,
            receivedSequence,
            recoveredMessages
        );

    if (!recovered)
    {
        return SequenceCheckResult::Stop;
    }
    logger_.info("recvoered success"+std::to_string(recoveredMessages.size()));

    const std::uint64_t expectedCount =
        receivedSequence - expectedSequence;
    
    if (recoveredMessages.size() != expectedCount)
    {
        logger_.error(
            "Sequence recovery returned incorrect message count"
        );

        recoveredMessages.clear();
        return SequenceCheckResult::Stop;
    }

    for (std::size_t i = 0; i < recoveredMessages.size(); ++i)
    {
        const std::uint64_t expected =
            expectedSequence + i;

        if (recoveredMessages[i].sequence != expected)
        {
            logger_.error(
                "Sequence recovery returned invalid sequence"
            );

            recoveredMessages.clear();
            return SequenceCheckResult::Stop;
        }
    }

    return SequenceCheckResult::Process;
}

} // namespace llt