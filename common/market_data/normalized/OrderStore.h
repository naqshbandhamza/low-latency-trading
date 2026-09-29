#pragma once

#include <cstddef>
#include <unordered_map>

#include "market_data/normalized/Order.h"

namespace llt::market_data
{

class OrderStore
{
public:
    [[nodiscard]]
    bool add(
        const Order& order
    )
    {
        const auto [iterator, inserted] =
            orders_.emplace(
                order.orderId,
                order
            );

        return inserted;
    }

    [[nodiscard]]
    Order* find(
        OrderId orderId
    ) noexcept
    {
        const auto iterator =
            orders_.find(orderId);

        if (iterator == orders_.end())
        {
            return nullptr;
        }

        return &iterator->second;
    }

    [[nodiscard]]
    const Order* find(
        OrderId orderId
    ) const noexcept
    {
        const auto iterator =
            orders_.find(orderId);

        if (iterator == orders_.end())
        {
            return nullptr;
        }

        return &iterator->second;
    }

    [[nodiscard]]
    bool contains(
        OrderId orderId
    ) const noexcept
    {
        return orders_.find(orderId) !=
               orders_.end();
    }

    [[nodiscard]]
    bool remove(
        OrderId orderId
    ) noexcept
    {
        return orders_.erase(orderId) != 0;
    }

    [[nodiscard]]
    std::size_t size() const noexcept
    {
        return orders_.size();
    }

private:
    std::unordered_map<
        OrderId,
        Order
    > orders_;
};

} // namespace llt::market_data