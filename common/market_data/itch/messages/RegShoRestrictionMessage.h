#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace llt::itch
{

struct RegShoRestrictionMessage
{
    std::uint16_t stockLocate{0};
    std::uint16_t trackingNumber{0};
    std::uint64_t timestamp{0};

    std::array<char, 8> stock{};
    char regShoAction{0};

    [[nodiscard]]
    std::string_view stockView() const noexcept
    {
        std::size_t length = stock.size();

        while (length > 0 &&
               stock[length - 1] == ' ')
        {
            --length;
        }

        return {stock.data(), length};
    }
};

}