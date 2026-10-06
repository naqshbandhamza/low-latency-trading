// #pragma once

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
            "Capacity must be greater than zero"
        );

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
                alignof(T)
            >;

    public:

        SpscRingBuffer()
            : storage_(
                  std::make_unique<Storage[]>(
                      Capacity
                  )
              )
        {
        }


        SpscRingBuffer(
            const SpscRingBuffer&
        ) = delete;

        SpscRingBuffer&
        operator=(
            const SpscRingBuffer&
        ) = delete;

        SpscRingBuffer(
            SpscRingBuffer&&
        ) = delete;

        SpscRingBuffer&
        operator=(
            SpscRingBuffer&&
        ) = delete;


        [[nodiscard]]
        bool push(
            const T& value
        )
        {
            const std::size_t write =
                writeIndex_.load(
                    std::memory_order_relaxed
                );

            const std::size_t next =
                increment(write);

            // Use the producer-local cached copy of the
            // consumer's read position first.
            //
            // Only touch the consumer-owned atomic when the
            // queue appears full according to our cached view.
            if (
                next
                == cachedReadIndex_
            )
            {
                cachedReadIndex_ =
                    readIndex_.load(
                        std::memory_order_acquire
                    );

                if (
                    next
                    == cachedReadIndex_
                )
                {
                    return false;
                }
            }

            T* slot =
                ptr(write);

            ::new (
                static_cast<void*>(slot)
            ) T(value);

            // Publish the fully constructed object to the
            // consumer.
            writeIndex_.store(
                next,
                std::memory_order_release
            );

            return true;
        }


        [[nodiscard]]
        bool push(
            T&& value
        )
        {
            const std::size_t write =
                writeIndex_.load(
                    std::memory_order_relaxed
                );

            const std::size_t next =
                increment(write);

            // Use the producer-local cached read position.
            //
            // Refresh from the consumer only when the queue
            // appears full.
            if (
                next
                == cachedReadIndex_
            )
            {
                cachedReadIndex_ =
                    readIndex_.load(
                        std::memory_order_acquire
                    );

                if (
                    next
                    == cachedReadIndex_
                )
                {
                    return false;
                }
            }

            T* slot =
                ptr(write);

            ::new (
                static_cast<void*>(slot)
            ) T(
                std::move(value)
            );

            // Make the constructed object visible to the
            // consumer.
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
                readIndex_.load(
                    std::memory_order_relaxed
                );

            // Use the consumer-local cached copy of the
            // producer's write position first.
            //
            // Only touch the producer-owned atomic when the
            // queue appears empty according to our cached view.
            if (
                read
                == cachedWriteIndex_
            )
            {
                cachedWriteIndex_ =
                    writeIndex_.load(
                        std::memory_order_acquire
                    );

                if (
                    read
                    == cachedWriteIndex_
                )
                {
                    return std::nullopt;
                }
            }

            T* slot =
                ptr(read);

            std::optional<T> value{
                std::move(*slot)
            };

            // Destroy the object occupying this raw slot.
            slot->~T();

            // Release the slot back to the producer.
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
                readIndex_.load(
                    std::memory_order_relaxed
                );

            const std::size_t write =
                writeIndex_.load(
                    std::memory_order_acquire
                );

            return read == write;
        }


        [[nodiscard]]
        bool full() const noexcept
        {
            const std::size_t write =
                writeIndex_.load(
                    std::memory_order_relaxed
                );

            const std::size_t next =
                increment(write);

            const std::size_t read =
                readIndex_.load(
                    std::memory_order_acquire
                );

            return next == read;
        }


        [[nodiscard]]
        std::size_t size() const noexcept
        {
            const std::size_t write =
                writeIndex_.load(
                    std::memory_order_acquire
                );

            const std::size_t read =
                readIndex_.load(
                    std::memory_order_acquire
                );

            if (
                write >= read
            )
            {
                return write - read;
            }

            return Capacity
                - read
                + write;
        }


        T* ptr(
            std::size_t index
        ) noexcept
        {
            return std::launder(
                reinterpret_cast<T*>(
                    &storage_[index]
                )
            );
        }


        ~SpscRingBuffer()
        {
            // The queue is expected to have no concurrent
            // producer/consumer activity during destruction.
            //
            // Destroy any objects that are still alive in the
            // preallocated slots.
            while (
                !empty()
            )
            {
                (void)pop();
            }
        }


    private:

        [[nodiscard]]
        static constexpr
        std::size_t increment(
            std::size_t index
        ) noexcept
        {
            return (
                index + 1
            ) % Capacity;
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


        // -----------------------------------------------------
        // Producer-owned position
        // -----------------------------------------------------
        //
        // Written by producer.
        // Observed by consumer.
        //
        // Keep it on its own cache line so that producer and
        // consumer do not continuously invalidate a shared
        // cache line merely by advancing their own positions.
        //
        alignas(64)
        std::atomic<std::size_t>
            writeIndex_{0};


        // -----------------------------------------------------
        // Consumer-owned position
        // -----------------------------------------------------
        //
        // Written by consumer.
        // Observed by producer.
        //
        alignas(64)
        std::atomic<std::size_t>
            readIndex_{0};


        // -----------------------------------------------------
        // Producer-local cached consumer position
        // -----------------------------------------------------
        //
        // Only the producer accesses this value.
        //
        // It may safely be stale. When it makes the queue
        // appear full, the producer refreshes readIndex_ using
        // an acquire load.
        //
        std::size_t
            cachedReadIndex_{0};


        // -----------------------------------------------------
        // Consumer-local cached producer position
        // -----------------------------------------------------
        //
        // Only the consumer accesses this value.
        //
        // It may safely be stale. When it makes the queue
        // appear empty, the consumer refreshes writeIndex_
        // using an acquire load.
        //
        std::size_t
            cachedWriteIndex_{0};
    };

} // namespace llt