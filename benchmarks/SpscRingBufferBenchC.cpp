
#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>
#include <type_traits>
#include <vector>

#include "market_data/MarketEvent.h"
#include "ring_buffer/SpscRingBuffer.h"

int main()
{
    constexpr std::size_t messageCount = 10'000'000;
    constexpr std::size_t queueCapacity = 4096;

    using MarketEvent = llt::MarketEvent;

    // ============================================================
    // Pre-create all MarketEvents BEFORE the benchmark starts.
    // ============================================================

    std::vector<MarketEvent> events;
    events.reserve(messageCount);

    for (std::uint64_t i = 0; i < messageCount; ++i)
    {
        if ((i & 1) == 0)
        {
            events.emplace_back(
                llt::Quote(
                    llt::Instrument("TXFU6"),
                    llt::SequenceNumber(i),
                    llt::Timestamp(i),
                    llt::Level(
                        llt::Price(234500),
                        llt::Quantity(10)
                    ),
                    llt::Level(
                        llt::Price(234510),
                        llt::Quantity(12)
                    )
                )
            );
        }
        else
        {
            events.emplace_back(
                llt::Trade(
                    llt::Instrument("TXFU6"),
                    llt::SequenceNumber(i),
                    llt::Timestamp(i),
                    llt::Price(234505),
                    llt::Quantity(3),
                    llt::Side::Buy
                )
            );
        }
    }

    llt::SpscRingBuffer<
        MarketEvent,
        queueCapacity
    > queue;

    std::uint64_t expected = 0;

    std::size_t quoteCount = 0;
    std::size_t tradeCount = 0;

    // ============================================================
    // Benchmark starts HERE.
    // ============================================================

    const auto start =
        std::chrono::steady_clock::now();

    std::thread producer(
        [&]()
        {
            for (std::size_t i = 0;
                 i < messageCount;)
            {
                if (queue.push(std::move(events[i])))
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

    std::thread consumer(
        [&]()
        {
            while (expected < messageCount)
            {
                auto event = queue.pop();

                if (!event)
                {
                    std::this_thread::yield();
                    continue;
                }

                std::visit(
                    [&](const auto& value)
                    {
                        // Verify ordering.
                        if (value.sequence()
                            != llt::SequenceNumber(expected))
                        {
                            std::cerr
                                << "ERROR: expected sequence "
                                << expected
                                << '\n';

                            std::terminate();
                        }

                        using EventType =
                            std::decay_t<decltype(value)>;

                        if constexpr (
                            std::is_same_v<
                                EventType,
                                llt::Quote
                            >
                        )
                        {
                            ++quoteCount;
                        }
                        else if constexpr (
                            std::is_same_v<
                                EventType,
                                llt::Trade
                            >
                        )
                        {
                            ++tradeCount;
                        }
                    },
                    *event
                );

                ++expected;
            }
        }
    );

    producer.join();
    consumer.join();

    const auto end =
        std::chrono::steady_clock::now();

    // ============================================================
    // Results
    // ============================================================

    const auto elapsed =
        std::chrono::duration_cast<
            std::chrono::microseconds
        >(end - start);

    const double seconds =
        elapsed.count() / 1'000'000.0;

    const double messagesPerSecond =
        messageCount / seconds;

    std::cout
        << "========================================\n"
        << "Pre-created MarketEvent SPSC Benchmark\n"
        << "========================================\n";

    std::cout
        << "Messages: "
        << messageCount
        << '\n';

    std::cout
        << "Quotes: "
        << quoteCount
        << '\n';

    std::cout
        << "Trades: "
        << tradeCount
        << '\n';

    std::cout
        << "Elapsed: "
        << seconds
        << " seconds\n";

    std::cout
        << "Throughput: "
        << messagesPerSecond
        << " messages/sec\n";

    std::cout
        << "========================================\n";
}

