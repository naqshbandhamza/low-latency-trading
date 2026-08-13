#include <catch2/catch_test_macros.hpp>

#include "ring_buffer/SpscRingBuffer.h"

TEST_CASE("Ring buffer starts empty")
{
    llt::SpscRingBuffer<int, 5> queue;

    REQUIRE(queue.empty());
    REQUIRE_FALSE(queue.full());
    REQUIRE(queue.size() == 0);
}

TEST_CASE("Ring buffer pushes and pops")
{
    llt::SpscRingBuffer<int, 5> queue;

    REQUIRE(queue.push(10));
    REQUIRE(queue.push(20));

    REQUIRE(queue.size() == 2);

    auto first = queue.pop();

    REQUIRE(first.has_value());
    REQUIRE(first.value() == 10);

    auto second = queue.pop();

    REQUIRE(second.has_value());
    REQUIRE(second.value() == 20);

    REQUIRE(queue.empty());
}

TEST_CASE("Ring buffer detects full state")
{
    llt::SpscRingBuffer<int, 3> queue;

    REQUIRE(queue.push(10));
    REQUIRE(queue.push(20));

    REQUIRE(queue.full());

    REQUIRE_FALSE(queue.push(30));

    REQUIRE(queue.size() == 2);
}

TEST_CASE("Ring buffer cannot pop from empty buffer")
{
    llt::SpscRingBuffer<int, 3> queue;

    auto result = queue.pop();

    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("Ring buffer wraps around")
{
    llt::SpscRingBuffer<int, 5> queue;

    REQUIRE(queue.push(1));
    REQUIRE(queue.push(2));
    REQUIRE(queue.push(3));
    REQUIRE(queue.push(4));

    REQUIRE(queue.pop().value() == 1);
    REQUIRE(queue.pop().value() == 2);

    REQUIRE(queue.push(5));
    REQUIRE(queue.push(6));

    REQUIRE(queue.pop().value() == 3);
    REQUIRE(queue.pop().value() == 4);
    REQUIRE(queue.pop().value() == 5);
    REQUIRE(queue.pop().value() == 6);

    REQUIRE(queue.empty());
}