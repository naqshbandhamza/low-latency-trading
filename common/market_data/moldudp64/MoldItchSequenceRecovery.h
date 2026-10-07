#pragma once

#include "market_data/moldudp64/IMoldItchSequenceRecovery.h"

namespace llt::moldudp64
{

class IMoldItchRecoverySource;

class MoldItchSequenceRecovery final
    : public IMoldItchSequenceRecovery
{
public:
    explicit MoldItchSequenceRecovery(
        IMoldItchRecoverySource& source
    ) noexcept;

    llt::SequenceCheckResult recover(
        std::uint64_t expectedSequence,
        std::uint64_t receivedSequence,
        std::vector<SequencedItchMessage>& recoveredMessages
    ) override;

private:
    IMoldItchRecoverySource& source_;
};

} // namespace llt::moldudp64