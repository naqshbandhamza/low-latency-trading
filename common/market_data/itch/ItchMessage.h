#pragma once

#include <variant>

#include "market_data/itch/messages/SystemEventMessage.h"
#include "market_data/itch/messages/StockDirectoryMessage.h"
#include "market_data/itch/messages/AddOrderMessage.h"
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

    using ItchMessage = std::variant<
        SystemEventMessage,
        StockDirectoryMessage,
        AddOrderMessage,
        AddOrderWithMpidMessage,
        OrderExecutedMessage,
        OrderExecutedWithPriceMessage,
        OrderCancelMessage,
        OrderDeleteMessage,
        OrderReplaceMessage,
        TradeMessage,
        CrossTradeMessage,
        BrokenTradeMessage,
        StockTradingActionMessage,
        MarketParticipantPositionMessage,
        RegShoRestrictionMessage,
        MwcbDeclineLevelMessage,
        MwcbStatusMessage,
        NoiiMessage,
        LuldAuctionCollarMessage
        >;

} // namespace llt::itch