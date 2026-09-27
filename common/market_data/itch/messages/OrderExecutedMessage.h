#pragma once

#include <cstdint>

namespace llt::itch
{

struct OrderExecutedMessage
{
    std::uint16_t stockLocate{0};

    std::uint16_t trackingNumber{0};

    std::uint64_t timestamp{0};

    std::uint64_t orderReferenceNumber{0};

    std::uint32_t executedShares{0};

    std::uint64_t matchNumber{0};
};

} // namespace llt::itch