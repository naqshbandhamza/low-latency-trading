#pragma once

#include <cstddef>
#include <cstdint>

#include "market_data/itch/messages/AddOrderMessage.h"
#include "market_data/itch/messages/StockDirectoryMessage.h"
#include "market_data/itch/messages/SystemEventMessage.h"
#include "market_data/itch/messages/OrderExecutedMessage.h"
#include "market_data/itch/messages/OrderExecutedWithPriceMessage.h"
#include "market_data/itch/messages/OrderCancelMessage.h"
#include "market_data/itch/messages/OrderDeleteMessage.h"
#include "market_data/itch/messages/OrderReplaceMessage.h"
#include "market_data/itch/messages/AddOrderWithMpidMessage.h"
#include "market_data/itch/messages/TradeMessage.h"
#include "market_data/itch/messages/CrossTradeMessage.h"
#include "market_data/itch/messages/BrokenTradeMessage.h"
#include "market_data/itch/messages/StockTradingActionMessage.h"
#include "market_data/itch/messages/MarketParticipantPositionMessage.h"
#include "market_data/itch/messages/RegShoRestrictionMessage.h"
#include "market_data/itch/messages/MwcbDeclineLevelMessage.h"
#include "market_data/itch/messages/MwcbStatusMessage.h"
#include "market_data/itch/messages/NoiiMessage.h"
#include "market_data/itch/messages/LuldAuctionCollarMessage.h"

namespace llt::itch
{

class ItchDecoder
{
public:

    static constexpr std::size_t
        SystemEventMessageSize = 12;  //bytes

    static constexpr std::size_t
        StockDirectoryMessageSize = 39;

    static constexpr std::size_t
        AddOrderMessageSize = 36;

    static constexpr std::size_t
        OrderExecutedMessageSize = 31;

    static constexpr std::size_t
        OrderExecutedWithPriceMessageSize = 36;

    static constexpr std::size_t
        OrderCancelMessageSize = 23;

    static constexpr std::size_t
        OrderDeleteMessageSize = 19;

    static constexpr std::size_t
        OrderReplaceMessageSize = 35;

    static constexpr std::size_t 
        AddOrderWithMpidMessageSize = 40;

    static constexpr std::size_t 
        TradeMessageSize = 44;

    static constexpr std::size_t
        LuldAuctionCollarMessageSize = 35;

    static constexpr std::size_t CrossTradeMessageSize = 40;

    static constexpr std::size_t BrokenTradeMessageSize = 19;

    static constexpr std::size_t StockTradingActionMessageSize = 25;

    static constexpr std::size_t MarketParticipantPositionMessageSize = 26;

    static constexpr std::size_t
    RegShoRestrictionMessageSize = 20;

    static constexpr std::size_t
        MwcbDeclineLevelMessageSize = 35;

    static constexpr std::size_t
        MwcbStatusMessageSize = 12;

    static constexpr std::size_t
        NoiiMessageSize = 50;

    static bool decodeSystemEvent(
        const std::uint8_t* data,
        std::size_t size,
        SystemEventMessage& message
    ) noexcept;


    static bool decodeStockDirectory(
        const std::uint8_t* data,
        std::size_t size,
        StockDirectoryMessage& message
    ) noexcept;


    static bool decodeAddOrder(
        const std::uint8_t* data,
        std::size_t size,
        AddOrderMessage& message
    ) noexcept;

    static bool decodeOrderExecuted(
        const std::uint8_t* data,
        std::size_t size,
        OrderExecutedMessage& message
    ) noexcept;

    static bool decodeOrderExecutedWithPrice(
        const std::uint8_t* data,
        std::size_t size,
        OrderExecutedWithPriceMessage& message
    ) noexcept;

    static bool decodeOrderCancel(
        const std::uint8_t* data,
        std::size_t size,
        OrderCancelMessage& message
    ) noexcept;

    static bool decodeOrderDelete(
        const std::uint8_t* data,
        std::size_t size,
        OrderDeleteMessage& message
    ) noexcept;

    static bool decodeOrderReplace(
        const std::uint8_t* data,
        std::size_t size,
        OrderReplaceMessage& message
    ) noexcept;

    static bool decodeAddOrderWithMpid(
        const std::uint8_t* data,
        std::size_t size,
        AddOrderWithMpidMessage& message
    ) noexcept;

    static bool decodeTrade(
        const std::uint8_t* data,
        std::size_t size,
        TradeMessage& message
    ) noexcept;

    static bool decodeCrossTrade(
        const std::uint8_t* data,
        std::size_t size,
        CrossTradeMessage& message
    ) noexcept;

    static bool decodeBrokenTrade(
        const std::uint8_t* data,
        std::size_t size,
        BrokenTradeMessage& message
    ) noexcept;

    static bool decodeStockTradingAction(
        const std::uint8_t* data,
        std::size_t size,
        StockTradingActionMessage& message
    ) noexcept;    

    static bool decodeMarketParticipantPosition(
        const std::uint8_t* data,
        std::size_t size,
        MarketParticipantPositionMessage& message
    ) noexcept;

    static bool decodeRegShoRestriction(
        const std::uint8_t*,
        std::size_t,
        RegShoRestrictionMessage&
    ) noexcept;
    
    static bool decodeMwcbDeclineLevel(
        const std::uint8_t*,
        std::size_t,
        MwcbDeclineLevelMessage&
    ) noexcept;
    
    static bool decodeMwcbStatus(
        const std::uint8_t*,
        std::size_t,
        MwcbStatusMessage&
    ) noexcept;
    
    static bool decodeNoii(
        const std::uint8_t*,
        std::size_t,
        NoiiMessage&
    ) noexcept;

    static bool decodeLuldAuctionCollar(
        const std::uint8_t* data,
        std::size_t size,
        LuldAuctionCollarMessage& message
    ) noexcept;

private:

    static std::uint16_t readU16(
        const std::uint8_t* data
    ) noexcept;


    static std::uint32_t readU32(
        const std::uint8_t* data
    ) noexcept;


    static std::uint64_t readU48(
        const std::uint8_t* data
    ) noexcept;


    static std::uint64_t readU64(
        const std::uint8_t* data
    ) noexcept;
};

} // namespace llt::itch