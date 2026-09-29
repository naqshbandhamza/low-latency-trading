#pragma once

#include <iostream>
#include <cstdint>
#include "market_data/itch/ItchMessage.h"
#include "market_data/itch/ItchNormalizer.h"
#include "market_data/normalized/InstrumentStore.h"
#include "market_data/normalized/OrderStore.h"
#include "market_data/book/BookStore.h"

namespace llt::itch
{

    class ItchMarketState
    {
    public:
        [[nodiscard]]
        market_data::OrderStore &
        orders() noexcept
        {
            return orders_;
        }

        [[nodiscard]]
        const market_data::OrderStore &
        orders() const noexcept
        {
            return orders_;
        }

        void onMessage(
            const ItchMessage &message) noexcept
        {
            std::visit(
                [this](const auto &msg)
                {
                    handle(msg);
                },
                message);
        }

        [[nodiscard]]
        market_data::InstrumentStore &
        instruments() noexcept
        {
            return instruments_;
        }

        [[nodiscard]]
        const market_data::InstrumentStore &
        instruments() const noexcept
        {
            return instruments_;
        }

        [[nodiscard]]
        std::uint64_t unknownTradingActionInstruments() const noexcept
        {
            return unknownTradingActionInstruments_;
        }

        [[nodiscard]]
        std::uint64_t unknownRegShoInstruments() const noexcept
        {
            return unknownRegShoInstruments_;
        }

        [[nodiscard]]
        std::uint64_t unknownOrderExecutions() const noexcept
        {
            return unknownOrderExecutions_;
        }

        [[nodiscard]]
        std::uint64_t overExecutedOrders() const noexcept
        {
            return overExecutedOrders_;
        }

        [[nodiscard]]
        std::uint64_t unknownOrderExecutionsWithPrice() const noexcept
        {
            return unknownOrderExecutionsWithPrice_;
        }

        [[nodiscard]]
        std::uint64_t overExecutedOrdersWithPrice() const noexcept
        {
            return overExecutedOrdersWithPrice_;
        }

        [[nodiscard]]
        std::uint64_t unknownOrderCancels() const noexcept
        {
            return unknownOrderCancels_;
        }

        [[nodiscard]]
        std::uint64_t overCancelledOrders() const noexcept
        {
            return overCancelledOrders_;
        }

        [[nodiscard]]
        std::uint64_t unknownOrderDeletes() const noexcept
        {
            return unknownOrderDeletes_;
        }

        [[nodiscard]]
        std::uint64_t unknownOrderReplaces() const noexcept
        {
            return unknownOrderReplaces_;
        }

        [[nodiscard]]
        std::uint64_t duplicateReplacementOrderIds() const noexcept
        {
            return duplicateReplacementOrderIds_;
        }

        [[nodiscard]]
        std::uint64_t duplicateAddOrders() const noexcept
        {
            return duplicateAddOrders_;
        }

        [[nodiscard]]
        market_data::BookStore &books() noexcept
        {
            return books_;
        }

        [[nodiscard]]
        const market_data::BookStore &books() const noexcept
        {
            return books_;
        }

        [[nodiscard]]
        std::uint64_t missingBooks() const noexcept
        {
            return missingBooks_;
        }

        [[nodiscard]]
        std::uint64_t failedBookReductions() const noexcept
        {
            return failedBookReductions_;
        }

        [[nodiscard]]
        std::uint64_t failedBookRemovals() const noexcept
        {
            return failedBookRemovals_;
        }

    private:
        void handle(
            const StockDirectoryMessage &message) noexcept
        {
            const auto instrument =
                ItchNormalizer::normalize(message);

            instruments_.add(instrument);
        }

        void handle(
            const StockTradingActionMessage &message) noexcept
        {
            auto *instrument =
                instruments_.find(
                    message.stockLocate);

            if (instrument == nullptr)
            {
                ++unknownTradingActionInstruments_;
                return;
            }

            instrument->tradingState =
                ItchNormalizer::normalizeTradingState(
                    message);
        }

