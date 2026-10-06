#pragma once

#include <iostream>
#include <cstdint>
#include <utility>
#include "market_data/itch/ItchMessage.h"
#include "market_data/itch/ItchNormalizer.h"
#include "market_data/normalized/InstrumentStore.h"
#include "market_data/normalized/OrderStore.h"
#include "market_data/book/BookStore.h"
#include <functional>

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

        // [[nodiscard]]
        // market_data::InstrumentStore &
        // instruments() noexcept
        // {
        //     return instruments_;
        // }

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

        [[nodiscard]]
        bool hasBboChangeHandler() const noexcept
        {
            return static_cast<bool>(bboChangeHandler_);
        }

        using BboChangeHandler =
            std::function<void(
                market_data::InstrumentId,
                market_data::Timestamp,
                const market_data::Bbo &)>;

        using TradeHandler =
            std::function<void(
                market_data::InstrumentId,
                market_data::Timestamp,
                market_data::Price,
                market_data::Quantity,
                market_data::Side)>;

        explicit ItchMarketState(
            BboChangeHandler handler = {})
            : bboChangeHandler_(
                  std::move(handler))
        {
        }

        void setBboChangeHandler(
            BboChangeHandler handler)
        {
            bboChangeHandler_ =
                std::move(handler);
        }

        void setTradeHandler(
            TradeHandler handler)
        {
            tradeHandler_ =
                std::move(handler);
        }

        [[nodiscard]]
        std::uint64_t bookSideRemoveCalls() const noexcept
        {
            std::uint64_t total = 0;

            books_.forEach(
                [&total](
                    const auto,
                    const auto &book)
                {
                    total +=
                        book.bids().removeCalls();

                    total +=
                        book.asks().removeCalls();
                });

            return total;
        }

        [[nodiscard]]
        std::uint64_t priceLevelEraseCount() const noexcept
        {
            std::uint64_t total = 0;

            books_.forEach(
                [&total](
                    const auto,
                    const auto &book)
                {
                    total +=
                        book.bids().levelEraseCount();

                    total +=
                        book.asks().levelEraseCount();
                });

            return total;
        }

    private:
        void notifyBboChange(
            market_data::InstrumentId instrumentId,
            market_data::Timestamp timestamp,
            const market_data::Bbo &before,
            const market_data::Bbo &after)
        {
            if (
                before == after ||
                !bboChangeHandler_)
            {
                return;
            }

            bboChangeHandler_(
                instrumentId,
                timestamp,
                after);
        }

        void notifyTrade(
            market_data::InstrumentId instrumentId,
            market_data::Timestamp timestamp,
            market_data::Price price,
            market_data::Quantity quantity,
            market_data::Side side)
        {
            if (!tradeHandler_)
            {
                return;
            }

            tradeHandler_(
                instrumentId,
                timestamp,
                price,
                quantity,
                side);
        }

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

            if (orders_.contains(
                    order.orderId))
            {
                ++duplicateAddOrders_;
                return;
            }

            auto &book =
                books_.getOrCreate(
                    order.instrumentId);

            const bool hasHandler =
                hasBboChangeHandler();

            market_data::Bbo before{};

            if (hasHandler)
            {
                before =
                    book.bbo();
            }

            auto &side =
                book.side(
                    order.side);

            auto level =
                side.add(
                    order.price,
                    order.quantity);

            if (!orders_.add(
                    order,
                    level))
            {
                ++duplicateAddOrders_;
                return;
            }

            if (hasHandler)
            {
                const auto after =
                    book.bbo();

                notifyBboChange(
                    order.instrumentId,
                    message.timestamp,
                    before,
                    after);
            }
        }

        void handle(
            const AddOrderWithMpidMessage &message)
        {
            const auto order =
                ItchNormalizer::normalizeOrder(
                    message);

            if (orders_.contains(
                    order.orderId))
            {
                ++duplicateAddOrders_;
                return;
            }

            auto &book =
                books_.getOrCreate(
                    order.instrumentId);

            const bool hasHandler =
                hasBboChangeHandler();

            market_data::Bbo before{};

            if (hasHandler)
            {
                before =
                    book.bbo();
            }

            auto &side =
                book.side(
                    order.side);

            auto level =
                side.add(
                    order.price,
                    order.quantity);

            if (!orders_.add(
                    order,
                    level))
            {
                ++duplicateAddOrders_;
                return;
            }

            if (hasHandler)
            {
                const auto after =
                    book.bbo();

                notifyBboChange(
                    order.instrumentId,
                    message.timestamp,
                    before,
                    after);
            }
        }

        void handle(
            const OrderExecutedMessage &message)
        {
            auto orderIt =
                orders_.findIterator(
                    message.orderReferenceNumber);

            if (orderIt == orders_.end())
            {
                ++unknownOrderExecutions_;
                return;
            }

            auto &stored =
                orders_.stored(orderIt);

            auto &order =
                stored.order;

            const auto executed =
                static_cast<market_data::Quantity>(
                    message.executedShares);

            if (executed > order.quantity)
            {
                ++overExecutedOrders_;
                return;
            }

            auto *book =
                books_.find(
                    order.instrumentId);

            if (book == nullptr)
            {
                ++missingBooks_;
                return;
            }

            notifyTrade(
                order.instrumentId,
                message.timestamp,
                order.price,
                executed,
                order.side);

            const bool hasHandler =
                hasBboChangeHandler();

            market_data::Bbo before{};

            if (hasHandler)
            {
                before =
                    book->bbo();
            }

            auto &side =
                book->side(
                    order.side);

            // Directly mutate the known price level.
            if (!side.reduce(
                    stored.level,
                    executed))
            {
                ++failedBookReductions_;
                return;
            }

            if (executed == order.quantity)
            {
                const auto instrumentId =
                    order.instrumentId;

                // The quantity has already been reduced to zero.
                // removeOrder() now removes the final order and,
                // if empty, erases the level through its iterator.
                if (!side.removeOrder(
                        stored.level,
                        0))
                {
                    ++failedBookRemovals_;
                    return;
                }

                // stored.level may now be invalid.
                // order/stored remain valid until this erase.
                orders_.removeAt(
                    orderIt);

                if (hasHandler)
                {
                    const auto after =
                        book->bbo();

                    notifyBboChange(
                        instrumentId,
                        message.timestamp,
                        before,
                        after);
                }

                return;
            }

            order.quantity -=
                executed;

            if (hasHandler)
            {
                const auto after =
                    book->bbo();

                notifyBboChange(
                    order.instrumentId,
                    message.timestamp,
                    before,
                    after);
            }
        }

        void handle(
            const OrderExecutedWithPriceMessage &message)
        {
            auto orderIt =
                orders_.findIterator(
                    message.orderReferenceNumber);

            if (orderIt == orders_.end())
            {
                ++unknownOrderExecutionsWithPrice_;
                return;
            }

            auto &stored =
                orders_.stored(orderIt);

            auto &order =
                stored.order;

            const auto executed =
                static_cast<market_data::Quantity>(
                    message.executedShares);

            if (executed > order.quantity)
            {
                ++overExecutedOrdersWithPrice_;
                return;
            }

            auto *book =
                books_.find(
                    order.instrumentId);

            if (book == nullptr)
            {
                ++missingBooks_;
                return;
            }

            // Execution price comes from the ITCH message here,
            // while the resting book level remains order.price.
            notifyTrade(
                order.instrumentId,
                message.timestamp,
                message.executionPrice,
                executed,
                order.side);

            const bool hasHandler =
                hasBboChangeHandler();

            market_data::Bbo before{};

            if (hasHandler)
            {
                before =
                    book->bbo();
            }

            auto &side =
                book->side(
                    order.side);

            if (!side.reduce(
                    stored.level,
                    executed))
            {
                ++failedBookReductions_;
                return;
            }

            if (executed == order.quantity)
            {
                const auto instrumentId =
                    order.instrumentId;

                if (!side.removeOrder(
                        stored.level,
                        0))
                {
                    ++failedBookRemovals_;
                    return;
                }

                orders_.removeAt(
                    orderIt);

                if (hasHandler)
                {
                    const auto after =
                        book->bbo();

                    notifyBboChange(
                        instrumentId,
                        message.timestamp,
                        before,
                        after);
                }

                return;
            }

            order.quantity -=
                executed;

            if (hasHandler)
            {
                const auto after =
                    book->bbo();

                notifyBboChange(
                    order.instrumentId,
                    message.timestamp,
                    before,
                    after);
            }
        }

        void handle(
            const OrderCancelMessage &message)
        {
            auto orderIt =
                orders_.findIterator(
                    message.orderReferenceNumber);

            if (orderIt == orders_.end())
            {
                ++unknownOrderCancels_;
                return;
            }

            auto &stored =
                orders_.stored(orderIt);

            auto &order =
                stored.order;

            const auto cancelled =
                static_cast<market_data::Quantity>(
                    message.cancelledShares);

            if (cancelled > order.quantity)
            {
                ++overCancelledOrders_;
                return;
            }

            auto *book =
                books_.find(
                    order.instrumentId);

            if (book == nullptr)
            {
                ++missingBooks_;
                return;
            }

            const bool hasHandler =
                hasBboChangeHandler();

            market_data::Bbo before{};

            if (hasHandler)
            {
                before =
                    book->bbo();
            }

            auto &side =
                book->side(
                    order.side);

            if (!side.reduce(
                    stored.level,
                    cancelled))
            {
                ++failedBookReductions_;
                return;
            }

            if (cancelled == order.quantity)
            {
                const auto instrumentId =
                    order.instrumentId;

                if (!side.removeOrder(
                        stored.level,
                        0))
                {
                    ++failedBookRemovals_;
                    return;
                }

                orders_.removeAt(
                    orderIt);

                if (hasHandler)
                {
                    const auto after =
                        book->bbo();

                    notifyBboChange(
                        instrumentId,
                        message.timestamp,
                        before,
                        after);
                }

                return;
            }

            order.quantity -=
                cancelled;

            if (hasHandler)
            {
                const auto after =
                    book->bbo();

                notifyBboChange(
                    order.instrumentId,
                    message.timestamp,
                    before,
                    after);
            }
        }

        void handle(
            const OrderDeleteMessage &message)
        {
            auto orderIt =
                orders_.findIterator(
                    message.orderReferenceNumber);

            if (orderIt == orders_.end())
            {
                ++unknownOrderDeletes_;
                return;
            }

            auto &stored =
                orders_.stored(orderIt);

            auto &order =
                stored.order;

            auto *book =
                books_.find(
                    order.instrumentId);

            if (book == nullptr)
            {
                ++missingBooks_;
                return;
            }

            const bool hasHandler =
                hasBboChangeHandler();

            market_data::Bbo before{};

            if (hasHandler)
            {
                before =
                    book->bbo();
            }

            auto &side =
                book->side(
                    order.side);

            // Must save anything needed after OrderStore erase.
            const auto instrumentId =
                order.instrumentId;

            if (!side.removeOrder(
                    stored.level,
                    order.quantity))
            {
                ++failedBookRemovals_;
                return;
            }

            // The level iterator may now be invalid.
            // Do not touch stored.level after this point.

            orders_.removeAt(
                orderIt);

            // order/stored are now invalid too.

            if (hasHandler)
            {
                const auto after =
                    book->bbo();

                notifyBboChange(
                    instrumentId,
                    message.timestamp,
                    before,
                    after);
            }
        }

        void handle(
            const OrderReplaceMessage &message)
        {
            auto orderIt =
                orders_.findIterator(
                    message.originalOrderReferenceNumber);

            if (orderIt == orders_.end())
            {
                ++unknownOrderReplaces_;
                return;
            }

            auto &stored =
                orders_.stored(orderIt);

            auto &order =
                stored.order;

            if (
                message.newOrderReferenceNumber !=
                    message.originalOrderReferenceNumber &&
                orders_.contains(
                    message.newOrderReferenceNumber))
            {
                ++duplicateReplacementOrderIds_;
                return;
            }

            // Copy everything needed before either the level
            // or OrderStore entry can be erased.
            const auto instrumentId =
                order.instrumentId;

            const auto sideValue =
                order.side;

            const auto oldQuantity =
                order.quantity;

            const market_data::Order replacement{
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

            const bool hasHandler =
                hasBboChangeHandler();

            market_data::Bbo before{};

            if (hasHandler)
            {
                before =
                    book->bbo();
            }

            auto &side =
                book->side(
                    sideValue);

            // Remove old order directly through its level handle.
            if (!side.removeOrder(
                    stored.level,
                    oldQuantity))
            {
                ++failedBookRemovals_;
                return;
            }

            // stored.level may now be invalid.

            orders_.removeAt(
                orderIt);

            // Create/find the replacement's price level and retain
            // its NEW handle.
            auto newLevel =
                side.add(
                    replacement.price,
                    replacement.quantity);

            if (!orders_.add(
                    replacement,
                    newLevel))
            {
                ++duplicateReplacementOrderIds_;
                return;
            }

            if (hasHandler)
            {
                const auto after =
                    book->bbo();

                notifyBboChange(
                    instrumentId,
                    message.timestamp,
                    before,
                    after);
            }
        }

        void handle(
            const TradeMessage &message)
        {
            const auto side =
                message.buySellIndicator == 'B'
                    ? market_data::Side::Buy
                    : market_data::Side::Sell;

            notifyTrade(
                message.stockLocate,
                message.timestamp,
                message.price,
                message.shares,
                side);
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

        BboChangeHandler bboChangeHandler_;
        TradeHandler tradeHandler_;
    };

} // namespace llt::itch