#pragma once

#include <cstdint>

namespace llt::itch
{

struct OrderReplaceMessage
{
    std::uint16_t stockLocate{0};

    std::uint16_t trackingNumber{0};

    std::uint64_t timestamp{0};

    std::uint64_t originalOrderReferenceNumber{0};

    std::uint64_t newOrderReferenceNumber{0};

    std::uint32_t shares{0};

    std::uint32_t price{0};
};

} // namespace llt::itch