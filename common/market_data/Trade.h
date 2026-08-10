#pragma once

#include <utility>

#include "market_data/Instrument.h"
#include "market_data/Side.h"

#include "types/Price.h"
#include "types/Quantity.h"
#include "types/SequenceNumber.h"
#include "types/Timestamp.h"

namespace llt
{

class Trade
{
public:

    Trade(
        Instrument instrument,
        SequenceNumber sequence,
        Timestamp timestamp,
        Price price,
        Quantity quantity,
        Side side
    )
        : instrument_(std::move(instrument))
        , sequence_(sequence)
        , timestamp_(timestamp)
        , price_(price)
        , quantity_(quantity)
        , side_(side)
    {
    }

    [[nodiscard]]
    const Instrument& instrument() const noexcept
    {
        return instrument_;
    }

    [[nodiscard]]
    constexpr SequenceNumber sequence() const noexcept
    {
        return sequence_;
    }

    [[nodiscard]]
    constexpr Timestamp timestamp() const noexcept
    {
        return timestamp_;
    }

    [[nodiscard]]
    constexpr Price price() const noexcept
    {
        return price_;
    }

    [[nodiscard]]
    constexpr Quantity quantity() const noexcept
    {
        return quantity_;
    }

    [[nodiscard]]
    constexpr Side side() const noexcept
    {
        return side_;
    }

private:

    Instrument instrument_;

    SequenceNumber sequence_;

    Timestamp timestamp_;

    Price price_;

    Quantity quantity_;

    Side side_;
};

} // namespace llt