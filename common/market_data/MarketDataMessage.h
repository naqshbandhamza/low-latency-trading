#pragma once

#include <cstdint>

#include "market_data/Side.h"

namespace llt
{

enum class MarketDataMessageType
{
    Quote,
    Trade
};

struct MarketDataMessage
{
    MarketDataMessageType type{
        MarketDataMessageType::Quote
    };

    std::uint64_t sequence{0};

    std::uint64_t timestamp{0};

    std::int64_t price{0};

    std::uint64_t quantity{0};

    std::int64_t bidPrice{0};

    std::uint64_t bidQuantity{0};

    std::int64_t askPrice{0};

    std::uint64_t askQuantity{0};

    Side side{
        Side::Buy
    };
};

} // namespace llt