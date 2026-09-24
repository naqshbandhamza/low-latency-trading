#pragma once

#include <atomic>

#include "market_data/MarketEvent.h"
#include "ring_buffer/SpscRingBuffer.h"
#include "strategy/IStrategy.h"
#include "strategy/OrderIntentQueue.h"
#include "strategy/StrategyEngineState.h"

namespace llt
{


    template <typename T, std::size_t Capacity>
    class SpscRingBuffer;

    using MarketEventQueue =
        SpscRingBuffer<MarketEvent, 4096>;


class StrategyEngine
{
public:

    StrategyEngine(
        MarketEventQueue& marketEventQueue,
        OrderIntentQueue& orderIntentQueue,
        IStrategy& strategy
    ) noexcept;

    void run();

    void stop() noexcept;

    [[nodiscard]]
    StrategyEngineState
    state() const noexcept;

private:

    bool publishIntent(
        OrderIntent&& intent
    );

    MarketEventQueue&
        marketEventQueue_;

    OrderIntentQueue&
        orderIntentQueue_;

    IStrategy&
        strategy_;

    std::atomic<bool>
        running_{false};

    std::atomic<StrategyEngineState>
        state_{
            StrategyEngineState::Stopped
        };
};

} // namespace llt