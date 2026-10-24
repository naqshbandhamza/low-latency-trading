#pragma once

// #include <array>
// #include <atomic>
// #include <cstddef>
// #include <memory>
// #include <new>
// #include <optional>
// #include <utility>

// namespace llt
// {

//     template <typename T, std::size_t Capacity>
//     class SpscRingBuffer
//     {
//         static_assert(Capacity > 0,
//                       "Capacity must be greater than zero");

//     public:
//         [[nodiscard]]
//         bool push(const T &value)
//         {
//             const std::size_t write =
//                 writeIndex_.load(std::memory_order_relaxed);

//             const std::size_t next =
//                 increment(write);

//             if (next == cachedReadIndex_)
//             {
//                 cachedReadIndex_ =
//                     readIndex_.load(
//                         std::memory_order_acquire);

//                 if (next == cachedReadIndex_)
//                 {
//                     return false;
//                 }
//             }

//             T *slot = ptr(write);

//             ::new (static_cast<void *>(slot))
//                 T(value);

//             writeIndex_.store(
//                 next,
//                 std::memory_order_release);

//             return true;
//         }

//         [[nodiscard]]
//         bool push(T &&value)
//         {
//             const std::size_t write =
//                 writeIndex_.load(std::memory_order_relaxed);

//             const std::size_t next =
//                 increment(write);

//             if (next == cachedReadIndex_)
//             {
//                 cachedReadIndex_ =
//                     readIndex_.load(
//                         std::memory_order_acquire);

//                 if (next == cachedReadIndex_)
//                 {
//                     return false;
//                 }
//             }

//             T *slot = ptr(write);

//             ::new (static_cast<void *>(slot))
//                 T(std::move(value));

//             writeIndex_.store(
//                 next,
//                 std::memory_order_release);

//             return true;
//         }

//         [[nodiscard]]
//         std::optional<T> pop()
//         {
//             const std::size_t read =
//                 readIndex_.load(std::memory_order_relaxed);

//             if (read == cachedWriteIndex_)
//             {
//                 cachedWriteIndex_ =
//                     writeIndex_.load(
//                         std::memory_order_acquire);

//                 if (read == cachedWriteIndex_)
//                 {
//                     return std::nullopt;
//                 }
//             }

//             T *slot = ptr(read);

//             std::optional<T> value{
//                 std::move(*slot)};

//             slot->~T();

//             readIndex_.store(
//                 increment(read),
//                 std::memory_order_release);

//             return value;
//         }

//         [[nodiscard]]
//         bool empty() const noexcept
//         {
//             const std::size_t read =
//                 readIndex_.load(std::memory_order_relaxed);

//             const std::size_t write =
//                 writeIndex_.load(std::memory_order_acquire);

//             return read == write;
//         }

//         [[nodiscard]]
//         bool full() const noexcept
//         {
//             const std::size_t write =
//                 writeIndex_.load(std::memory_order_relaxed);

//             const std::size_t next =
//                 increment(write);

//             const std::size_t read =
//                 readIndex_.load(std::memory_order_acquire);

//             return next == read;
//         }

//         [[nodiscard]]
//         std::size_t size() const noexcept
//         {
//             const std::size_t write =
//                 writeIndex_.load(std::memory_order_acquire);

//             const std::size_t read =
//                 readIndex_.load(std::memory_order_acquire);

//             if (write >= read)
//             {
//                 return write - read;
//             }

//             return Capacity - read + write;
//         }

//         T *ptr(std::size_t index) noexcept
//         {
//             return std::launder(
//                 reinterpret_cast<T *>(
//                     storage_.data() + index * sizeof(T)));
//         }

//         ~SpscRingBuffer()
//         {
//             while (!empty())
//             {
//                 (void)pop();
//             }
//         }

//     private:
//         [[nodiscard]]
//         static constexpr std::size_t increment(
//             std::size_t index) noexcept
//         {
//             return (index + 1) % Capacity;
//         }

//         alignas(T)
//             std::array<std::byte, sizeof(T) * Capacity> storage_{};

//         alignas(64)
//             std::atomic<std::size_t> writeIndex_{0};

//         alignas(64)
//             std::atomic<std::size_t> readIndex_{0};

//         std::size_t cachedReadIndex_{0};
//         std::size_t cachedWriteIndex_{0};
//     };

// } // namespace llt

#pragma once

#include <atomic>
#include <cstddef>
#include <memory>
#include <new>
#include <optional>
#include <type_traits>
#include <utility>

namespace llt
{

    template <typename T, std::size_t Capacity>
    class SpscRingBuffer
    {
        static_assert(
            Capacity > 0,
            "Capacity must be greater than zero");

        // Raw storage for one T with the correct size and
        // alignment.
        //
        // The backing array of these slots is allocated once
        // on the heap when the queue is constructed.
        //
        // push() and pop() perform no heap allocation.
        using Storage =
            std::aligned_storage_t<
                sizeof(T),
                alignof(T)>;

    public:
        SpscRingBuffer()
            : storage_(
                  std::make_unique<Storage[]>(
                      Capacity))
        {
        }

        SpscRingBuffer(
            const SpscRingBuffer &) = delete;

        SpscRingBuffer &
        operator=(
            const SpscRingBuffer &) = delete;

        SpscRingBuffer(
            SpscRingBuffer &&) = delete;

        SpscRingBuffer &
        operator=(
            SpscRingBuffer &&) = delete;

