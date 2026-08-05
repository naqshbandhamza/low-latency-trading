#pragma once

#include <type_traits>

namespace llt
{

    template <typename T, typename Tag>
    class StrongType
    {
    public:
        constexpr explicit StrongType(T value) noexcept
            : value_(value)
        {
        }

        [[nodiscard]]
        constexpr T value() const noexcept
        {
            return value_;
        }

        [[nodiscard]]
        constexpr bool operator==(const StrongType &other) const noexcept
        {
            return value_ == other.value_;
        }

        [[nodiscard]]
        constexpr bool operator!=(const StrongType &other) const noexcept
        {
            return !(*this == other);
        }

        [[nodiscard]]
        constexpr bool operator<(const StrongType &other) const noexcept
        {
            return value_ < other.value_;
        }

        [[nodiscard]]
        constexpr bool operator<=(const StrongType &other) const noexcept
        {
            return !(other < *this);
        }

        [[nodiscard]]
        constexpr bool operator>(const StrongType &other) const noexcept
        {
            return other < *this;
        }

        [[nodiscard]]
        constexpr bool operator>=(const StrongType &other) const noexcept
        {
            return !(*this < other);
        }

    private:
        T value_;
    };

}