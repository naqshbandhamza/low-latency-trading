#pragma once

#include <variant>

#include "market_data/Quote.h"
#include "market_data/Trade.h"

namespace llt
{

using MarketEvent = std::variant<
    Quote,
    Trade
>;

} // namespace llt