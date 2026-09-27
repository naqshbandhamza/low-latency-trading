#pragma once

#include <cstdint>

namespace llt::market_data
{

using InstrumentId = std::uint16_t;
using Price        = std::uint32_t;
using Quantity     = std::uint64_t;
using Timestamp    = std::uint64_t;
using OrderId      = std::uint64_t;

enum class Side : std::uint8_t
{
    Buy,
    Sell
};

enum class TradingState : std::uint8_t
{
    Unknown,
    Halted,
    Paused,
    QuotationOnly,
    Trading
};

enum class RegShoState : std::uint8_t
{
    Unknown,
    NoRestriction,
    RestrictionInEffect,
    RestrictionRemains
};

} // namespace llt::market_data