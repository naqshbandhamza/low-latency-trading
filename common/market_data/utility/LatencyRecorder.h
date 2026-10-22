#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>

namespace llt::latency
{

    template <std::size_t Capacity = 65536>
    class LatencyRecorder
    {
        static_assert(Capacity > 0);

    public:
        void record(std::uint64_t durationNs) noexcept
        {
            ++totalCount_;

            if (durationNs > maximumNs_)
            {
                maximumNs_ = durationNs;
            }

            // Store up to Capacity samples.
            if (sampleCount_ < Capacity)
            {
                samples_[sampleCount_++] = durationNs;
            }
            else
            {
                ++discardedSamples_;
            }
        }

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

        [[nodiscard]]
        Summary summarize()
        {
            Summary result{};

            result.count = totalCount_;
            result.sampled = sampleCount_;
            result.discarded = discardedSamples_;
            result.maxNs = maximumNs_;

            if (sampleCount_ == 0)
            {
                return result;
            }

            // Sorting is done only when summarizing,
            // never on the hot path.
            std::sort(
                samples_.get(),
                samples_.get() + sampleCount_);

            const auto percentile =
                [this](std::uint64_t numerator,
                       std::uint64_t denominator)
            {
                const std::uint64_t rank =
                    (sampleCount_ * numerator +
                     denominator - 1) /
                    denominator;

                return samples_[rank - 1];
            };

            result.p50Ns = percentile(50, 100);
            result.p99Ns = percentile(99, 100);
            result.p999Ns = percentile(999, 1000);

            return result;
        }

    private:
        // std::array<std::uint64_t, Capacity> samples_{};
        std::unique_ptr<std::uint64_t[]> samples_{
            std::make_unique<std::uint64_t[]>(Capacity)};

        std::size_t sampleCount_{0};

        std::uint64_t totalCount_{0};
        std::uint64_t discardedSamples_{0};
        std::uint64_t maximumNs_{0};
    };

} // namespace llt::latency