#pragma once

#include "market_data/normalized/MarketTypes.h"

namespace llt::market_data
{

struct Order
{
    OrderId orderId{0};
    InstrumentId instrumentId{0};
    Timestamp timestamp{0};

    Price price{0};
    Quantity quantity{0};

    Side side{Side::Buy};
};

} // namespace llt::market_data