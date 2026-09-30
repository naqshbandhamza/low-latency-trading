#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "market_data/MarketEventQueue.h"
#include "market_data/normalized/InstrumentStore.h"
#include "market_data/book/OrderBook.h"

namespace llt::itch
{

class ItchQuotePublisher
{
public:
    static constexpr std::size_t
        UnknownInstrumentSampleCapacity = 16;

    struct UnknownInstrumentSample
    {
        market_data::InstrumentId instrumentId{0};
        market_data::Timestamp timestamp{0};
        std::uint64_t occurrences{0};
    };

    ItchQuotePublisher(
        const market_data::InstrumentStore &instruments,
        MarketEventQueue &queue) noexcept
        : instruments_(instruments),
          queue_(queue)
    {
    }

    void onBboChange(
        market_data::InstrumentId instrumentId,
        market_data::Timestamp timestamp,
        const market_data::Bbo &bbo);

    [[nodiscard]]
    std::uint64_t publishedQuotes() const noexcept
    {
        return publishedQuotes_;
    }

    [[nodiscard]]
    std::uint64_t droppedQuotes() const noexcept
    {
        return droppedQuotes_;
    }

    [[nodiscard]]
    std::uint64_t unknownInstruments() const noexcept
    {
        return unknownInstruments_;
    }

    [[nodiscard]]
    const auto &unknownInstrumentSamples() const noexcept
    {
        return unknownInstrumentSamples_;
    }

    [[nodiscard]]
    std::size_t unknownInstrumentSampleCount() const noexcept
    {
        return unknownInstrumentSampleCount_;
    }

private:
    void recordUnknownInstrument(
        market_data::InstrumentId instrumentId,
        market_data::Timestamp timestamp) noexcept;

    const market_data::InstrumentStore &instruments_;
    MarketEventQueue &queue_;

    std::uint64_t nextSequence_{0};

    std::uint64_t publishedQuotes_{0};
    std::uint64_t droppedQuotes_{0};
    std::uint64_t unknownInstruments_{0};

    std::array<
        UnknownInstrumentSample,
        UnknownInstrumentSampleCapacity>
        unknownInstrumentSamples_{};

    std::size_t
        unknownInstrumentSampleCount_{0};
};

} // namespace llt::itch