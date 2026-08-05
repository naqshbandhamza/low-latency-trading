#pragma once

#include "StrongType.h"

#include <cstdint>

namespace llt
{

struct SequenceNumberTag
{
};

using SequenceNumber =
    StrongType<std::uint64_t, SequenceNumberTag>;

}