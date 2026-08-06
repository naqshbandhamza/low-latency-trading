#pragma once

#include <string>

#include "market_data/Level.h"
#include "types/SequenceNumber.h"
#include "types/Timestamp.h"
#include "market_data/Instrument.h"

namespace llt
{


class Quote
{
public:

    Quote(
        Instrument instrument,
        SequenceNumber sequence,
        Timestamp timestamp,
        Level bid,
        Level ask
    ) noexcept
        : instrument_(std::move(instrument))
        , sequence_(sequence)
        , timestamp_(timestamp)
        , bid_(bid)
        , ask_(ask)
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
    constexpr Level bid() const noexcept
    {
        return bid_;
    }

    [[nodiscard]]
    constexpr Level ask() const noexcept
    {
        return ask_;
    }

private:

    Instrument instrument_;

    SequenceNumber sequence_;

    Timestamp timestamp_;

    Level bid_;

    Level ask_;
};

} // namespace llt