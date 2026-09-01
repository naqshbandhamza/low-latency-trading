#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

#include "market_data/MarketEvent.h"
#include "ring_buffer/SpscRingBuffer.h"

int main()
{
    constexpr std::size_t messageCount = 10'000'000;

    using MarketEvent = llt::MarketEvent;

    llt::SpscRingBuffer<MarketEvent, 4096> queue;

    std::thread producer(
        [&]()
        {
            for (std::uint64_t i = 0; i < messageCount;)
            {
                MarketEvent event = [&]()
                {
                    if ((i & 1) == 0)
                    {
                        return MarketEvent{
                            llt::Quote(
                                // "TXFU6",
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
                        };
                    }

                    return MarketEvent{
                        llt::Trade(
                            // "TXFU6",
                            llt::Instrument("TXFU6"),
                            llt::SequenceNumber(i),
                            llt::Timestamp(i),
                            llt::Price(234505),
                            llt::Quantity(3),
                            llt::Side::Buy
                        )
                    };
                }();

                if (queue.push(std::move(event)))
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
    std::uint64_t quoteCount = 0;
    std::uint64_t tradeCount = 0;

    const auto start =
        std::chrono::steady_clock::now();

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

                const auto& marketEvent = *event;

                std::visit(
                    [&](const auto& value)
                    {
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
                    marketEvent
                );

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

    const double messagesPerSecond =
        messageCount / seconds;

    std::cout
        << "========================================\n"
        << "MarketEvent SPSC Benchmark\n"
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

