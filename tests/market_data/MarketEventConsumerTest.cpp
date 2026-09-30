#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <cstdint>
#include <thread>

#include "market_data/MarketEventConsumer.h"
#include "market_data/MarketEventQueue.h"

TEST_CASE(
    "Market event consumer drains SPSC across thread boundary")
{
    llt::MarketEventQueue queue;

    std::atomic<bool> producerDone{
        false};

    llt::MarketEventConsumer consumer{
        queue};

    std::thread consumerThread{
        [&consumer, &producerDone]()
        {
            consumer.run(
                producerDone);
        }};

    constexpr std::uint64_t EventCount =
        100000;

    //
    // We don't want to test queue overflow here.
    //
    // This test verifies reliable transfer across
    // the producer/consumer thread boundary.
    //
    for (
        std::uint64_t i = 0;
        i < EventCount;
        ++i)
    {
        //
        // Default MarketEvent construction may not
        // be available because MarketEvent is a
        // variant.
        //
        // Use the normalized Quote type already
        // supported by the queue.
        //
        const llt::Quote quote{
            llt::Instrument{"TEST"},
            llt::SequenceNumber{i},
            llt::Timestamp{i},
            llt::Level{
                llt::Price{1000000},
                llt::Quantity{100}},
            llt::Level{
                llt::Price{1000100},
                llt::Quantity{100}}};

        llt::MarketEvent event{
            quote};

        //
        // This test requires lossless delivery.
        //
        // If the SPSC is temporarily full, wait
        // until the consumer frees a slot.
        //
        while (!queue.push(event))
        {
            std::this_thread::yield();
        }
    }

    producerDone.store(
        true,
        std::memory_order_release);

    consumerThread.join();

    REQUIRE(
        consumer.consumedEvents() ==
        EventCount);

    REQUIRE(
        queue.empty());
}