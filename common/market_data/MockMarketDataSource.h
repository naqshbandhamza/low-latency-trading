#pragma once

#include <cstdint>

#include "market_data/IMarketDataSource.h"
#include "market_data/MarketDataMessage.h"

namespace llt
{

class MockMarketDataSource
    : public IMarketDataSource
{
public:

    bool receive(
        MarketDataMessage& message
    ) noexcept override
    {
        const auto sequence =
            sequence_++;

        message.sequence =
            sequence;

        message.timestamp =
            sequence;

        if ((sequence & 1) == 0)
        {
            message.type =
                MarketDataMessageType::Quote;

            message.bidPrice =
                234500;

            message.bidQuantity =
                10;

            message.askPrice =
                234510;

            message.askQuantity =
                12;
        }
        else
        {
            message.type =
                MarketDataMessageType::Trade;

            message.price =
                234505;

            message.quantity =
                3;

            message.side =
                Side::Buy;
        }

        return true;
    }

private:

    std::uint64_t sequence_{0};
};

} // namespace llt