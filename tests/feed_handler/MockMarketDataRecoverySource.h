#pragma once

#include <cstdint>
#include <vector>

#include "market_data/IMarketDataRecoverySource.h"
#include "market_data/MarketDataMessage.h"

class MockMarketDataRecoverySource
    : public llt::IMarketDataRecoverySource
{
public:

    bool recover(
        std::uint64_t fromSequence,
        std::uint64_t toSequence,
        std::vector<llt::MarketDataMessage>& messages
    ) override
    {
        called = true;

        from = fromSequence;

        to = toSequence;

        messages.clear();

        if (!shouldRecover)
        {
            return false;
        }

        for (
            std::uint64_t sequence = fromSequence;
            sequence <= toSequence;
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

            messages.push_back(
                message
            );
        }

        return true;
    }

    bool called{false};

    bool shouldRecover{true};

    std::uint64_t from{0};

    std::uint64_t to{0};
};