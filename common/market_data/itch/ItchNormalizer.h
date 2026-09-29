#pragma once

#include "market_data/itch/messages/StockDirectoryMessage.h"
#include "market_data/normalized/Instrument.h"
#include "market_data/itch/messages/StockTradingActionMessage.h"
#include "market_data/itch/messages/RegShoRestrictionMessage.h"

#include "market_data/itch/messages/AddOrderMessage.h"
#include "market_data/itch/messages/AddOrderWithMpidMessage.h"
#include "market_data/normalized/Order.h"

namespace llt::itch
{

    class ItchNormalizer
    {
    public:
        [[nodiscard]]
        static market_data::Instrument normalize(
            const StockDirectoryMessage &message) noexcept
        {
            market_data::Instrument instrument;

            // Nasdaq stockLocate becomes our compact
            // internal instrument identifier.
            instrument.id =
                message.stockLocate;

            instrument.symbol =
                message.stock;

            // Stock Directory establishes the instrument,
            // but does not itself tell us that continuous
            // trading is active.
            instrument.tradingState =
                market_data::TradingState::Unknown;

            return instrument;
        }

        [[nodiscard]]
        static market_data::TradingState normalizeTradingState(
            const StockTradingActionMessage &message) noexcept
        {
            switch (message.tradingState)
            {
            case 'H':
                return market_data::TradingState::Halted;

            case 'P':
                return market_data::TradingState::Paused;

            case 'Q':
                return market_data::TradingState::QuotationOnly;

            case 'T':
                return market_data::TradingState::Trading;

            default:
                return market_data::TradingState::Unknown;
            }
        }

        [[nodiscard]]
        static market_data::RegShoState normalizeRegShoState(
            const RegShoRestrictionMessage &message) noexcept
        {
            switch (message.regShoAction)
            {
            case '0':
                return market_data::RegShoState::NoRestriction;

            case '1':
                return market_data::RegShoState::RestrictionInEffect;

            case '2':
                return market_data::RegShoState::RestrictionRemains;

            default:
                return market_data::RegShoState::Unknown;
            }
        }

        [[nodiscard]]
        static market_data::Order normalizeOrder(
            const AddOrderMessage &message) noexcept
        {
            market_data::Order order;

            order.orderId =
                message.orderReferenceNumber;

            order.instrumentId =
                message.stockLocate;

            order.timestamp =
                message.timestamp;

            order.price =
                message.price;

            order.quantity =
                message.shares;

            order.side =
                message.buySellIndicator == 'B'
                    ? market_data::Side::Buy
                    : market_data::Side::Sell;

            return order;
        }

        [[nodiscard]]
        static market_data::Order normalizeOrder(
            const AddOrderWithMpidMessage &message) noexcept
        {
            market_data::Order order;

            order.orderId =
                message.orderReferenceNumber;

            order.instrumentId =
                message.stockLocate;

            order.timestamp =
                message.timestamp;

            order.price =
                message.price;

            order.quantity =
                message.shares;

            order.side =
                message.buySellIndicator == 'B'
                    ? market_data::Side::Buy
                    : market_data::Side::Sell;

            return order;
        }
    };

} // namespace llt::itch