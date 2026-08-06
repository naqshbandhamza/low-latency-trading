#pragma once

#include "types/Price.h"
#include "types/Quantity.h"

namespace llt
{

class Level
{
public:

    constexpr Level(
        Price price,
        Quantity quantity
    ) noexcept
        : price_(price),
          quantity_(quantity)
    {
    }

    [[nodiscard]]
    constexpr Price price() const noexcept
    {
        return price_;
    }

    [[nodiscard]]
    constexpr Quantity quantity() const noexcept
    {
        return quantity_;
    }

private:

    Price price_;
    Quantity quantity_;
};

}