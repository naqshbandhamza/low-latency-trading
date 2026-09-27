#pragma once

#include <cstdint>

namespace llt::itch
{

enum class ItchMessageType : std::uint8_t
{
    SystemEvent            = 'S',
    StockDirectory         = 'R',
    AddOrder               = 'A',
    AddOrderWithMpid = 'F',
    OrderExecuted          = 'E',
    OrderExecutedWithPrice = 'C',
    OrderCancel            = 'X',
    OrderDelete            = 'D',
    OrderReplace           = 'U',
    Trade = 'P',
    CrossTrade = 'Q',
    BrokenTrade = 'B',
    StockTradingAction = 'H',
    MarketParticipantPosition = 'L',
    RegShoRestriction = 'Y',
    // MwcbDeclineLevel = 'J',
    // MwcbStatus = 'V',
    LuldAuctionCollar = 'J',
    MwcbDeclineLevel = 'V',
    MwcbStatus = 'W',
    Noii = 'I',
};

} // namespace llt::itch