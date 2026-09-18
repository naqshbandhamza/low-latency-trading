#pragma once

#include <cstddef>
#include <cstdint>

#include "market_data/MarketEvent.h"

namespace llt
{

    enum class SequenceCheckResult;

    class ILogger;
    class IMarketDataSource;
    class ISequenceRecovery;
    class MarketDataMessage;
    template <typename T, std::size_t Capacity>
    class SpscRingBuffer;

    using MarketEventQueue =
        SpscRingBuffer<MarketEvent, 4096>;

    class FeedHandler
    {
    public:
        FeedHandler(
            ILogger &logger,
            MarketEventQueue &queue,
            IMarketDataSource &source,
            ISequenceRecovery &recovery) noexcept;

        void start(
            std::size_t eventCount);

    private:
        MarketEvent createMarketEvent(
            const MarketDataMessage &message);

        void processMessage(
            const MarketDataMessage &message);

        SequenceCheckResult checkSequence(
            std::uint64_t sequence);

    private:
        ILogger &logger_;
        MarketEventQueue &queue_;
        IMarketDataSource &source_;
        ISequenceRecovery &recovery_;

        std::uint64_t expectedSequence_{0};
        bool hasSequence_{false};
    };

} // namespace llt