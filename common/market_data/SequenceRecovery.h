#pragma once

#include <vector>

#include "ISequenceRecovery.h"
#include "IMarketDataRecoverySource.h"
#include "logging/ILogger.h"

namespace llt
{

class SequenceRecovery
    : public ISequenceRecovery
{
public:

    SequenceRecovery(
        ILogger& logger,
        IMarketDataRecoverySource& source
    ) noexcept
        : logger_(logger)
        , source_(source)
    {
    }

    bool recover(
        std::uint64_t expectedSequence,
        std::uint64_t receivedSequence,
        std::vector<MarketDataMessage>& recoveredMessages
    ) override;

private:

    ILogger& logger_;

    IMarketDataRecoverySource& source_;
};

} // namespace llt