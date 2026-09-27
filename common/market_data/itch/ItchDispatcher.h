#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

#include "market_data/itch/ItchMessage.h"

namespace llt::itch
{

class ItchDispatcher
{
public:
    static std::optional<ItchMessage> dispatch(
        const std::uint8_t* data,
        std::size_t size
    ) noexcept;
};

} // namespace llt::itch