        void handle(
            const RegShoRestrictionMessage &message) noexcept
        {
            auto *instrument =
                instruments_.find(
                    message.stockLocate);

            if (instrument == nullptr)
            {
                ++unknownRegShoInstruments_;
                return;
            }

            instrument->regShoState =
                ItchNormalizer::normalizeRegShoState(
                    message);
        }

        void handle(
            const AddOrderMessage &message)
        {
            const auto order =
                ItchNormalizer::normalizeOrder(
                    message);

            if (!orders_.add(order))
            {
                ++duplicateAddOrders_;
                return;
            }

            auto &book =
                books_.getOrCreate(
                    order.instrumentId);

            book.side(
                    order.side)
                .add(
                    order.price,
                    order.quantity);
        }

        void handle(
            const AddOrderWithMpidMessage &message)
        {
            const auto order =
                ItchNormalizer::normalizeOrder(
                    message);

            if (!orders_.add(order))
            {
                ++duplicateAddOrders_;
                return;
            }

            auto &book =
                books_.getOrCreate(
                    order.instrumentId);

            book.side(
                    order.side)
                .add(
                    order.price,
                    order.quantity);
        }

        void handle(
            const OrderExecutedMessage &message)
        {
            auto *order =
                orders_.find(
                    message.orderReferenceNumber);

            if (order == nullptr)
            {
                ++unknownOrderExecutions_;
                return;
            }

            const auto executed =
                static_cast<market_data::Quantity>(
                    message.executedShares);

            if (executed > order->quantity)
            {
                ++overExecutedOrders_;
                return;
            }

            auto *book =
                books_.find(
                    order->instrumentId);

            if (book == nullptr)
            {
                ++missingBooks_;
                return;
            }

            auto &side =
                book->side(
                    order->side);

            if (!side.reduce(
                    order->price,
                    executed))
            {
                ++failedBookReductions_;
                return;
            }

            if (executed == order->quantity)
            {
                // reduce() already removed all quantity.
                // removeOrder(..., 0) removes the order count
                // and erases the now-empty price level.
                if (!side.removeOrder(
                        order->price,
                        0))
                {
                    ++failedBookRemovals_;
                    return;
                }

                orders_.remove(
                    message.orderReferenceNumber);

                return;
            }

            order->quantity -= executed;
        }

        void handle(
            const OrderExecutedWithPriceMessage &message)
        {
            auto *order =
                orders_.find(
                    message.orderReferenceNumber);

            if (order == nullptr)
            {
                ++unknownOrderExecutionsWithPrice_;
                return;
            }

            const auto executed =
                static_cast<market_data::Quantity>(
                    message.executedShares);

            if (executed > order->quantity)
            {
                ++overExecutedOrdersWithPrice_;
                return;
            }

            auto *book =
                books_.find(
                    order->instrumentId);

            if (book == nullptr)
            {
                ++missingBooks_;
                return;
            }

            auto &side =
                book->side(
                    order->side);

            if (!side.reduce(
                    order->price,
                    executed))
            {
                ++failedBookReductions_;
                return;
            }

            if (executed == order->quantity)
            {
                if (!side.removeOrder(
                        order->price,
                        0))
                {
                    ++failedBookRemovals_;
                    return;
                }

                orders_.remove(
                    message.orderReferenceNumber);

                return;
            }

            order->quantity -= executed;
        }

