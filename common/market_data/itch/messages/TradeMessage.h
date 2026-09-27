#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace llt::itch
{

struct TradeMessage
{
    std::uint16_t stockLocate{0};
    std::uint16_t trackingNumber{0};
    std::uint64_t timestamp{0};

    std::uint64_t orderReferenceNumber{0};

    char buySellIndicator{0};

    std::uint32_t shares{0};

    std::array<char, 8> stock{};

    std::uint32_t price{0};

    std::uint64_t matchNumber{0};

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
};

} // namespace llt::itch