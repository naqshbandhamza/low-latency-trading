#pragma once

#include "StrongType.h"

#include <cstdint>

namespace llt
{

struct TimestampTag
{
};

using Timestamp =
    StrongType<std::uint64_t, TimestampTag>;

}