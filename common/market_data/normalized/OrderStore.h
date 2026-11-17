#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>

#include "market_data/normalized/Order.h"
#include "market_data/book/BookSide.h"

namespace llt::market_data
{

    class OrderStore
    {
    public:
        static constexpr OrderId EmptyOrderId = 0;

        OrderStore()
            : orderIds_(
                  std::make_unique<OrderId[]>(Capacity)),
              orders_(
                  std::make_unique<Order[]>(Capacity)),
              levels_(
                  std::make_unique<BookSide::LevelHandle[]>(Capacity))
        {
        }

    private:
        // Peak observed:
        //     1,742,866 active orders
        //
        // 2^22 slots gives ~41.6% peak load.
        static constexpr std::size_t Capacity =
            1ULL << 22;

        static constexpr std::size_t Mask =
            Capacity - 1;

    public:
        using Iterator = std::size_t;
        using ConstIterator = std::size_t;

        static constexpr Iterator InvalidIterator =
            std::numeric_limits<std::size_t>::max();

        // -----------------------------------------------------
        // Add
        // -----------------------------------------------------

        [[nodiscard]]
        bool add(
            const Order &order,
            BookSide::LevelHandle level) noexcept
        {
            assert(order.orderId != EmptyOrderId &&
                   "OrderId 0 is reserved as EmptyOrderId");

            std::size_t index =
                hash(order.orderId);

            for (std::size_t probe = 0;
                 probe < Capacity;
                 ++probe)
            {
                const OrderId existingId =
                    orderIds_[index];

                if (existingId == EmptyOrderId)
                {
                    orderIds_[index] =
                        order.orderId;

                    orders_[index] =
                        order;

                    levels_[index] =
                        level;

                    ++size_;

                    if (size_ > peakSize_)
                    {
                        peakSize_ = size_;
                    }

                    return true;
                }

                if (existingId ==
                    order.orderId)
                {
                    return false;
                }

                index =
                    (index + 1) & Mask;
            }

            return false;
        }

        // -----------------------------------------------------
        // Pointer lookup
        // -----------------------------------------------------

        [[nodiscard]]
        Order *find(
            OrderId orderId) noexcept
        {
            const Iterator iterator =
                findIterator(orderId);

            if (iterator == InvalidIterator)
            {
                return nullptr;
            }

            return &orders_[iterator];
        }

        [[nodiscard]]
        const Order *find(
            OrderId orderId) const noexcept
        {
            const ConstIterator iterator =
                findIterator(orderId);

            if (iterator == InvalidIterator)
            {
                return nullptr;
            }

            return &orders_[iterator];
        }

        // -----------------------------------------------------
        // Handle lookup
        // -----------------------------------------------------

        [[nodiscard]]
        Iterator findIterator(
            OrderId orderId) noexcept
        {
            return findIndex(orderId);
        }

        [[nodiscard]]
        ConstIterator findIterator(
            OrderId orderId) const noexcept
        {
            return findIndex(orderId);
        }

        [[nodiscard]]
        Iterator end() noexcept
        {
            return InvalidIterator;
        }

        [[nodiscard]]
        ConstIterator end() const noexcept
        {
            return InvalidIterator;
        }

        // -----------------------------------------------------
        // Access Order and LevelHandle through handle
        // -----------------------------------------------------

        [[nodiscard]]
        Order &order(
            Iterator iterator) noexcept
        {
            assert(iterator < Capacity &&
                   orderIds_[iterator] != EmptyOrderId &&
                   "Invalid or empty iterator passed to order()");
            return orders_[iterator];
        }

        [[nodiscard]]
        const Order &order(
            ConstIterator iterator) const noexcept
        {
            assert(iterator < Capacity &&
                   orderIds_[iterator] != EmptyOrderId &&
                   "Invalid or empty iterator passed to order()");
            return orders_[iterator];
        }

        [[nodiscard]]
        BookSide::LevelHandle &level(
            Iterator iterator) noexcept
        {
            assert(iterator < Capacity &&
                   orderIds_[iterator] != EmptyOrderId &&
                   "Invalid or empty iterator passed to level()");
            return levels_[iterator];
        }

        [[nodiscard]]
        const BookSide::LevelHandle &level(
            ConstIterator iterator) const noexcept
        {
            assert(iterator < Capacity &&
                   orderIds_[iterator] != EmptyOrderId &&
                   "Invalid or empty iterator passed to level()");
            return levels_[iterator];
        }