        [[nodiscard]]
        bool push(const T &value)
        {
            const std::size_t write =
                producer_.writeIndex.load(
                    std::memory_order_relaxed);

            const std::size_t next = increment(write);

            if (next == producer_.cachedReadIndex)
            {
                producer_.cachedReadIndex =
                    consumer_.readIndex.load(
                        std::memory_order_acquire);

                if (next == producer_.cachedReadIndex)
                    return false;
            }

            T *slot = ptr(write);

            ::new (static_cast<void *>(slot)) T(value);

            producer_.writeIndex.store(
                next,
                std::memory_order_release);

            return true;
        }

        [[nodiscard]]
        bool push(
            T &&value)
        {
            const std::size_t write =
                producer_.writeIndex.load(
                    std::memory_order_relaxed);

            const std::size_t next = increment(write);

            if (next == producer_.cachedReadIndex)
            {
                producer_.cachedReadIndex =
                    consumer_.readIndex.load(
                        std::memory_order_acquire);

                if (next == producer_.cachedReadIndex)
                    return false;
            }

            T *slot = ptr(write);

            ::new (static_cast<void *>(slot)) T(std::move(value));

            producer_.writeIndex.store(
                next,
                std::memory_order_release);

            return true;
        }

        // Producer only. Construct directly in the next unpublished slot.
        // Callback returns true to publish, false to discard.
        // Never call concurrently with another producer operation.
        enum class ProduceResult { Published, Rejected, Full };

        template <typename Fill>
        [[nodiscard]]
        ProduceResult try_produce(Fill&& fill)
        {
            const std::size_t write =
                producer_.writeIndex.load(std::memory_order_relaxed);
            const std::size_t next = increment(write);

            if (next == producer_.cachedReadIndex)
            {
                producer_.cachedReadIndex =
                    consumer_.readIndex.load(std::memory_order_acquire);
                if (next == producer_.cachedReadIndex)
                    return ProduceResult::Full;
            }

            T* slot = ptr(write);
            ::new (static_cast<void*>(slot)) T{};

            // A failed receive must never publish partially written data.
            // This method is intended for noexcept receive callbacks.
            static_assert(noexcept(std::declval<Fill&>()(std::declval<T&>())),
                          "try_produce callback must be noexcept");
            if (!std::forward<Fill>(fill)(*slot))
            {
                slot->~T();
                return ProduceResult::Rejected;
            }

            producer_.writeIndex.store(next, std::memory_order_release);
            return ProduceResult::Published;
        }

        [[nodiscard]]
        std::optional<T> pop()
        {
            const std::size_t read =
                consumer_.readIndex.load(
                    std::memory_order_relaxed);

            if (read == consumer_.cachedWriteIndex)
            {
                consumer_.cachedWriteIndex =
                    producer_.writeIndex.load(
                        std::memory_order_acquire);

                if (read == consumer_.cachedWriteIndex)
                    return std::nullopt;
            }

            T *slot = ptr(read);

            std::optional<T> value{
                std::move(*slot)};

            slot->~T();

            consumer_.readIndex.store(
                increment(read),
                std::memory_order_release);

            return value;
        }

        [[nodiscard]]
        bool empty() const noexcept
        {
            const std::size_t read =
                consumer_.readIndex.load(
                    std::memory_order_relaxed);

            const std::size_t write =
                producer_.writeIndex.load(
                    std::memory_order_acquire);

            return read == write;
        }

        [[nodiscard]]
        bool full() const noexcept
        {
            const std::size_t write =
                producer_.writeIndex.load(
                    std::memory_order_relaxed);

            const std::size_t next =
                increment(write);

            const std::size_t read =
                consumer_.readIndex.load(
                    std::memory_order_acquire);

            return next == read;
        }

        [[nodiscard]]
        std::size_t size() const noexcept
        {
            const std::size_t write =
                producer_.writeIndex.load(
                    std::memory_order_acquire);

            const std::size_t read =
                consumer_.readIndex.load(
                    std::memory_order_acquire);

            if (write >= read)
            {
                return write - read;
            }

            return Capacity - read + write;
        }

        T *ptr(
            std::size_t index) noexcept
        {
            return std::launder(
                reinterpret_cast<T *>(
                    &storage_[index]));
        }

        ~SpscRingBuffer()
        {
            // The queue is expected to have no concurrent
            // producer/consumer activity during destruction.
            //
            // Destroy any objects that are still alive in the
            // preallocated slots.
            while (
                !empty())
            {
                (void)pop();
            }
        }

    private:
        [[nodiscard]]
        static constexpr std::size_t increment(
            std::size_t index) noexcept
        {
            return (
                       index + 1) %
                   Capacity;
        }

        // -----------------------------------------------------
        // Preallocated event storage
        // -----------------------------------------------------
        //
        // Previously this was an inline std::array of raw
        // bytes. Large queue capacities therefore made the
        // SpscRingBuffer object itself very large and could
        // exhaust the caller's stack.
        //
        // The storage is now allocated once on the heap.
        // Individual push/pop operations still perform no
        // allocation.
        //
        std::unique_ptr<Storage[]>
            storage_;

        struct alignas(64) ProducerState
        {
            std::atomic<std::size_t> writeIndex{0};
            std::size_t cachedReadIndex{0};
        };

        struct alignas(64) ConsumerState
        {
            std::atomic<std::size_t> readIndex{0};
            std::size_t cachedWriteIndex{0};
        };

        // Compile-time alignment checks
        static_assert(alignof(ProducerState) >= 64);
        static_assert(alignof(ConsumerState) >= 64);

        static_assert(sizeof(ProducerState) % 64 == 0);
        static_assert(sizeof(ConsumerState) % 64 == 0);

        ProducerState producer_;
        ConsumerState consumer_;
    };

} // namespace llt