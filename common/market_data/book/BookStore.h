// #pragma once

// #include <cstddef>
// #include <unordered_map>

// #include "market_data/book/OrderBook.h"
// #include "market_data/normalized/MarketTypes.h"

// namespace llt::market_data
// {

//     class BookStore
//     {
//     public:
//         [[nodiscard]]
//         OrderBook &getOrCreate(
//             InstrumentId instrumentId)
//         {
//             auto [iterator, inserted] =
//                 books_.try_emplace(
//                     instrumentId);

//             return iterator->second;
//         }

//         [[nodiscard]]
//         OrderBook *find(
//             InstrumentId instrumentId) noexcept
//         {
//             const auto iterator =
//                 books_.find(instrumentId);

//             if (iterator == books_.end())
//             {
//                 return nullptr;
//             }

//             return &iterator->second;
//         }

//         [[nodiscard]]
//         const OrderBook *find(
//             InstrumentId instrumentId) const noexcept
//         {
//             const auto iterator =
//                 books_.find(instrumentId);

//             if (iterator == books_.end())
//             {
//                 return nullptr;
//             }

//             return &iterator->second;
//         }

//         [[nodiscard]]
//         bool contains(
//             InstrumentId instrumentId) const noexcept
//         {
//             return books_.find(instrumentId) !=
//                    books_.end();
//         }

//         [[nodiscard]]
//         std::size_t size() const noexcept
//         {
//             return books_.size();
//         }

//         template <typename Fn>
//         void forEach(
//             Fn &&fn) const
//         {
//             for (const auto &[instrumentId, book] : books_)
//             {
//                 fn(
//                     instrumentId,
//                     book);
//             }
//         }

//     private:
//         std::unordered_map<
//             InstrumentId,
//             OrderBook>
//             books_;
//     };

// } // namespace llt::market_data




#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "market_data/book/OrderBook.h"
#include "market_data/normalized/MarketTypes.h"

namespace llt::market_data
{

class BookStore
{
public:
    // InstrumentId uses the same direct-index domain
    // as InstrumentStore.
    static constexpr std::size_t Capacity =
        1ULL << 16;

    // -----------------------------------------------------
    // Get or create
    // -----------------------------------------------------

    [[nodiscard]]
    OrderBook& getOrCreate(
        InstrumentId instrumentId) noexcept
    {
        const auto index =
            static_cast<std::size_t>(
                instrumentId);

        if (!present_[index])
        {
            present_[index] = true;
            ++size_;
        }

        return books_[index];
    }

    // -----------------------------------------------------
    // Find
    // -----------------------------------------------------

    [[nodiscard]]
    OrderBook* find(
        InstrumentId instrumentId) noexcept
    {
        const auto index =
            static_cast<std::size_t>(
                instrumentId);

        if (!present_[index])
        {
            return nullptr;
        }

        return &books_[index];
    }

    [[nodiscard]]
    const OrderBook* find(
        InstrumentId instrumentId) const noexcept
    {
        const auto index =
            static_cast<std::size_t>(
                instrumentId);

        if (!present_[index])
        {
            return nullptr;
        }

        return &books_[index];
    }

    // -----------------------------------------------------
    // Membership
    // -----------------------------------------------------

    [[nodiscard]]
    bool contains(
        InstrumentId instrumentId) const noexcept
    {
        return present_[
            static_cast<std::size_t>(
                instrumentId)
        ];
    }

    // -----------------------------------------------------
    // Size
    // -----------------------------------------------------

    [[nodiscard]]
    std::size_t size() const noexcept
    {
        return size_;
    }

    // -----------------------------------------------------
    // Iteration
    // -----------------------------------------------------

    template <typename Fn>
    void forEach(
        Fn&& fn) const
    {
        for (
            std::size_t index = 0;
            index < Capacity;
            ++index)
        {
            if (!present_[index])
            {
                continue;
            }

            fn(
                static_cast<InstrumentId>(
                    index),
                books_[index]);
        }
    }

private:
    std::array<
        OrderBook,
        Capacity
    > books_{};

    std::array<
        bool,
        Capacity
    > present_{};

    std::size_t size_{0};
};

} // namespace llt::market_data