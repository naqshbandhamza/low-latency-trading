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

    Quote(const Quote&) = default;
    Quote(Quote&&) noexcept = default;
    // Copy constructor
    // Quote(const Quote& other)
    // : instrument_(other.instrument_)
    // , sequence_(other.sequence_)
    // , timestamp_(other.timestamp_)
    // , bid_(other.bid_)
    // , ask_(other.ask_)
    // {
    //     //std::cout<<"copy constrcutor quote";
    // }

    // // Move constructor
    // Quote(Quote&& other) noexcept
    // : instrument_(std::move(other.instrument_))
    // , sequence_(std::move(other.sequence_))
    // , timestamp_(std::move(other.timestamp_))
    // , bid_(std::move(other.bid_))
    // , ask_(std::move(other.ask_))
    // {
    //     //std::cout<<"move constrcutor quote";
    // }

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