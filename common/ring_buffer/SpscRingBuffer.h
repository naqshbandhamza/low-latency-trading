#pragma once

#include <array>
#include <cstddef>
#include <optional>

namespace llt
{

template<typename T, std::size_t Capacity>
class SpscRingBuffer
{
    static_assert(Capacity > 0, "Capacity must be greater than zero");

public:

    [[nodiscard]]
    bool push(const T& value)
    {
        if (size_ == Capacity)
        {
            return false;
        }

        buffer_[writeIndex_] = value;

        writeIndex_ = increment(writeIndex_);

        ++size_;

        return true;
    }

    [[nodiscard]]
    std::optional<T> pop()
    {
        if (size_ == 0)
        {
            return std::nullopt;
        }

        T value = buffer_[readIndex_];

        readIndex_ = increment(readIndex_);

        --size_;

        return value;
    }

    [[nodiscard]]
    bool empty() const noexcept
    {
        return size_ == 0;
    }

    [[nodiscard]]
    bool full() const noexcept
    {
        return size_ == Capacity;
    }

    [[nodiscard]]
    std::size_t size() const noexcept
    {
        return size_;
    }

private:

    [[nodiscard]]
    static constexpr std::size_t increment(
        std::size_t index) noexcept
    {
        return (index + 1) % Capacity;
    }

private:

    std::array<T, Capacity> buffer_{};

    std::size_t writeIndex_{0};

    std::size_t readIndex_{0};

    std::size_t size_{0};
};

} // namespace llt