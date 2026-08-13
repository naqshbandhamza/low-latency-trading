#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <optional>

namespace llt
{

template<typename T, std::size_t Capacity>
class SpscRingBuffer
{
    static_assert(Capacity > 0,
                  "Capacity must be greater than zero");

public:

    [[nodiscard]]
    bool push(const T& value)
    {
        const std::size_t write =
            writeIndex_.load(std::memory_order_relaxed);

        const std::size_t next =
            increment(write);

        const std::size_t read =
            readIndex_.load(std::memory_order_acquire);

        if (next == read)
        {
            return false;
        }

        buffer_[write] = value;

        writeIndex_.store(
            next,
            std::memory_order_release
        );

        return true;
    }

    [[nodiscard]]
    std::optional<T> pop()
    {
        const std::size_t read =
            readIndex_.load(std::memory_order_relaxed);

        const std::size_t write =
            writeIndex_.load(std::memory_order_acquire);

        if (read == write)
        {
            return std::nullopt;
        }

        T value = buffer_[read];

        readIndex_.store(
            increment(read),
            std::memory_order_release
        );

        return value;
    }

    [[nodiscard]]
    bool empty() const noexcept
    {
        const std::size_t read =
            readIndex_.load(std::memory_order_relaxed);

        const std::size_t write =
            writeIndex_.load(std::memory_order_acquire);

        return read == write;
    }

    [[nodiscard]]
    bool full() const noexcept
    {
        const std::size_t write =
            writeIndex_.load(std::memory_order_relaxed);

        const std::size_t next =
            increment(write);

        const std::size_t read =
            readIndex_.load(std::memory_order_acquire);

        return next == read;
    }

    [[nodiscard]]
    std::size_t size() const noexcept
    {
        const std::size_t write =
            writeIndex_.load(std::memory_order_acquire);

        const std::size_t read =
            readIndex_.load(std::memory_order_acquire);

        if (write >= read)
        {
            return write - read;
        }

        return Capacity - read + write;
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

    std::atomic<std::size_t> writeIndex_{0};

    std::atomic<std::size_t> readIndex_{0};
};

} // namespace llt