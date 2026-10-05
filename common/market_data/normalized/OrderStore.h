// #pragma once

// #include <cstddef>
// #include <unordered_map>

// #include "market_data/normalized/Order.h"

// namespace llt::market_data
// {

// class OrderStore
// {
// public:
//     [[nodiscard]]
//     bool add(
//         const Order& order
//     )
//     {
//         const auto [iterator, inserted] =
//             orders_.emplace(
//                 order.orderId,
//                 order
//             );

//         return inserted;
//     }

//     [[nodiscard]]
//     Order* find(
//         OrderId orderId
//     ) noexcept
//     {
//         const auto iterator =
//             orders_.find(orderId);

//         if (iterator == orders_.end())
//         {
//             return nullptr;
//         }

//         return &iterator->second;
//     }

//     [[nodiscard]]
//     const Order* find(
//         OrderId orderId
//     ) const noexcept
//     {
//         const auto iterator =
//             orders_.find(orderId);

//         if (iterator == orders_.end())
//         {
//             return nullptr;
//         }

//         return &iterator->second;
//     }

//     [[nodiscard]]
//     bool contains(
//         OrderId orderId
//     ) const noexcept
//     {
//         return orders_.find(orderId) !=
//                orders_.end();
//     }

//     [[nodiscard]]
//     bool remove(
//         OrderId orderId
//     ) noexcept
//     {
//         return orders_.erase(orderId) != 0;
//     }

//     [[nodiscard]]
//     std::size_t size() const noexcept
//     {
//         return orders_.size();
//     }

// private:
//     std::unordered_map<
//         OrderId,
//         Order
//     > orders_;
// };

// } // namespace llt::market_data

#pragma once

#include <cstddef>
#include <unordered_map>

#include "market_data/normalized/Order.h"
#include "market_data/book/BookSide.h"

namespace llt::market_data
{

    struct StoredOrder
    {
        Order order;
        BookSide::LevelHandle level;
    };

    class OrderStore
    {
    private:
        // using Container =
        //     std::unordered_map<
        //         OrderId,
        //         Order>;

        using Container =
            std::unordered_map<
                OrderId,
                StoredOrder>;

    public:
        using Iterator =
            Container::iterator;

        using ConstIterator =
            Container::const_iterator;

        // [[nodiscard]]
        // bool add(
        //     const Order &order)
        // {
        //     const auto [iterator, inserted] =
        //         orders_.emplace(
        //             order.orderId,
        //             order);

        //     return inserted;
        // }

        [[nodiscard]]
        bool add(
            const Order &order,
            BookSide::LevelHandle level)
        {
            const auto [iterator, inserted] =
                orders_.emplace(
                    order.orderId,
                    StoredOrder{
                        .order = order,
                        .level = level});

            return inserted;
        }

        // -----------------------------------------------------
        // Existing pointer-based API
        // -----------------------------------------------------

        [[nodiscard]]
        Order *find(
            OrderId orderId) noexcept
        {
            const auto iterator =
                orders_.find(
                    orderId);

            if (iterator == orders_.end())
            {
                return nullptr;
            }

            return &iterator->second.order;
        }

        [[nodiscard]]
        const Order *find(
            OrderId orderId) const noexcept
        {
            const auto iterator =
                orders_.find(
                    orderId);

            if (iterator == orders_.end())
            {
                return nullptr;
            }

            return &iterator->second.order;
        }

        // -----------------------------------------------------
        // Iterator API
        //
        // Allows hot paths to:
        //
        //     find once
        //         ->
        //     use Order
        //         ->
        //     erase same node
        //
        // without performing another key lookup.
        // -----------------------------------------------------

        [[nodiscard]]
        Iterator findIterator(
            OrderId orderId) noexcept
        {
            return orders_.find(
                orderId);
        }

        [[nodiscard]]
        ConstIterator findIterator(
            OrderId orderId) const noexcept
        {
            return orders_.find(
                orderId);
        }

        [[nodiscard]]
        Iterator end() noexcept
        {
            return orders_.end();
        }

        [[nodiscard]]
        ConstIterator end() const noexcept
        {
            return orders_.end();
        }

        // -----------------------------------------------------
        // Membership
        // -----------------------------------------------------

        [[nodiscard]]
        bool contains(
            OrderId orderId) const noexcept
        {
            return orders_.find(
                       orderId) !=
                   orders_.end();
        }

        // -----------------------------------------------------
        // Key-based removal
        //
        // Keep this API because existing callers/tests may
        // depend on it.
        // -----------------------------------------------------

        [[nodiscard]]
        bool remove(
            OrderId orderId) noexcept
        {
            return orders_.erase(
                       orderId) != 0;
        }

        // -----------------------------------------------------
        // Iterator-based removal
        //
        // No second key-based hash-table lookup.
        // -----------------------------------------------------

        void remove(
            Iterator iterator) noexcept
        {
            orders_.erase(
                iterator);
        }

        [[nodiscard]]
        std::size_t size() const noexcept
        {
            return orders_.size();
        }

    private:
        Container orders_;
    };

} // namespace llt::market_data