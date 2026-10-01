#pragma once

#include "market_data/itch/IItchSequenceRecovery.h"

namespace llt::itch
{

class IItchRecoverySource;

class ItchSequenceRecovery final
    : public IItchSequenceRecovery
{
public:
    explicit ItchSequenceRecovery(
        IItchRecoverySource& source
    ) noexcept;

    llt::SequenceCheckResult recover(
        std::uint64_t expectedSequence,
        std::uint64_t receivedSequence,
        std::vector<ItchUdpPacket>& recoveredPackets
    ) override;

private:
    IItchRecoverySource& source_;
};

} // namespace llt::itch