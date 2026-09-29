#pragma once

#include <cstddef>
#include <unordered_map>

#include "market_data/book/OrderBook.h"
#include "market_data/normalized/MarketTypes.h"

namespace llt::market_data
{

class BookStore
{
public:
    [[nodiscard]]
    OrderBook& getOrCreate(
        InstrumentId instrumentId
    )
    {
        auto [iterator, inserted] =
            books_.try_emplace(
                instrumentId
            );

        return iterator->second;
    }

    [[nodiscard]]
    OrderBook* find(
        InstrumentId instrumentId
    ) noexcept
    {
        const auto iterator =
            books_.find(instrumentId);

        if (iterator == books_.end())
        {
            return nullptr;
        }

        return &iterator->second;
    }

    [[nodiscard]]
    const OrderBook* find(
        InstrumentId instrumentId
    ) const noexcept
    {
        const auto iterator =
            books_.find(instrumentId);

        if (iterator == books_.end())
        {
            return nullptr;
        }

        return &iterator->second;
    }

    [[nodiscard]]
    bool contains(
        InstrumentId instrumentId
    ) const noexcept
    {
        return books_.find(instrumentId) !=
               books_.end();
    }

    [[nodiscard]]
    std::size_t size() const noexcept
    {
        return books_.size();
    }

private:
    std::unordered_map<
        InstrumentId,
        OrderBook
    > books_;
};

} // namespace llt::market_data