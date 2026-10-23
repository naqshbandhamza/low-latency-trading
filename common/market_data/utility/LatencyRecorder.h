// #pragma once

// #include <algorithm>
// #include <array>
// #include <cstddef>
// #include <cstdint>
// #include <limits>
// #include <memory>

// namespace llt::latency
// {

//     template <std::size_t Capacity = 65536>
//     class LatencyRecorder
//     {
//         static_assert(Capacity > 0);

//     public:
//         void record(std::uint64_t durationNs) noexcept
//         {
//             ++totalCount_;

//             if (durationNs > maximumNs_)
//             {
//                 maximumNs_ = durationNs;
//             }

//             // Store up to Capacity samples.
//             if (sampleCount_ < Capacity)
//             {
//                 samples_[sampleCount_++] = durationNs;
//             }
//             else
//             {
//                 ++discardedSamples_;
//             }
//         }

//         struct Summary
//         {
//             std::uint64_t count = 0;
//             std::uint64_t sampled = 0;
//             std::uint64_t discarded = 0;

//             std::uint64_t p50Ns = 0;
//             std::uint64_t p99Ns = 0;
//             std::uint64_t p999Ns = 0;
//             std::uint64_t maxNs = 0;
//         };

//         [[nodiscard]]
//         Summary summarize()
//         {
//             Summary result{};

//             result.count = totalCount_;
//             result.sampled = sampleCount_;
//             result.discarded = discardedSamples_;
//             result.maxNs = maximumNs_;

//             if (sampleCount_ == 0)
//             {
//                 return result;
//             }

//             // Sorting is done only when summarizing,
//             // never on the hot path.
//             std::sort(
//                 samples_.get(),
//                 samples_.get() + sampleCount_);

//             const auto percentile =
//                 [this](std::uint64_t numerator,
//                        std::uint64_t denominator)
//             {
//                 const std::uint64_t rank =
//                     (sampleCount_ * numerator +
//                      denominator - 1) /
//                     denominator;

//                 return samples_[rank - 1];
//             };

//             result.p50Ns = percentile(50, 100);
//             result.p99Ns = percentile(99, 100);
//             result.p999Ns = percentile(999, 1000);

//             return result;
//         }

//     private:
//         // std::array<std::uint64_t, Capacity> samples_{};
//         std::unique_ptr<std::uint64_t[]> samples_{
//             std::make_unique<std::uint64_t[]>(Capacity)};

//         std::size_t sampleCount_{0};

//         std::uint64_t totalCount_{0};
//         std::uint64_t discardedSamples_{0};
//         std::uint64_t maximumNs_{0};
//     };

// } // namespace llt::latency






