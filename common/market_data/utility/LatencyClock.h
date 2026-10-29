#pragma once

#include <chrono>
#include <cstdint>

namespace llt::latency
{
    [[nodiscard]]
    inline std::uint64_t nowNs() noexcept
    {
        using Clock = std::chrono::steady_clock;

        return static_cast<std::uint64_t>(
            std::chrono::duration_cast<
                std::chrono::nanoseconds>(
                Clock::now().time_since_epoch())
                .count());
    }
}