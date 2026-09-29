#pragma once

#include <iostream>
#include <cstdint>
#include "market_data/itch/ItchMessage.h"
#include "market_data/itch/ItchNormalizer.h"
#include "market_data/normalized/InstrumentStore.h"
#include "market_data/normalized/OrderStore.h"

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

        // void handle(
        //     const StockDirectoryMessage& message
        // ) noexcept
        // {
        //     const auto instrument =
        //         ItchNormalizer::normalize(message);

        //     if (!instruments_.add(instrument))
        //     {
        //         const auto* existing =
        //             instruments_.find(instrument.id);

        //         std::cerr
        //             << "\n[DUPLICATE STOCK DIRECTORY]\n"
        //             << "Stock Locate : "
        //             << instrument.id
        //             << '\n'
        //             << "New Symbol   : "
        //             << instrument.symbolView()
        //             << '\n';

        //         if (existing != nullptr)
        //         {
        //             std::cerr
        //                 << "Old Symbol   : "
        //                 << existing->symbolView()
        //                 << '\n';
        //         }
        //     }
        // }

        void handle(
            const AddOrderMessage &message)
        {
            const auto order =
                ItchNormalizer::normalizeOrder(
                    message);

            if (!orders_.add(order))
            {
                ++duplicateAddOrders_;
            }
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
            }
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

            if (executed == order->quantity)
            {
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

            if (executed == order->quantity)
            {
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

            if (cancelled == order->quantity)
            {
                orders_.remove(
                    message.orderReferenceNumber);

                return;
            }

            order->quantity -= cancelled;
        }

        void handle(
            const OrderDeleteMessage &message)
        {
            if (!orders_.remove(
                    message.orderReferenceNumber))
            {
                ++unknownOrderDeletes_;
            }
        }

        void handle(
            const OrderReplaceMessage &message)
        {
            const auto *existing =
                orders_.find(
                    message.originalOrderReferenceNumber);

            if (existing == nullptr)
            {
                ++unknownOrderReplaces_;
                return;
            }

            // Capture inherited state BEFORE removing the old order.
            const auto instrumentId =
                existing->instrumentId;

            const auto side =
                existing->side;

            const auto timestamp =
                message.timestamp;

            market_data::Order replacement{
                .orderId =
                    message.newOrderReferenceNumber,

                .instrumentId =
                    instrumentId,

                .timestamp =
                    timestamp,

                .price =
                    message.price,

                .quantity =
                    message.shares,

                .side =
                    side};

            // Protect the old order if the replacement ID
            // unexpectedly already exists.
            if (
                message.newOrderReferenceNumber !=
                    message.originalOrderReferenceNumber &&
                orders_.contains(
                    message.newOrderReferenceNumber))
            {
                ++duplicateReplacementOrderIds_;
                return;
            }

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
    };

} // namespace llt::itch