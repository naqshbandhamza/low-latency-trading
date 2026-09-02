#pragma once
#include <vector>
#include <cstdint>


namespace llt
{

class MarketDataMessage;


class ISequenceRecovery
{
public:

    virtual ~ISequenceRecovery() = default;

    virtual bool recover(
        std::uint64_t expectedSequence,
        std::uint64_t receivedSequence,
        std::vector<MarketDataMessage>& recoveredMessages
    ) = 0;
};

} // namespace llt