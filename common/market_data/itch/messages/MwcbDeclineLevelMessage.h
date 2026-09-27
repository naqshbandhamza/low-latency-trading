#pragma once

#include <cstdint>

namespace llt::itch
{

struct MwcbDeclineLevelMessage
{
    std::uint16_t stockLocate{0};
    std::uint16_t trackingNumber{0};
    std::uint64_t timestamp{0};

    std::uint64_t level1{0};
    std::uint64_t level2{0};
    std::uint64_t level3{0};
};

}