#include "market_data/MarketDataMessage.h"
#include "SequenceRecovery.h"
#include "IMarketDataRecoverySource.h"
#include "logging/ILogger.h"

#include <string>

namespace llt
{

bool SequenceRecovery::recover(
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

    if (expectedSequence >= receivedSequence)
    {
        return true;
    }

    return source_.recover(
        expectedSequence,
        receivedSequence - 1,
        recoveredMessages
    );
}

} // namespace llt