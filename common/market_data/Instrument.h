// #pragma once

// #include <string>

// namespace llt
// {

// using Instrument = std::string;

// }

#pragma once

#include <array>
#include <cstddef>
#include <string_view>

namespace llt
{

class Instrument
{
public:

    static constexpr std::size_t MaxLength = 16;

    constexpr Instrument() noexcept = default;

    constexpr Instrument(std::string_view value) noexcept
    {
        const auto length =
            value.size() < MaxLength
                ? value.size()
                : MaxLength;

        for (std::size_t i = 0; i < length; ++i)
        {
            data_[i] = value[i];
        }

        size_ = length;
    }

    [[nodiscard]]
    constexpr std::string_view view() const noexcept
    {
        return {
            data_.data(),
            size_
        };
    }

    [[nodiscard]]
    constexpr std::size_t size() const noexcept
    {
        return size_;
    }

    [[nodiscard]]
    constexpr bool operator==(
        const Instrument& other
    ) const noexcept
    {
        return view() == other.view();
    }

private:

    std::array<char, MaxLength> data_{};
    std::size_t size_{0};
};

} // namespace llt