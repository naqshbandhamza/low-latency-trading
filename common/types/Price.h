#pragma once

#include <cstdint>

namespace llt
{

class Price
{
public:

    constexpr explicit Price(std::int64_t value)
        : value_(value)
    {
    }

    [[nodiscard]]
    constexpr std::int64_t value() const noexcept
    {
        return value_;
    }

private:

    std::int64_t value_;
};

} // namespace llt