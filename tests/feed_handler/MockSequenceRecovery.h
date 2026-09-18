#pragma once

#include <cstdint>
#include <vector>

#include "market_data/ISequenceRecovery.h"
#include "market_data/MarketDataMessage.h"
#include "types/SequenceCheckResult.h"

class MockSequenceRecovery
    : public llt::ISequenceRecovery
{
public:

        llt::SequenceCheckResult recover(
        std::uint64_t expectedSequence,
        std::uint64_t receivedSequence,
        std::vector<llt::MarketDataMessage>& recoveredMessages
    ) override
    {
        called = true;

        expected = expectedSequence;

        received = receivedSequence;

        recoveredMessages.clear();

        for (
            std::uint64_t sequence = expectedSequence;
            sequence < receivedSequence;
            ++sequence
        )
        {
            llt::MarketDataMessage message;

            message.type =
                llt::MarketDataMessageType::Quote;

            message.sequence =
                sequence;

            message.timestamp =
                sequence;

            message.bidPrice =
                234500;

            message.bidQuantity =
                10;

            message.askPrice =
                234510;

            message.askQuantity =
                12;

            recoveredMessages.push_back(
                message
            );
        }

        return llt::SequenceCheckResult::Process;
    }

    bool called{false};

    std::uint64_t expected{0};

    std::uint64_t received{0};
};