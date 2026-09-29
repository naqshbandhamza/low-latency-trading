#pragma once

#include <cstddef>
#include <map>

#include "market_data/book/PriceLevel.h"
#include "market_data/normalized/MarketTypes.h"

namespace llt::market_data
{

class BookSide
{
public:
    explicit BookSide(
        Side side
    ) noexcept
        : side_(side)
    {
    }

    [[nodiscard]]
    Side side() const noexcept
    {
        return side_;
    }

    void add(
        Price price,
        Quantity quantity
    )
    {
        auto [iterator, inserted] =
            levels_.try_emplace(
                price,
                PriceLevel{
                    .price = price
                }
            );

        iterator->second.add(quantity);
    }

    [[nodiscard]]
    bool reduce(
        Price price,
        Quantity quantity
    ) noexcept
    {
        auto iterator =
            levels_.find(price);

        if (iterator == levels_.end())
        {
            return false;
        }

        return iterator->second.reduce(
            quantity
        );
    }

    [[nodiscard]]
    bool removeOrder(
        Price price,
        Quantity remainingQuantity
    ) noexcept
    {
        auto iterator =
            levels_.find(price);

        if (iterator == levels_.end())
        {
            return false;
        }

        if (!iterator->second.removeOrder(
                remainingQuantity
            ))
        {
            return false;
        }

        if (iterator->second.empty())
        {
            levels_.erase(iterator);
        }

        return true;
    }

    [[nodiscard]]
    const PriceLevel*
    find(
        Price price
    ) const noexcept
    {
        const auto iterator =
            levels_.find(price);

        if (iterator == levels_.end())
        {
            return nullptr;
        }

        return &iterator->second;
    }

    [[nodiscard]]
    const PriceLevel*
    best() const noexcept
    {
        if (levels_.empty())
        {
            return nullptr;
        }

        if (side_ == Side::Buy)
        {
            return &levels_.rbegin()->second;
        }

        return &levels_.begin()->second;
    }

    [[nodiscard]]
    std::size_t levelCount() const noexcept
    {
        return levels_.size();
    }

    [[nodiscard]]
    bool empty() const noexcept
    {
        return levels_.empty();
    }

private:
    Side side_;
    std::map<
        Price,
        PriceLevel
    > levels_;
};

} // namespace llt::market_data