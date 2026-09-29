#pragma once

#include <cstddef>

#include "market_data/normalized/MarketTypes.h"

namespace llt::market_data
{

struct PriceLevel
{
    Price price{0};
    Quantity quantity{0};
    std::size_t orderCount{0};

    void add(
        Quantity addedQuantity
    ) noexcept
    {
        quantity += addedQuantity;
        ++orderCount;
    }

    [[nodiscard]]
    bool reduce(
        Quantity reducedQuantity
    ) noexcept
    {
        if (reducedQuantity > quantity)
        {
            return false;
        }

        quantity -= reducedQuantity;
        return true;
    }

    [[nodiscard]]
    bool removeOrder(
        Quantity remainingOrderQuantity
    ) noexcept
    {
        if (
            orderCount == 0 ||
            remainingOrderQuantity > quantity
        )
        {
            return false;
        }

        quantity -= remainingOrderQuantity;
        --orderCount;

        return true;
    }

    [[nodiscard]]
    bool empty() const noexcept
    {
        return orderCount == 0;
    }
};

} // namespace llt::market_data