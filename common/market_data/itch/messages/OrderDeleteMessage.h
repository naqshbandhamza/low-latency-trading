#pragma once

#include <cstdint>

namespace llt::itch
{

struct OrderDeleteMessage
{
    std::uint16_t stockLocate{0};

    std::uint16_t trackingNumber{0};

    std::uint64_t timestamp{0};

    std::uint64_t orderReferenceNumber{0};
};

} // namespace llt::itch