#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <optional>
#include <utility>

namespace benchmark
{

template<typename T, std::size_t Capacity>
class SeqCstSpscRingBuffer
{
public:

    bool push(const T& value)
    {
        const auto write =
            writeIndex_.load(std::memory_order_seq_cst);

        const auto read =
            readIndex_.load(std::memory_order_seq_cst);

        if (write - read >= Capacity)
        {
            return false;
        }

        buffer_[write % Capacity] = value;

        writeIndex_.store(
            write + 1,
            std::memory_order_seq_cst
        );

        return true;
    }

    bool push(T&& value)
    {
        const auto write =
            writeIndex_.load(std::memory_order_seq_cst);

        const auto read =
            readIndex_.load(std::memory_order_seq_cst);

        if (write - read >= Capacity)
        {
            return false;
        }

        buffer_[write % Capacity] =
            std::move(value);

        writeIndex_.store(
            write + 1,
            std::memory_order_seq_cst
        );

        return true;
    }

    std::optional<T> pop()
    {
        const auto read =
            readIndex_.load(std::memory_order_seq_cst);

        const auto write =
            writeIndex_.load(std::memory_order_seq_cst);

        if (read == write)
        {
            return std::nullopt;
        }

        T value =
            std::move(buffer_[read % Capacity]);

        readIndex_.store(
            read + 1,
            std::memory_order_seq_cst
        );

        return value;
    }

private:

    std::array<T, Capacity> buffer_{};

    alignas(64)
    std::atomic<std::size_t> writeIndex_{0};

    alignas(64)
    std::atomic<std::size_t> readIndex_{0};
};

} // namespace benchmark