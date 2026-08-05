#pragma once

#include "StrongType.h"

#include <cstdint>

namespace llt
{

struct PriceTag
{
};

using Price =
    StrongType<std::int64_t, PriceTag>;

}