        void handle(
            const OrderCancelMessage &message)
        {
            auto *order =
                orders_.find(
                    message.orderReferenceNumber);

            if (order == nullptr)
            {
                ++unknownOrderCancels_;
                return;
            }

            const auto cancelled =
                static_cast<market_data::Quantity>(
                    message.cancelledShares);

            if (cancelled > order->quantity)
            {
                ++overCancelledOrders_;
                return;
            }

            auto *book =
                books_.find(
                    order->instrumentId);

            if (book == nullptr)
            {
                ++missingBooks_;
                return;
            }

            auto &side =
                book->side(
                    order->side);

            if (!side.reduce(
                    order->price,
                    cancelled))
            {
                ++failedBookReductions_;
                return;
            }

            if (cancelled == order->quantity)
            {
                if (!side.removeOrder(
                        order->price,
                        0))
                {
                    ++failedBookRemovals_;
                    return;
                }

                orders_.remove(
                    message.orderReferenceNumber);

                return;
            }

            order->quantity -= cancelled;
        }

        void handle(
            const OrderDeleteMessage &message)
        {
            auto *order =
                orders_.find(
                    message.orderReferenceNumber);

            if (order == nullptr)
            {
                ++unknownOrderDeletes_;
                return;
            }

            auto *book =
                books_.find(
                    order->instrumentId);

            if (book == nullptr)
            {
                ++missingBooks_;
                return;
            }

            auto &side =
                book->side(
                    order->side);

            if (!side.removeOrder(
                    order->price,
                    order->quantity))
            {
                ++failedBookRemovals_;
                return;
            }

            orders_.remove(
                message.orderReferenceNumber);
        }

        void handle(
            const OrderReplaceMessage &message)
        {
            auto *existing =
                orders_.find(
                    message.originalOrderReferenceNumber);

            if (existing == nullptr)
            {
                ++unknownOrderReplaces_;
                return;
            }

            if (
                message.newOrderReferenceNumber !=
                    message.originalOrderReferenceNumber &&
                orders_.contains(
                    message.newOrderReferenceNumber))
            {
                ++duplicateReplacementOrderIds_;
                return;
            }

            // Save the old state before removing anything.
            const auto instrumentId =
                existing->instrumentId;

            const auto sideValue =
                existing->side;

            const auto oldPrice =
                existing->price;

            const auto oldQuantity =
                existing->quantity;

            market_data::Order replacement{
                .orderId =
                    message.newOrderReferenceNumber,

                .instrumentId =
                    instrumentId,

                .timestamp =
                    message.timestamp,

                .price =
                    message.price,

                .quantity =
                    message.shares,

                .side =
                    sideValue};

            auto *book =
                books_.find(
                    instrumentId);

            if (book == nullptr)
            {
                ++missingBooks_;
                return;
            }

            auto &side =
                book->side(
                    sideValue);

            if (!side.removeOrder(
                    oldPrice,
                    oldQuantity))
            {
                ++failedBookRemovals_;
                return;
            }

            side.add(
                replacement.price,
                replacement.quantity);

            orders_.remove(
                message.originalOrderReferenceNumber);

            if (!orders_.add(replacement))
            {
                ++duplicateReplacementOrderIds_;
            }
        }

        // All other ITCH messages are intentionally
        // ignored at this stage.
        template <typename T>
        void handle(
            const T &) noexcept
        {
        }

        std::uint64_t unknownTradingActionInstruments_{0};
        std::uint64_t unknownRegShoInstruments_{0};

        market_data::InstrumentStore instruments_;
        market_data::OrderStore orders_;

        std::uint64_t unknownOrderExecutions_{0};
        std::uint64_t overExecutedOrders_{0};

        std::uint64_t unknownOrderExecutionsWithPrice_{0};
        std::uint64_t overExecutedOrdersWithPrice_{0};

        std::uint64_t unknownOrderCancels_{0};
        std::uint64_t overCancelledOrders_{0};

        std::uint64_t unknownOrderDeletes_{0};

        std::uint64_t unknownOrderReplaces_{0};
        std::uint64_t duplicateReplacementOrderIds_{0};

        std::uint64_t duplicateAddOrders_{0};

        market_data::BookStore books_;

        std::uint64_t missingBooks_{0};
        std::uint64_t failedBookReductions_{0};
        std::uint64_t failedBookRemovals_{0};
    };

} // namespace llt::itch