#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace llt::latency
{

template <std::size_t Capacity = 65536>
class LatencyRecorder
{
public:
    // 64 logarithmic ranges, each with 64 subdivisions.
    //
    // For values below 64 ns, every integer nanosecond
    // is represented exactly.
    //
    // At larger durations, bucket resolution is
    // approximately 1.56% of the range's lower bound.
    //
    // Capacity is retained for compatibility with
    // the previous LatencyRecorder<Capacity> API.

    static constexpr std::size_t SubBuckets = 64;
    static constexpr std::size_t Exponents = 64;

    static constexpr std::size_t BucketCount =
        Exponents * SubBuckets;

    struct Summary
    {
        std::uint64_t count = 0;
        std::uint64_t sampled = 0;
        std::uint64_t discarded = 0;

        std::uint64_t p50Ns = 0;
        std::uint64_t p99Ns = 0;
        std::uint64_t p999Ns = 0;
        std::uint64_t maxNs = 0;
    };

    // Called on the hot path.
    //
    // No heap allocations.
    // No locks.
    // No sorting.
    //
    // Requires a single recording thread per instance.
    void record(std::uint64_t durationNs) noexcept
    {
        ++totalCount_;

        if (durationNs > maximumNs_)
        {
            maximumNs_ = durationNs;
        }

        ++histogram_[bucketIndex(durationNs)];
    }

    // Call only after recording has finished.
    [[nodiscard]]
    Summary summarize() const noexcept
    {
        Summary result{};

        result.count = totalCount_;
        result.sampled = totalCount_;
        result.discarded = 0;
        result.maxNs = maximumNs_;

        if (totalCount_ == 0)
        {
            return result;
        }

        result.p50Ns = percentile(50, 100);
        result.p99Ns = percentile(99, 100);
        result.p999Ns = percentile(999, 1000);

        return result;
    }

private:
    // Returns floor(log2(value)).
    //
    // Constant-time on GCC/Clang, including macOS Clang.
    [[nodiscard]]
    static unsigned log2Floor(
        std::uint64_t value) noexcept
    {
#if defined(__GNUC__) || defined(__clang__)
        return 63u -
            static_cast<unsigned>(
                __builtin_clzll(value));
#else
        unsigned exponent = 0;

        while (value >>= 1)
        {
            ++exponent;
        }

        return exponent;
#endif
    }

    [[nodiscard]]
    static std::size_t bucketIndex(
        std::uint64_t value) noexcept
    {
        // Dedicated bucket for zero.
        if (value == 0)
        {
            return 0;
        }

        const unsigned exponent = log2Floor(value);

        const std::uint64_t base =
            std::uint64_t{1} << exponent;

        const std::uint64_t offset =
            value - base;

        std::size_t subdivision = 0;

        if (exponent < 6)
        {
            // Exact integer-nanosecond resolution
            // for all values below 64 ns.
            subdivision =
                static_cast<std::size_t>(offset);
        }
        else
        {
            // Width is a power of two.
            //
            // Avoids division, multiplication overflow,
            // and floating-point arithmetic.
            subdivision =
                static_cast<std::size_t>(
                    offset >> (exponent - 6));
        }

        return static_cast<std::size_t>(exponent)
                   * SubBuckets
               + subdivision;
    }

    // Return the upper bound represented by a bucket.
    //
    // Percentiles are reported conservatively using
    // the upper bound of the selected histogram bucket.
    [[nodiscard]]
    static std::uint64_t bucketUpperBound(
        std::size_t index) noexcept
    {
        const unsigned exponent =
            static_cast<unsigned>(
                index / SubBuckets);

        const std::uint64_t subdivision =
            static_cast<std::uint64_t>(
                index % SubBuckets);

        const std::uint64_t base =
            std::uint64_t{1} << exponent;

        if (exponent < 6)
        {
            return base + subdivision;
        }

        const std::uint64_t width =
            std::uint64_t{1} << (exponent - 6);

        // Compute the upper bound without overflow.
        //
        // The final bucket in exponent 63 must
        // saturate at UINT64_MAX.
        if (exponent == 63 && subdivision == 63)
        {
            return std::numeric_limits<
                std::uint64_t>::max();
        }

        return base +
            (subdivision + 1) * width - 1;
    }

    [[nodiscard]]
    std::uint64_t percentile(
        std::uint64_t numerator,
        std::uint64_t denominator) const noexcept
    {
        // Ceiling of count * numerator / denominator,
        // avoiding multiplication overflow.
        const std::uint64_t rank =
            (totalCount_ / denominator) * numerator +
            (
                (totalCount_ % denominator) * numerator +
                denominator - 1
            ) / denominator;

        std::uint64_t cumulative = 0;

        for (std::size_t i = 0;
             i < BucketCount;
             ++i)
        {
            cumulative += histogram_[i];

            if (cumulative >= rank)
            {
                return bucketUpperBound(i);
            }
        }

        return maximumNs_;
    }

private:
    // 4096 counters × 8 bytes = 32 KiB.
    //
    // Storage is allocated as part of the recorder
    // object, not during record().
    std::array<std::uint64_t, BucketCount>
        histogram_{};

    std::uint64_t totalCount_{0};
    std::uint64_t maximumNs_{0};
};

} // namespace llt::latency
