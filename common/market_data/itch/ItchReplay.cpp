#include "market_data/itch/ItchReplay.h"
#include <type_traits>
#include <variant>

#include "market_data/itch/ItchDispatcher.h"
#include "market_data/itch/ItchMessageType.h"
#include "market_data/itch/ItchStreamReader.h"

#include "market_data/itch/messages/SystemEventMessage.h"
#include "market_data/itch/messages/StockDirectoryMessage.h"
#include "market_data/itch/messages/AddOrderMessage.h"
#include "market_data/itch/messages/OrderExecutedMessage.h"
#include "market_data/itch/messages/OrderExecutedWithPriceMessage.h"
#include "market_data/itch/messages/OrderCancelMessage.h"
#include "market_data/itch/messages/OrderDeleteMessage.h"
#include "market_data/itch/messages/OrderReplaceMessage.h"

namespace llt::itch
{

    namespace
    {

        bool isSupportedMessageType(
            std::uint8_t type) noexcept
        {
            switch (
                static_cast<ItchMessageType>(type))
            {
            case ItchMessageType::SystemEvent:
            case ItchMessageType::StockDirectory:
            case ItchMessageType::AddOrder:
            case ItchMessageType::AddOrderWithMpid:
            case ItchMessageType::OrderExecuted:
            case ItchMessageType::OrderExecutedWithPrice:
            case ItchMessageType::OrderCancel:
            case ItchMessageType::OrderDelete:
            case ItchMessageType::OrderReplace:
            case ItchMessageType::Trade:
            case ItchMessageType::CrossTrade:
            case ItchMessageType::BrokenTrade:
            case ItchMessageType::StockTradingAction:
            case ItchMessageType::MarketParticipantPosition:
            case ItchMessageType::RegShoRestriction:
            case ItchMessageType::MwcbDeclineLevel:
            case ItchMessageType::MwcbStatus:
            case ItchMessageType::Noii:
            case ItchMessageType::LuldAuctionCollar:
                return true;
            }

            return false;
        }

        void countDecodedMessage(
            const ItchMessage &message,
            ItchReplayStats &stats) noexcept
        {
            std::visit(
                [&stats](const auto &decoded)
                {
                    using T =
                        std::decay_t<
                            decltype(decoded)>;

                    if constexpr (
                        std::is_same_v<
                            T,
                            SystemEventMessage>)
                    {
                        ++stats.systemEvents;
                    }
                    else if constexpr (
                        std::is_same_v<
                            T,
                            StockDirectoryMessage>)
                    {
                        ++stats.stockDirectories;
                    }
                    else if constexpr (
                        std::is_same_v<
                            T,
                            AddOrderMessage>)
                    {
                        ++stats.addOrders;
                    }
                    else if constexpr (
                        std::is_same_v<
                            T,
                            AddOrderWithMpidMessage>)
                    {
                        ++stats.addOrdersWithMpid;
                    }
                    else if constexpr (
                        std::is_same_v<
                            T,
                            OrderExecutedMessage>)
                    {
                        ++stats.orderExecutions;
                    }
                    else if constexpr (
                        std::is_same_v<
                            T,
                            OrderExecutedWithPriceMessage>)
                    {
                        ++stats.orderExecutionsWithPrice;
                    }
                    else if constexpr (
                        std::is_same_v<
                            T,
                            OrderCancelMessage>)
                    {
                        ++stats.orderCancels;
                    }
                    else if constexpr (
                        std::is_same_v<
                            T,
                            OrderDeleteMessage>)
                    {
                        ++stats.orderDeletes;
                    }
                    else if constexpr (
                        std::is_same_v<
                            T,
                            OrderReplaceMessage>)
                    {
                        ++stats.orderReplaces;
                    }
                    else if constexpr (
                        std::is_same_v<
                            T,
                            TradeMessage>)
                    {
                        ++stats.trades;
                    }
                    else if constexpr (
                        std::is_same_v<
                            T,
                            CrossTradeMessage>)
                    {
                        ++stats.crossTrades;
                    }
                    else if constexpr (
                        std::is_same_v<
                            T,
                            BrokenTradeMessage>)
                    {
                        ++stats.brokenTrades;
                    }
                    else if constexpr (
                        std::is_same_v<
                            T,
                            StockTradingActionMessage>)
                    {
                        ++stats.stockTradingActions;
                    }
                    else if constexpr (
                        std::is_same_v<
                            T,
                            MarketParticipantPositionMessage>)
                    {
                        ++stats.marketParticipantPositions;
                    }
                    else if constexpr (
                        std::is_same_v<T, RegShoRestrictionMessage>)
                    {
                        ++stats.regShoRestrictions;
                    }
                    else if constexpr (
                        std::is_same_v<T, MwcbDeclineLevelMessage>)
                    {
                        ++stats.mwcbDeclineLevels;
                    }
                    else if constexpr (
                        std::is_same_v<T, MwcbStatusMessage>)
                    {
                        ++stats.mwcbStatuses;
                    }
                    else if constexpr (
                        std::is_same_v<T, NoiiMessage>)
                    {
                        ++stats.noiiMessages;
                    }
                    else if constexpr (
                        std::is_same_v<
                            T,
                            LuldAuctionCollarMessage
                        >
                    )
                    {
                        ++stats.luldAuctionCollars;
                    }
                },
                message);
        }

    } // namespace

    ItchReplayResult ItchReplay::run(
        std::istream& input,
        const MessageHandler& handler
    )
    {
        ItchReplayResult result{};
    
        ItchStreamReader reader(input);
    
        while (true)
        {
            auto record =
                reader.readNext();
    
            switch (record.status)
            {
            case ItchStreamReadStatus::Message:
            {
                ++result.stats.recordsRead;
    
                if (record.payload.empty())
                {
                    ++result.stats.malformedMessages;
                    continue;
                }
    
                const std::uint8_t type =
                    record.payload[0];
    
                if (!isSupportedMessageType(type))
                {
                    ++result.stats.unsupportedMessages;
    
                    ++result.stats.unsupportedByType[
                        type
                    ];
    
                    continue;
                }
    
                auto decoded =
                    ItchDispatcher::dispatch(
                        record.payload.data(),
                        record.payload.size()
                    );
    
                if (!decoded.has_value())
                {
                    ++result.stats.malformedMessages;
                    continue;
                }
    
                ++result.stats.decodedMessages;
    
                countDecodedMessage(
                    *decoded,
                    result.stats
                );
    
                // Deliver only successfully decoded
                // messages to downstream consumers.
                if (handler)
                {
                    handler(*decoded);
                }
    
                break;
            }
    
            case ItchStreamReadStatus::EndOfSession:
            {
                result.stats.sessionComplete = true;
    
                result.status =
                    ItchReplayStatus::Complete;
    
                return result;
            }
    
            case ItchStreamReadStatus::Incomplete:
            {
                result.status =
                    ItchReplayStatus::IncompleteStream;
    
                return result;
            }
    
            case ItchStreamReadStatus::Error:
            {
                result.status =
                    ItchReplayStatus::StreamError;
    
                return result;
            }
            }
        }
    }
} // namespace llt::itch