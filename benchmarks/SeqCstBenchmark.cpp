#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

#include "SeqCstSpscRingBuffer.h"

int main()
{
    constexpr std::size_t messageCount = 10'000'000;

    benchmark::SeqCstSpscRingBuffer<
        std::uint64_t,
        4096
    > queue;

    std::thread producer(
        [&]()
        {
            for (std::uint64_t i = 0;
                 i < messageCount;)
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
        }
    );

    std::uint64_t expected = 0;

    const auto start =
        std::chrono::steady_clock::now();

    std::thread consumer(
        [&]()
        {
            while (expected < messageCount)
            {
                auto value = queue.pop();

                if (!value)
                {
                    std::this_thread::yield();
                    continue;
                }

                if (*value != expected)
                {
                    std::cerr
                        << "ERROR: expected "
                        << expected
                        << ", got "
                        << *value
                        << '\n';

                    std::terminate();
                }

                ++expected;
            }
        }
    );

    producer.join();
    consumer.join();

    const auto end =
        std::chrono::steady_clock::now();

    const auto elapsed =
        std::chrono::duration_cast<
            std::chrono::microseconds
        >(end - start);

    const double seconds =
        elapsed.count() / 1'000'000.0;

    const double throughput =
        messageCount / seconds;

    std::cout
        << "Messages: "
        << messageCount
        << '\n';

    std::cout
        << "Elapsed: "
        << seconds
        << " seconds\n";

    std::cout
        << "Throughput: "
        << throughput
        << " messages/sec\n";
}