#pragma once

#include "market_data/Instrument.h"
#include "types/Price.h"
#include "types/Quantity.h"
#include "market_data/Side.h"

namespace llt
{

struct OrderIntent
{
    Instrument instrument;
    Side side;
    Price price;
    Quantity quantity;
};

} // namespace llt