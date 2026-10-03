#pragma once

#include <cstdint>

#include "market_data/MarketEventQueue.h"
#include "market_data/normalized/InstrumentStore.h"
#include "market_data/normalized/Order.h"

namespace llt::itch
{

class ItchTradePublisher
{
public:
    ItchTradePublisher(
        const market_data::InstrumentStore& instruments,
        MarketEventQueue& queue) noexcept
        : instruments_(instruments),
          queue_(queue)
    {
    }

    void onOrderExecution(
        market_data::InstrumentId instrumentId,
        market_data::Timestamp timestamp,
        market_data::Price price,
        market_data::Quantity quantity,
        market_data::Side side);

    [[nodiscard]]
    std::uint64_t publishedTrades() const noexcept
    {
        return publishedTrades_;
    }

    [[nodiscard]]
    std::uint64_t droppedTrades() const noexcept
    {
        return droppedTrades_;
    }

    [[nodiscard]]
    std::uint64_t unknownInstruments() const noexcept
    {
        return unknownInstruments_;
    }

private:
    const market_data::InstrumentStore& instruments_;
    MarketEventQueue& queue_;

    std::uint64_t nextSequence_{0};

    std::uint64_t publishedTrades_{0};
    std::uint64_t droppedTrades_{0};
    std::uint64_t unknownInstruments_{0};
};

} // namespace llt::itch