// #pragma once

// #include <memory>
// #include <array>
// #include <cstddef>
// #include <cstdint>
// #include <limits>

// #include "market_data/normalized/Order.h"
// #include "market_data/book/BookSide.h"

// namespace llt::market_data
// {

//     struct StoredOrder
//     {
//         Order order;
//         BookSide::LevelHandle level;
//     };

//     class OrderStore
//     {
//     public:
//         OrderStore()
//             : slots_(
//                   std::make_unique<Slot[]>(
//                       Capacity))
//         {
//         }

//     private:
//         // Peak observed:
//         //     1,742,866 active orders
//         //
//         // 2^22 slots gives ~41.6% peak load.
//         static constexpr std::size_t Capacity =
//             1ULL << 22;

//         static constexpr std::size_t Mask =
//             Capacity - 1;

//         enum class SlotState : std::uint8_t
//         {
//             Empty,
//             Occupied
//         };

//         struct Slot
//         {
//             StoredOrder value{};
//             SlotState state{SlotState::Empty};
//         };

//     public:
//         using Iterator = std::size_t;
//         using ConstIterator = std::size_t;

//         static constexpr Iterator InvalidIterator =
//             std::numeric_limits<std::size_t>::max();

//         // -----------------------------------------------------
//         // Add
//         // -----------------------------------------------------

//         [[nodiscard]]
//         bool add(
//             const Order &order,
//             BookSide::LevelHandle level) noexcept
//         {
//             std::size_t index =
//                 hash(order.orderId);

//             for (std::size_t probe = 0;
//                  probe < Capacity;
//                  ++probe)
//             {
//                 Slot &slot =
//                     slots_[index];

//                 if (slot.state == SlotState::Empty)
//                 {
//                     slot.value.order =
//                         order;

//                     slot.value.level =
//                         level;

//                     slot.state =
//                         SlotState::Occupied;

//                     ++size_;

//                     if (size_ > peakSize_)
//                     {
//                         peakSize_ = size_;
//                     }

//                     return true;
//                 }

//                 if (slot.value.order.orderId ==
//                     order.orderId)
//                 {
//                     return false;
//                 }

//                 index =
//                     (index + 1) & Mask;
//             }

//             return false;
//         }
//         // -----------------------------------------------------
//         // Pointer lookup
//         // -----------------------------------------------------

//         [[nodiscard]]
//         Order *find(
//             OrderId orderId) noexcept
//         {
//             const Iterator iterator =
//                 findIterator(orderId);

//             if (iterator == InvalidIterator)
//             {
//                 return nullptr;
//             }

//             return &slots_[iterator]
//                         .value
//                         .order;
//         }

//         [[nodiscard]]
//         const Order *find(
//             OrderId orderId) const noexcept
//         {
//             const ConstIterator iterator =
//                 findIterator(orderId);

//             if (iterator == InvalidIterator)
//             {
//                 return nullptr;
//             }

//             return &slots_[iterator]
//                         .value
//                         .order;
//         }

//         // -----------------------------------------------------
//         // Handle lookup
//         // -----------------------------------------------------

//         [[nodiscard]]
//         Iterator findIterator(
//             OrderId orderId) noexcept
//         {
//             return findIndex(orderId);
//         }

//         [[nodiscard]]
//         ConstIterator findIterator(
//             OrderId orderId) const noexcept
//         {
//             return findIndex(orderId);
//         }

//         [[nodiscard]]
//         Iterator end() noexcept
//         {
//             return InvalidIterator;
//         }

//         [[nodiscard]]
//         ConstIterator end() const noexcept
//         {
//             return InvalidIterator;
//         }

//         // -----------------------------------------------------
//         // Access StoredOrder through handle
//         // -----------------------------------------------------

//         [[nodiscard]]
//         StoredOrder &stored(
//             Iterator iterator) noexcept
//         {
//             return slots_[iterator].value;
//         }

//         [[nodiscard]]
//         const StoredOrder &stored(
//             ConstIterator iterator) const noexcept
//         {
//             return slots_[iterator].value;
//         }

//         // -----------------------------------------------------
//         // Membership
//         // -----------------------------------------------------

//         [[nodiscard]]
//         bool contains(
//             OrderId orderId) const noexcept
//         {
//             return findIndex(orderId) !=
//                    InvalidIterator;
//         }

//         // -----------------------------------------------------
//         // Remove by ID
//         // -----------------------------------------------------

//         [[nodiscard]]
//         bool remove(
//             OrderId orderId) noexcept
//         {
//             const Iterator iterator =
//                 findIterator(orderId);

//             if (iterator == InvalidIterator)
//             {
//                 return false;
//             }

//             removeAt(iterator);

//             return true;
//         }

//         // -----------------------------------------------------
//         // Remove by handle
//         // -----------------------------------------------------

//         void removeAt(
//             Iterator iterator) noexcept
//         {
//             std::size_t hole =
//                 iterator;

//             std::size_t current =
//                 (hole + 1) & Mask;

//             while (
//                 slots_[current].state ==
//                 SlotState::Occupied)
//             {
//                 const std::size_t home =
//                     hash(
//                         slots_[current]
//                             .value
//                             .order
//                             .orderId);

//                 const std::size_t distanceToCurrent =
//                     (current - home) & Mask;

//                 const std::size_t distanceToHole =
//                     (hole - home) & Mask;

//                 if (distanceToHole <
//                     distanceToCurrent)
//                 {
//                     slots_[hole].value =
//                         slots_[current].value;

//                     slots_[hole].state =
//                         SlotState::Occupied;

