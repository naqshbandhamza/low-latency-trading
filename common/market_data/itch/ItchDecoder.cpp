#include "market_data/itch/ItchDecoder.h"

#include <algorithm>

#include "market_data/itch/ItchMessageType.h"

namespace llt::itch
{

std::uint16_t ItchDecoder::readU16(
    const std::uint8_t* data
) noexcept
{
    return
        (static_cast<std::uint16_t>(data[0]) << 8) |
        static_cast<std::uint16_t>(data[1]);
}


std::uint32_t ItchDecoder::readU32(
    const std::uint8_t* data
) noexcept
{
    return
        (static_cast<std::uint32_t>(data[0]) << 24) |
        (static_cast<std::uint32_t>(data[1]) << 16) |
        (static_cast<std::uint32_t>(data[2]) << 8) |
        static_cast<std::uint32_t>(data[3]);
}


std::uint64_t ItchDecoder::readU48(
    const std::uint8_t* data
) noexcept
{
    return
        (static_cast<std::uint64_t>(data[0]) << 40) |
        (static_cast<std::uint64_t>(data[1]) << 32) |
        (static_cast<std::uint64_t>(data[2]) << 24) |
        (static_cast<std::uint64_t>(data[3]) << 16) |
        (static_cast<std::uint64_t>(data[4]) << 8) |
        static_cast<std::uint64_t>(data[5]);
}


std::uint64_t ItchDecoder::readU64(
    const std::uint8_t* data
) noexcept
{
    return
        (static_cast<std::uint64_t>(data[0]) << 56) |
        (static_cast<std::uint64_t>(data[1]) << 48) |
        (static_cast<std::uint64_t>(data[2]) << 40) |
        (static_cast<std::uint64_t>(data[3]) << 32) |
        (static_cast<std::uint64_t>(data[4]) << 24) |
        (static_cast<std::uint64_t>(data[5]) << 16) |
        (static_cast<std::uint64_t>(data[6]) << 8) |
        static_cast<std::uint64_t>(data[7]);
}

bool ItchDecoder::decodeAddOrder(
    const std::uint8_t* data,
    std::size_t size,
    AddOrderMessage& message
) noexcept
{
    if (data == nullptr)
    {
        return false;
    }

    if (size != AddOrderMessageSize)
    {
        return false;
    }

    if (
        data[0] !=
        static_cast<std::uint8_t>(
            ItchMessageType::AddOrder
        )
    )
    {
        return false;
    }

    message.stockLocate =
        readU16(
            data + 1
        );

    message.trackingNumber =
        readU16(
            data + 3
        );

    message.timestamp =
        readU48(
            data + 5
        );

    message.orderReferenceNumber =
        readU64(
            data + 11
        );

    message.buySellIndicator =
        static_cast<char>(
            data[19]
        );

    message.shares =
        readU32(
            data + 20
        );

    std::copy_n(
        reinterpret_cast<const char*>(
            data + 24
        ),
        8,
        message.stock.begin()
    );

    message.price =
        readU32(
            data + 32
        );

    return true;
}


bool ItchDecoder::decodeSystemEvent(
    const std::uint8_t* data,
    std::size_t size,
    SystemEventMessage& message
) noexcept
{
    if (data == nullptr)
    {
        return false;
    }

    if (size != SystemEventMessageSize)
    {
        return false;
    }

    if (
        data[0] !=
        static_cast<std::uint8_t>(
            ItchMessageType::SystemEvent
        )
    )
    {
        return false;
    }

    message.stockLocate =
        readU16(data + 1);

    message.trackingNumber =
        readU16(data + 3);

    message.timestamp =
        readU48(data + 5);

    message.eventCode =
        static_cast<char>(
            data[11]
        );

    return true;
}


bool ItchDecoder::decodeStockDirectory(
    const std::uint8_t* data,
    std::size_t size,
    StockDirectoryMessage& message
) noexcept
{
    if (data == nullptr)
    {
        return false;
    }

    if (size != StockDirectoryMessageSize)
    {
        return false;
    }

    if (
        data[0] !=
        static_cast<std::uint8_t>(
            ItchMessageType::StockDirectory
        )
    )
    {
        return false;
    }

    message.stockLocate =
        readU16(data + 1);

    message.trackingNumber =
        readU16(data + 3);

    message.timestamp =
        readU48(data + 5);

    std::copy_n(
        reinterpret_cast<const char*>(
            data + 11
        ),
        8,
        message.stock.begin()
    );

    message.marketCategory =
        static_cast<char>(
            data[19]
        );

    message.financialStatusIndicator =
        static_cast<char>(
            data[20]
        );

    message.roundLotSize =
        readU32(
            data + 21
        );

    message.roundLotsOnly =
        static_cast<char>(
            data[25]
        );

    message.issueClassification =
        static_cast<char>(
            data[26]
        );

    std::copy_n(
        reinterpret_cast<const char*>(
            data + 27
        ),
        2,
        message.issueSubType.begin()
    );

    message.authenticity =
        static_cast<char>(
            data[29]
        );

    message.shortSaleThresholdIndicator =
        static_cast<char>(
            data[30]
        );

    message.ipoFlag =
        static_cast<char>(
            data[31]
        );

    message.luldReferencePriceTier =
        static_cast<char>(
            data[32]
        );

    message.etpFlag =
        static_cast<char>(
            data[33]
        );

    message.etpLeverageFactor =
        readU32(
            data + 34
        );

    message.inverseIndicator =
        static_cast<char>(
            data[38]
        );

    return true;
}


bool ItchDecoder::decodeOrderExecuted(
    const std::uint8_t* data,
    std::size_t size,
    OrderExecutedMessage& message
) noexcept
{
    if (data == nullptr)
    {
        return false;
    }

    if (size != OrderExecutedMessageSize)
    {
        return false;
    }

    if (
        data[0] !=
        static_cast<std::uint8_t>(
            ItchMessageType::OrderExecuted
        )
    )
    {
        return false;
    }

    message.stockLocate =
        readU16(
            data + 1
        );

    message.trackingNumber =
        readU16(
            data + 3
        );

    message.timestamp =
        readU48(
            data + 5
        );

    message.orderReferenceNumber =
        readU64(
            data + 11
        );

    message.executedShares =
        readU32(
            data + 19
        );

    message.matchNumber =
        readU64(
            data + 23
        );

    return true;
}


bool ItchDecoder::decodeOrderExecutedWithPrice(
    const std::uint8_t* data,
    std::size_t size,
    OrderExecutedWithPriceMessage& message
) noexcept
{
    if (data == nullptr)
    {
        return false;
    }

    if (size != OrderExecutedWithPriceMessageSize)
    {
        return false;
    }

    if (
        data[0] !=
        static_cast<std::uint8_t>(
            ItchMessageType::OrderExecutedWithPrice
        )
    )
    {
        return false;
    }

    message.stockLocate =
        readU16(
            data + 1
        );

    message.trackingNumber =
        readU16(
            data + 3
        );

    message.timestamp =
        readU48(
            data + 5
        );

    message.orderReferenceNumber =
        readU64(
            data + 11
        );

    message.executedShares =
        readU32(
            data + 19
        );

    message.matchNumber =
        readU64(
            data + 23
        );

    message.printable =
        static_cast<char>(
            data[31]
        );

    message.executionPrice =
        readU32(
            data + 32
        );

    return true;
}

bool ItchDecoder::decodeOrderCancel(
    const std::uint8_t* data,
    std::size_t size,
    OrderCancelMessage& message
) noexcept
{
    if (data == nullptr)
    {
        return false;
    }

    if (size != OrderCancelMessageSize)
    {
        return false;
    }

    if (
        data[0] !=
        static_cast<std::uint8_t>(
            ItchMessageType::OrderCancel
        )
    )
    {
        return false;
    }

    message.stockLocate =
        readU16(
            data + 1
        );

    message.trackingNumber =
        readU16(
            data + 3
        );

    message.timestamp =
        readU48(
            data + 5
        );

    message.orderReferenceNumber =
        readU64(
            data + 11
        );

    message.cancelledShares =
        readU32(
            data + 19
        );

    return true;
}


bool ItchDecoder::decodeOrderDelete(
    const std::uint8_t* data,
    std::size_t size,
    OrderDeleteMessage& message
) noexcept
{
    if (data == nullptr)
    {
        return false;
    }

    if (size != OrderDeleteMessageSize)
    {
        return false;
    }

    if (
        data[0] !=
        static_cast<std::uint8_t>(
            ItchMessageType::OrderDelete
        )
    )
    {
        return false;
    }

    message.stockLocate =
        readU16(
            data + 1
        );

    message.trackingNumber =
        readU16(
            data + 3
        );

    message.timestamp =
        readU48(
            data + 5
        );

    message.orderReferenceNumber =
        readU64(
            data + 11
        );

    return true;
}


bool ItchDecoder::decodeOrderReplace(
    const std::uint8_t* data,
    std::size_t size,
    OrderReplaceMessage& message
) noexcept
{
    if (data == nullptr)
    {
        return false;
    }

    if (size != OrderReplaceMessageSize)
    {
        return false;
    }

    if (
        data[0] !=
        static_cast<std::uint8_t>(
            ItchMessageType::OrderReplace
        )
    )
    {
        return false;
    }

    message.stockLocate =
        readU16(
            data + 1
        );

    message.trackingNumber =
        readU16(
            data + 3
        );

    message.timestamp =
        readU48(
            data + 5
        );

    message.originalOrderReferenceNumber =
        readU64(
            data + 11
        );

    message.newOrderReferenceNumber =
        readU64(
            data + 19
        );

    message.shares =
        readU32(
            data + 27
        );

    message.price =
        readU32(
            data + 31
        );

    return true;
}


bool ItchDecoder::decodeAddOrderWithMpid(
    const std::uint8_t* data,
    std::size_t size,
    AddOrderWithMpidMessage& message
) noexcept
{
    if (data == nullptr)
        return false;

    if (size != AddOrderWithMpidMessageSize)
        return false;

    if (data[0] !=
        static_cast<std::uint8_t>(
            ItchMessageType::AddOrderWithMpid
        ))
    {
        return false;
    }

    message.stockLocate =
        readU16(data + 1);

    message.trackingNumber =
        readU16(data + 3);

    message.timestamp =
        readU48(data + 5);

    message.orderReferenceNumber =
        readU64(data + 11);

    message.buySellIndicator =
        static_cast<char>(data[19]);

    message.shares =
        readU32(data + 20);

    for (std::size_t i = 0;
         i < message.stock.size();
         ++i)
    {
        message.stock[i] =
            static_cast<char>(
                data[24 + i]
            );
    }

    message.price =
        readU32(data + 32);

    for (std::size_t i = 0;
         i < message.attribution.size();
         ++i)
    {
        message.attribution[i] =
            static_cast<char>(
                data[36 + i]
            );
    }

    return true;
}



bool ItchDecoder::decodeTrade(
    const std::uint8_t* data,
    std::size_t size,
    TradeMessage& message
) noexcept
{
    if (data == nullptr)
        return false;

    if (size != TradeMessageSize)
        return false;

    if (data[0] !=
        static_cast<std::uint8_t>(
            ItchMessageType::Trade
        ))
    {
        return false;
    }

    message.stockLocate =
        readU16(data + 1);

    message.trackingNumber =
        readU16(data + 3);

    message.timestamp =
        readU48(data + 5);

    message.orderReferenceNumber =
        readU64(data + 11);

    message.buySellIndicator =
        static_cast<char>(data[19]);

    message.shares =
        readU32(data + 20);

    for (std::size_t i = 0;
         i < message.stock.size();
         ++i)
    {
        message.stock[i] =
            static_cast<char>(
                data[24 + i]
            );
    }

    message.price =
        readU32(data + 32);

    message.matchNumber =
        readU64(data + 36);

    return true;
}


bool ItchDecoder::decodeCrossTrade(
    const std::uint8_t* data,
    std::size_t size,
    CrossTradeMessage& message
) noexcept
{
    if (data == nullptr)
        return false;

    if (size != CrossTradeMessageSize)
        return false;

    if (data[0] !=
        static_cast<std::uint8_t>(
            ItchMessageType::CrossTrade
        ))
    {
        return false;
    }

    message.stockLocate =
        readU16(data + 1);

    message.trackingNumber =
        readU16(data + 3);

    message.timestamp =
        readU48(data + 5);

    message.shares =
        readU64(data + 11);

    for (std::size_t i = 0;
         i < message.stock.size();
         ++i)
    {
        message.stock[i] =
            static_cast<char>(
                data[19 + i]
            );
    }

    message.crossPrice =
        readU32(data + 27);

    message.matchNumber =
        readU64(data + 31);

    message.crossType =
        static_cast<char>(data[39]);

    return true;
}

bool ItchDecoder::decodeBrokenTrade(
    const std::uint8_t* data,
    std::size_t size,
    BrokenTradeMessage& message
) noexcept
{
    if (data == nullptr)
        return false;

    if (size != BrokenTradeMessageSize)
        return false;

    if (data[0] !=
        static_cast<std::uint8_t>(
            ItchMessageType::BrokenTrade
        ))
    {
        return false;
    }

    message.stockLocate =
        readU16(data + 1);

    message.trackingNumber =
        readU16(data + 3);

    message.timestamp =
        readU48(data + 5);

    message.matchNumber =
        readU64(data + 11);

    return true;
}

bool ItchDecoder::decodeStockTradingAction(
    const std::uint8_t* data,
    std::size_t size,
    StockTradingActionMessage& message
) noexcept
{
    if (data == nullptr)
        return false;

    if (size != StockTradingActionMessageSize)
        return false;

    if (data[0] !=
        static_cast<std::uint8_t>(
            ItchMessageType::StockTradingAction
        ))
    {
        return false;
    }

    message.stockLocate =
        readU16(data + 1);

    message.trackingNumber =
        readU16(data + 3);

    message.timestamp =
        readU48(data + 5);

    for (std::size_t i = 0;
         i < message.stock.size();
         ++i)
    {
        message.stock[i] =
            static_cast<char>(
                data[11 + i]
            );
    }

    message.tradingState =
        static_cast<char>(data[19]);

    message.reserved =
        static_cast<char>(data[20]);

    for (std::size_t i = 0;
         i < message.reason.size();
         ++i)
    {
        message.reason[i] =
            static_cast<char>(
                data[21 + i]
            );
    }

    return true;
}


bool ItchDecoder::decodeMarketParticipantPosition(
    const std::uint8_t* data,
    std::size_t size,
    MarketParticipantPositionMessage& message
) noexcept
{
    if (data == nullptr)
        return false;

    if (size != MarketParticipantPositionMessageSize)
        return false;

    if (data[0] !=
        static_cast<std::uint8_t>(
            ItchMessageType::MarketParticipantPosition
        ))
    {
        return false;
    }

    message.stockLocate =
        readU16(data + 1);

    message.trackingNumber =
        readU16(data + 3);

    message.timestamp =
        readU48(data + 5);

    for (std::size_t i = 0;
         i < message.mpid.size();
         ++i)
    {
        message.mpid[i] =
            static_cast<char>(
                data[11 + i]
            );
    }

    for (std::size_t i = 0;
         i < message.stock.size();
         ++i)
    {
        message.stock[i] =
            static_cast<char>(
                data[15 + i]
            );
    }

    message.primaryMarketMaker =
        static_cast<char>(data[23]);

    message.marketMakerMode =
        static_cast<char>(data[24]);

    message.marketParticipantState =
        static_cast<char>(data[25]);

    return true;
}

bool ItchDecoder::decodeRegShoRestriction(
    const std::uint8_t* data,
    std::size_t size,
    RegShoRestrictionMessage& message
) noexcept
{
    if (data == nullptr ||
        size != RegShoRestrictionMessageSize ||
        data[0] != static_cast<std::uint8_t>(
            ItchMessageType::RegShoRestriction))
    {
        return false;
    }

    message.stockLocate = readU16(data + 1);
    message.trackingNumber = readU16(data + 3);
    message.timestamp = readU48(data + 5);

    for (std::size_t i = 0; i < 8; ++i)
        message.stock[i] =
            static_cast<char>(data[11 + i]);

    message.regShoAction =
        static_cast<char>(data[19]);

    return true;
}

bool ItchDecoder::decodeMwcbDeclineLevel(
    const std::uint8_t* data,
    std::size_t size,
    MwcbDeclineLevelMessage& message
) noexcept
{
    if (data == nullptr ||
        size != MwcbDeclineLevelMessageSize ||
        data[0] != static_cast<std::uint8_t>(
            ItchMessageType::MwcbDeclineLevel))
    {
        return false;
    }

    message.stockLocate = readU16(data + 1);
    message.trackingNumber = readU16(data + 3);
    message.timestamp = readU48(data + 5);

    message.level1 = readU64(data + 11);
    message.level2 = readU64(data + 19);
    message.level3 = readU64(data + 27);

    return true;
}

bool ItchDecoder::decodeMwcbStatus(
    const std::uint8_t* data,
    std::size_t size,
    MwcbStatusMessage& message
) noexcept
{
    if (data == nullptr ||
        size != MwcbStatusMessageSize ||
        data[0] != static_cast<std::uint8_t>(
            ItchMessageType::MwcbStatus))
    {
        return false;
    }

    message.stockLocate = readU16(data + 1);
    message.trackingNumber = readU16(data + 3);
    message.timestamp = readU48(data + 5);

    message.breachedLevel =
        static_cast<char>(data[11]);

    return true;
}

bool ItchDecoder::decodeNoii(
    const std::uint8_t* data,
    std::size_t size,
    NoiiMessage& message
) noexcept
{
    if (data == nullptr ||
        size != NoiiMessageSize ||
        data[0] != static_cast<std::uint8_t>(
            ItchMessageType::Noii))
    {
        return false;
    }

    message.stockLocate = readU16(data + 1);
    message.trackingNumber = readU16(data + 3);
    message.timestamp = readU48(data + 5);

    message.pairedShares =
        readU64(data + 11);

    message.imbalanceShares =
        readU64(data + 19);

    message.imbalanceDirection =
        static_cast<char>(data[27]);

    for (std::size_t i = 0; i < 8; ++i)
        message.stock[i] =
            static_cast<char>(data[28 + i]);

    message.farPrice =
        readU32(data + 36);

    message.nearPrice =
        readU32(data + 40);

    message.currentReferencePrice =
        readU32(data + 44);

    message.crossType =
        static_cast<char>(data[48]);

    message.priceVariationIndicator =
        static_cast<char>(data[49]);

    return true;
}

bool ItchDecoder::decodeLuldAuctionCollar(
    const std::uint8_t* data,
    std::size_t size,
    LuldAuctionCollarMessage& message
) noexcept
{
    if (data == nullptr ||
        size != LuldAuctionCollarMessageSize ||
        data[0] != static_cast<std::uint8_t>(
            ItchMessageType::LuldAuctionCollar))
    {
        return false;
    }

    message.stockLocate =
        readU16(data + 1);

    message.trackingNumber =
        readU16(data + 3);

    message.timestamp =
        readU48(data + 5);

    for (std::size_t i = 0; i < 8; ++i)
    {
        message.stock[i] =
            static_cast<char>(
                data[11 + i]
            );
    }

    message.auctionCollarReferencePrice =
        readU32(data + 19);

    message.upperAuctionCollarPrice =
        readU32(data + 23);

    message.lowerAuctionCollarPrice =
        readU32(data + 27);

    message.auctionCollarExtension =
        readU32(data + 31);

    return true;
}



} // namespace llt::itch