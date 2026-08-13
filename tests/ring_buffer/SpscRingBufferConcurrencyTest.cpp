#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <thread>

#include "ring_buffer/SpscRingBuffer.h"

TEST_CASE("SPSC ring buffer works between producer and consumer")
{
    constexpr int count = 100000;

    llt::SpscRingBuffer<int, 1024> queue;

    std::atomic<bool> producerFinished{false};

    std::thread producer(
        [&]()
        {
            for (int i = 0; i < count;)
            {
                if (queue.push(i))
                {
                    ++i;
                }
                else
                {
                    std::this_thread::yield();
                }
            }

            producerFinished.store(
                true,
                std::memory_order_release
            );
        }
    );

    std::thread consumer(
        [&]()
        {
            int expected = 0;

            while (expected < count)
            {
                auto value = queue.pop();

                if (!value)
                {
                    std::this_thread::yield();
                    continue;
                }

                REQUIRE(*value == expected);

                ++expected;
            }
        }
    );

    producer.join();
    consumer.join();

    REQUIRE(
        producerFinished.load(
            std::memory_order_acquire
        )
    );

    REQUIRE(queue.empty());
}