#pragma once

#include <array>
#include <cstddef>
#include <string_view>

#include "market_data/normalized/MarketTypes.h"

namespace llt::market_data
{

struct Instrument
{
    InstrumentId id{0};

    std::array<char, 8> symbol{};

    TradingState tradingState{
        TradingState::Unknown
    };

    //char regShoAction{'0'};
    RegShoState regShoState{
        RegShoState::Unknown
    };

    [[nodiscard]]
    std::string_view symbolView() const noexcept
    {
        std::size_t length = symbol.size();

        while (
            length > 0 &&
            symbol[length - 1] == ' '
        )
        {
            --length;
        }

        return {
            symbol.data(),
            length
        };
    }
};

} // namespace llt::market_data