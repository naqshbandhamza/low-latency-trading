#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace llt::itch
{

struct StockDirectoryMessage
{
    std::uint16_t stockLocate{0};
    std::uint16_t trackingNumber{0};
    std::uint64_t timestamp{0};

    std::array<char, 8> stock{};

    char marketCategory{0};
    char financialStatusIndicator{0};

    std::uint32_t roundLotSize{0};

    char roundLotsOnly{0};
    char issueClassification{0};

    std::array<char, 2> issueSubType{};

    char authenticity{0};
    char shortSaleThresholdIndicator{0};
    char ipoFlag{0};
    char luldReferencePriceTier{0};
    char etpFlag{0};

    std::uint32_t etpLeverageFactor{0};

    char inverseIndicator{0};

    [[nodiscard]]
    std::string_view stockView() const noexcept
    {
        std::size_t length =
            stock.size();

        while (
            length > 0 &&
            stock[length - 1] == ' '
        )
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