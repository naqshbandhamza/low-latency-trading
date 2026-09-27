#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace llt::itch
{

struct MarketParticipantPositionMessage
{
    std::uint16_t stockLocate{0};
    std::uint16_t trackingNumber{0};
    std::uint64_t timestamp{0};

    std::array<char, 4> mpid{};
    std::array<char, 8> stock{};

    char primaryMarketMaker{0};
    char marketMakerMode{0};
    char marketParticipantState{0};

    [[nodiscard]]
    std::string_view mpidView() const noexcept
    {
        std::size_t length = mpid.size();

        while (length > 0 &&
               mpid[length - 1] == ' ')
        {
            --length;
        }

        return std::string_view(
            mpid.data(),
            length
        );
    }

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