        // -----------------------------------------------------
        // Membership
        // -----------------------------------------------------

        [[nodiscard]]
        bool contains(
            OrderId orderId) const noexcept
        {
            return findIndex(orderId) !=
                   InvalidIterator;
        }

        // -----------------------------------------------------
        // Remove by ID
        // -----------------------------------------------------

        [[nodiscard]]
        bool remove(
            OrderId orderId) noexcept
        {
            const Iterator iterator =
                findIterator(orderId);

            if (iterator == InvalidIterator)
            {
                return false;
            }

            removeAt(iterator);

            return true;
        }

        // -----------------------------------------------------
        // Remove by handle
        // -----------------------------------------------------

        void removeAt(
            Iterator iterator) noexcept
        {
            assert(iterator < Capacity &&
                   orderIds_[iterator] != EmptyOrderId &&
                   "Invalid or empty iterator passed to removeAt");

            std::size_t hole =
                iterator;

            std::size_t current =
                (hole + 1) & Mask;

            while (orderIds_[current] != EmptyOrderId)
            {
                const OrderId currentId =
                    orderIds_[current];

                const std::size_t home =
                    hash(currentId);

                const std::size_t distanceToCurrent =
                    (current - home) & Mask;

                const std::size_t distanceToHole =
                    (hole - home) & Mask;

                if (distanceToHole <
                    distanceToCurrent)
                {
                    // Move the ID.
                    orderIds_[hole] =
                        currentId;

                    // Move the order and level handle in lockstep.
                    orders_[hole] =
                        orders_[current];

                    levels_[hole] =
                        levels_[current];

                    hole =
                        current;
                }

                current =
                    (current + 1) & Mask;
            }

            orderIds_[hole] = EmptyOrderId;

            --size_;
        }

        // -----------------------------------------------------
        // Compatibility with previous experiment
        //
        // Fixed-capacity table requires no reserve.
        // -----------------------------------------------------

        void reserve(
            std::size_t) noexcept
        {
        }

        // -----------------------------------------------------
        // Statistics
        // -----------------------------------------------------

        [[nodiscard]]
        std::size_t size() const noexcept
        {
            return size_;
        }

        [[nodiscard]]
        std::size_t peakSize() const noexcept
        {
            return peakSize_;
        }

    private:
        // -----------------------------------------------------
        // Hash
        // -----------------------------------------------------

        [[nodiscard]]
        static constexpr std::size_t hash(
            OrderId orderId) noexcept
        {
            std::uint64_t x =
                static_cast<std::uint64_t>(
                    orderId);

            // Fast 64-bit integer finalizer.
            x ^= x >> 30;
            x *= 0xbf58476d1ce4e5b9ULL;

            x ^= x >> 27;
            x *= 0x94d049bb133111ebULL;

            x ^= x >> 31;

            return static_cast<std::size_t>(
                       x) &
                   Mask;
        }

        // -----------------------------------------------------
        // Internal lookup
        // -----------------------------------------------------

        [[nodiscard]]
        Iterator findIndex(
            OrderId orderId) noexcept
        {
            std::size_t index =
                hash(orderId);

            for (std::size_t probe = 0;
                 probe < Capacity;
                 ++probe)
            {
                const OrderId existingId =
                    orderIds_[index];

                if (existingId == EmptyOrderId)
                {
                    return InvalidIterator;
                }

                if (existingId == orderId)
                {
                    return index;
                }

                index =
                    (index + 1) & Mask;
            }

            return InvalidIterator;
        }

        [[nodiscard]]
        ConstIterator findIndex(
            OrderId orderId) const noexcept
        {
            std::size_t index =
                hash(orderId);

            for (std::size_t probe = 0;
                 probe < Capacity;
                 ++probe)
            {
                const OrderId existingId =
                    orderIds_[index];

                if (existingId == EmptyOrderId)
                {
                    return InvalidIterator;
                }

                if (existingId == orderId)
                {
                    return index;
                }

                index =
                    (index + 1) & Mask;
            }

            return InvalidIterator;
        }

    private:
        // -----------------------------------------------------
        // Structure of Arrays (SoA)
        // -----------------------------------------------------

        std::unique_ptr<OrderId[]> orderIds_;

        std::unique_ptr<Order[]> orders_;

        std::unique_ptr<BookSide::LevelHandle[]> levels_;

        std::size_t size_{0};
        std::size_t peakSize_{0};
    };

} // namespace llt::market_data