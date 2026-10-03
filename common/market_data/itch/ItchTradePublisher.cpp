#include "market_data/itch/ItchTradePublisher.h"

#include <utility>

#include "market_data/MarketEvent.h"
#include "market_data/Trade.h"

namespace llt::itch
{

void ItchTradePublisher::onOrderExecution(
    market_data::InstrumentId instrumentId,
    market_data::Timestamp timestamp,
    market_data::Price price,
    market_data::Quantity quantity,
    market_data::Side side)
{
    const auto* instrumentInfo =
        instruments_.find(instrumentId);

    if (instrumentInfo == nullptr)
    {
        ++unknownInstruments_;
        return;
    }

    const Instrument instrument{
        instrumentInfo->symbolView()};

    const SequenceNumber sequence{
        nextSequence_++};

    const auto normalizedSide =
        side == market_data::Side::Buy
            ? Side::Buy
            : Side::Sell;

    Trade trade{
        instrument,
        sequence,
        Timestamp{timestamp},
        Price{price},
        Quantity{
            static_cast<std::uint32_t>(
                quantity)},
        normalizedSide};

    MarketEvent event{
        std::move(trade)};

    if (!queue_.push(
            std::move(event)))
    {
        ++droppedTrades_;
        return;
    }

    ++publishedTrades_;
}

} // namespace llt::itch