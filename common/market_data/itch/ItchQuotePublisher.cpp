#include "market_data/itch/ItchQuotePublisher.h"

#include <cstdint>
#include <limits>
#include <utility>

#include "market_data/MarketEvent.h"

namespace llt::itch
{

void ItchQuotePublisher::recordUnknownInstrument(
    market_data::InstrumentId instrumentId,
    market_data::Timestamp timestamp) noexcept
{
    //
    // First see whether this ID is already one
    // of our bounded diagnostic samples.
    //
    for (
        std::size_t i = 0;
        i < unknownInstrumentSampleCount_;
        ++i)
    {
        auto &sample =
            unknownInstrumentSamples_[i];

        if (sample.instrumentId == instrumentId)
        {
            ++sample.occurrences;
            return;
        }
    }

    //
    // Only retain the first N unique IDs.
    //
    // This keeps diagnostics bounded and avoids
    // allocations on the publication path.
    //
    if (
        unknownInstrumentSampleCount_ >=
        UnknownInstrumentSampleCapacity)
    {
        return;
    }

    unknownInstrumentSamples_[
        unknownInstrumentSampleCount_++] =
        UnknownInstrumentSample{
            instrumentId,
            timestamp,
            1};
}


void ItchQuotePublisher::onBboChange(
    market_data::InstrumentId instrumentId,
    market_data::Timestamp timestamp,
    const market_data::Bbo &bbo)
{
    const auto *instrumentInfo =
        instruments_.find(
            instrumentId);

    if (instrumentInfo == nullptr)
    {
        ++unknownInstruments_;

        recordUnknownInstrument(
            instrumentId,
            timestamp);

        return;
    }

    if (!bbo.hasBid || !bbo.hasAsk)
    {
        return;
    }

    if (
        bbo.bidQuantity >
            std::numeric_limits<std::uint32_t>::max() ||
        bbo.askQuantity >
            std::numeric_limits<std::uint32_t>::max())
    {
        ++droppedQuotes_;
        return;
    }

    const Instrument instrument{
        instrumentInfo->symbolView()};

    const SequenceNumber sequence{
        nextSequence_++};

    const Level bid{
        Price{
            bbo.bidPrice},
        Quantity{
            static_cast<std::uint32_t>(
                bbo.bidQuantity)}};

    const Level ask{
        Price{
            bbo.askPrice},
        Quantity{
            static_cast<std::uint32_t>(
                bbo.askQuantity)}};

    Quote quote{
        instrument,
        sequence,
        Timestamp{
            timestamp},
        bid,
        ask};

    MarketEvent event{
        std::move(quote)};

    if (!queue_.push(
            std::move(event)))
    {
        ++droppedQuotes_;
        return;
    }

    ++publishedQuotes_;
}

} // namespace llt::itch