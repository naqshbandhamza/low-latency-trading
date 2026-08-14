#include <atomic>
#include <cstddef>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("size_t atomic is lock-free")
{
    STATIC_REQUIRE(
        std::atomic<std::size_t>::is_always_lock_free
    );
}