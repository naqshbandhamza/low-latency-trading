#include "market_data/itch/ItchDispatcher.h"

#include "market_data/itch/ItchDecoder.h"
#include "market_data/itch/ItchMessageType.h"
#include <utility>

namespace llt::itch
{

    std::optional<ItchMessage>
    ItchDispatcher::dispatch(
        const std::uint8_t *data,
        std::size_t size) noexcept
    {
        if (data == nullptr || size == 0)
        {
            return std::nullopt;
        }

        const auto type =
            static_cast<ItchMessageType>(
                data[0]);

        switch (type)
        {
        case ItchMessageType::SystemEvent:
        {
            SystemEventMessage message{};

            if (
                !ItchDecoder::decodeSystemEvent(
                    data,
                    size,
                    message))
            {
                return std::nullopt;
            }

            return ItchMessage{
                std::move(message)};
        }

        case ItchMessageType::StockDirectory:
        {
            StockDirectoryMessage message{};

            if (
                !ItchDecoder::decodeStockDirectory(
                    data,
                    size,
                    message))
            {
                return std::nullopt;
            }

            return ItchMessage{
                std::move(message)};
        }

        case ItchMessageType::AddOrder:
        {
            AddOrderMessage message{};

            if (
                !ItchDecoder::decodeAddOrder(
                    data,
                    size,
                    message))
            {
                return std::nullopt;
            }

            return ItchMessage{
                std::move(message)};
        }

        case ItchMessageType::OrderExecuted:
        {
            OrderExecutedMessage message{};

            if (
                !ItchDecoder::decodeOrderExecuted(
                    data,
                    size,
                    message))
            {
                return std::nullopt;
            }

            return ItchMessage{
                std::move(message)};
        }

        case ItchMessageType::OrderExecutedWithPrice:
        {
            OrderExecutedWithPriceMessage message{};

            if (
                !ItchDecoder::
                    decodeOrderExecutedWithPrice(
                        data,
                        size,
                        message))
            {
                return std::nullopt;
            }

            return ItchMessage{
                std::move(message)};
        }

        case ItchMessageType::OrderCancel:
        {
            OrderCancelMessage message{};

            if (
                !ItchDecoder::decodeOrderCancel(
                    data,
                    size,
                    message))
            {
                return std::nullopt;
            }

            return ItchMessage{
                std::move(message)};
        }

        case ItchMessageType::OrderDelete:
        {
            OrderDeleteMessage message{};

            if (
                !ItchDecoder::decodeOrderDelete(
                    data,
                    size,
                    message))
            {
                return std::nullopt;
            }

            return ItchMessage{
                std::move(message)};
        }

        case ItchMessageType::OrderReplace:
        {
            OrderReplaceMessage message{};

            if (
                !ItchDecoder::decodeOrderReplace(
                    data,
                    size,
                    message))
            {
                return std::nullopt;
            }

            return ItchMessage{
                std::move(message)};
        }

        case ItchMessageType::AddOrderWithMpid:
        {
            AddOrderWithMpidMessage message;

            if (!ItchDecoder::decodeAddOrderWithMpid(
                    data,
                    size,
                    message))
            {
                return std::nullopt;
            }

            return ItchMessage{
                std::move(message)};
        }

        case ItchMessageType::Trade:
        {
            TradeMessage message;

            if (!ItchDecoder::decodeTrade(
                    data,
                    size,
                    message))
            {
                return std::nullopt;
            }

            return ItchMessage{
                std::move(message)};
        }

        case ItchMessageType::CrossTrade:
        {
            CrossTradeMessage message;

            if (!ItchDecoder::decodeCrossTrade(
                    data,
                    size,
                    message))
            {
                return std::nullopt;
            }

            return ItchMessage{
                std::move(message)};
        }

        case ItchMessageType::BrokenTrade:
        {
            BrokenTradeMessage message;

            if (!ItchDecoder::decodeBrokenTrade(
                    data,
                    size,
                    message))
            {
                return std::nullopt;
            }

            return ItchMessage{
                std::move(message)};
        }

        case ItchMessageType::StockTradingAction:
        {
            StockTradingActionMessage message;

            if (!ItchDecoder::decodeStockTradingAction(
                    data,
                    size,
                    message))
            {
                return std::nullopt;
            }

            return ItchMessage{
                std::move(message)};
        }

        case ItchMessageType::MarketParticipantPosition:
        {
            MarketParticipantPositionMessage message;

            if (!ItchDecoder::decodeMarketParticipantPosition(
                    data,
                    size,
                    message))
            {
                return std::nullopt;
            }

            return ItchMessage{
                std::move(message)};
        }

        case ItchMessageType::RegShoRestriction:
        {
            RegShoRestrictionMessage message;

            if (!ItchDecoder::decodeRegShoRestriction(
                    data, size, message))
                return std::nullopt;

            return ItchMessage{std::move(message)};
        }

        case ItchMessageType::MwcbDeclineLevel:
        {
            MwcbDeclineLevelMessage message;

            if (!ItchDecoder::decodeMwcbDeclineLevel(
                    data, size, message))
                return std::nullopt;

            return ItchMessage{std::move(message)};
        }

        case ItchMessageType::MwcbStatus:
        {
            MwcbStatusMessage message;

            if (!ItchDecoder::decodeMwcbStatus(
                    data, size, message))
                return std::nullopt;

            return ItchMessage{std::move(message)};
        }

        case ItchMessageType::Noii:
        {
            NoiiMessage message;

            if (!ItchDecoder::decodeNoii(
                    data, size, message))
                return std::nullopt;

            return ItchMessage{std::move(message)};
        }

        case ItchMessageType::LuldAuctionCollar:
        {
            LuldAuctionCollarMessage message;

            if (!ItchDecoder::decodeLuldAuctionCollar(
                    data,
                    size,
                    message))
            {
                return std::nullopt;
            }

            return ItchMessage{
                std::move(message)};
        }
        }

        return std::nullopt;
    }

} // namespace llt::itch