//                     hole =
//                         current;
//                 }

//                 current =
//                     (current + 1) & Mask;
//             }

//             slots_[hole].state =
//                 SlotState::Empty;

//             --size_;
//         }

//         // -----------------------------------------------------
//         // Compatibility with previous experiment
//         //
//         // Fixed-capacity table requires no reserve.
//         // -----------------------------------------------------

//         void reserve(
//             std::size_t) noexcept
//         {
//         }

//         // -----------------------------------------------------
//         // Statistics
//         // -----------------------------------------------------

//         [[nodiscard]]
//         std::size_t size() const noexcept
//         {
//             return size_;
//         }

//         [[nodiscard]]
//         std::size_t peakSize() const noexcept
//         {
//             return peakSize_;
//         }

//     private:
//         [[nodiscard]]
//         static constexpr std::size_t hash(
//             OrderId orderId) noexcept
//         {
//             std::uint64_t x =
//                 static_cast<std::uint64_t>(
//                     orderId);

//             // Fast 64-bit integer finalizer.
//             x ^= x >> 30;
//             x *= 0xbf58476d1ce4e5b9ULL;

//             x ^= x >> 27;
//             x *= 0x94d049bb133111ebULL;

//             x ^= x >> 31;

//             return static_cast<std::size_t>(
//                        x) &
//                    Mask;
//         }

//         [[nodiscard]]
//         Iterator findIndex(
//             OrderId orderId) noexcept
//         {
//             std::size_t index =
//                 hash(orderId);

//             for (std::size_t probe = 0;
//                  probe < Capacity;
//                  ++probe)
//             {
//                 Slot &slot =
//                     slots_[index];

//                 if (slot.state == SlotState::Empty)
//                 {
//                     return InvalidIterator;
//                 }

//                 if (slot.value.order.orderId ==
//                     orderId)
//                 {
//                     return index;
//                 }

//                 index =
//                     (index + 1) & Mask;
//             }

//             return InvalidIterator;
//         }

//         [[nodiscard]]
//         ConstIterator findIndex(
//             OrderId orderId) const noexcept
//         {
//             std::size_t index =
//                 hash(orderId);

//             for (std::size_t probe = 0;
//                  probe < Capacity;
//                  ++probe)
//             {
//                 const Slot &slot =
//                     slots_[index];

//                 if (slot.state == SlotState::Empty)
//                 {
//                     return InvalidIterator;
//                 }

//                 if (slot.value.order.orderId ==
//                     orderId)
//                 {
//                     return index;
//                 }

//                 index =
//                     (index + 1) & Mask;
//             }

//             return InvalidIterator;
//         }

//     private:
//         std::unique_ptr<Slot[]> slots_;

//         std::size_t size_{0};
//         std::size_t peakSize_{0};
//     };

// } // namespace llt::market_data







#pragma once

#include <memory>
#include <cstddef>
#include <cstdint>
#include <limits>

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
    public:
        OrderStore()
            : states_(
                  std::make_unique<std::uint8_t[]>(Capacity)),
              orderIds_(
                  std::make_unique<OrderId[]>(Capacity)),
              orders_(
                  std::make_unique<StoredOrder[]>(Capacity))
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

        static constexpr std::uint8_t Empty = 0;
        static constexpr std::uint8_t Occupied = 1;

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
            std::size_t index =
                hash(order.orderId);

            for (std::size_t probe = 0;
                 probe < Capacity;
                 ++probe)
            {
                if (states_[index] == Empty)
                {
                    orderIds_[index] =
                        order.orderId;

                    orders_[index].order =
                        order;

                    orders_[index].level =
                        level;

                    states_[index] =
                        Occupied;

                    ++size_;

                    if (size_ > peakSize_)
                    {
                        peakSize_ = size_;
                    }

                    return true;
                }

                if (orderIds_[index] ==
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

            return &orders_[iterator].order;
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

            return &orders_[iterator].order;
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
        // Access StoredOrder through handle
        // -----------------------------------------------------

        [[nodiscard]]
        StoredOrder &stored(
            Iterator iterator) noexcept
        {
            return orders_[iterator];
        }

        [[nodiscard]]
        const StoredOrder &stored(
            ConstIterator iterator) const noexcept
        {
            return orders_[iterator];
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
            std::size_t hole =
                iterator;

            std::size_t current =
                (hole + 1) & Mask;

            while (states_[current] == Occupied)
            {
                const std::size_t home =
                    hash(orderIds_[current]);

                const std::size_t distanceToCurrent =
                    (current - home) & Mask;

                const std::size_t distanceToHole =
                    (hole - home) & Mask;

                if (distanceToHole <
                    distanceToCurrent)
                {
                    // Move the ID.
                    orderIds_[hole] =
                        orderIds_[current];

                    // Move the actual order.
                    orders_[hole] =
                        orders_[current];

                    states_[hole] =
                        Occupied;

                    hole =
                        current;
                }

                current =
                    (current + 1) & Mask;
            }

            states_[hole] =
                Empty;

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
                if (states_[index] == Empty)
                {
                    return InvalidIterator;
                }

                if (orderIds_[index] == orderId)
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
                if (states_[index] == Empty)
                {
                    return InvalidIterator;
                }

                if (orderIds_[index] == orderId)
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

        std::unique_ptr<std::uint8_t[]> states_;

        std::unique_ptr<OrderId[]> orderIds_;

        std::unique_ptr<StoredOrder[]> orders_;

        std::size_t size_{0};
        std::size_t peakSize_{0};
    };

} // namespace llt::market_data
