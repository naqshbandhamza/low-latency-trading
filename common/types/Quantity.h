#pragma once

#include "StrongType.h"

#include <cstdint>

namespace llt
{

struct QuantityTag
{
};

using Quantity =
    StrongType<std::uint32_t, QuantityTag>;

}