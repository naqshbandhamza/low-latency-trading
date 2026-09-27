#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace llt::itch
{

struct StockTradingActionMessage
{
    std::uint16_t stockLocate{0};
    std::uint16_t trackingNumber{0};
    std::uint64_t timestamp{0};

    std::array<char, 8> stock{};

    char tradingState{0};
    char reserved{0};

    std::array<char, 4> reason{};

    [[nodiscard]]
    std::string_view stockView() const noexcept
    {
        std::size_t length = stock.size();

        while (length > 0 &&
               stock[length - 1] == ' ')
        {
            --length;
        }

        return std::string_view(
            stock.data(),
            length
        );
    }

    [[nodiscard]]
    std::string_view reasonView() const noexcept
    {
        std::size_t length = reason.size();

        while (length > 0 &&
               reason[length - 1] == ' ')
        {
            --length;
        }

        return std::string_view(
            reason.data(),
            length
        );
    }
};

} // namespace llt::itch