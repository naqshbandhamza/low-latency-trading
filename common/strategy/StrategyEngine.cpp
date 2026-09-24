#include "strategy/StrategyEngine.h"

#include <thread>
#include <utility>

namespace llt
{

StrategyEngine::StrategyEngine(
    MarketEventQueue& marketEventQueue,
    OrderIntentQueue& orderIntentQueue,
    IStrategy& strategy
) noexcept
    : marketEventQueue_(
        marketEventQueue
    )
    , orderIntentQueue_(
        orderIntentQueue
    )
    , strategy_(
        strategy
    )
{
}


void StrategyEngine::run()
{
    state_.store(
        StrategyEngineState::Running,
        std::memory_order_release
    );

    running_.store(
        true,
        std::memory_order_release
    );

    while (
        running_.load(
            std::memory_order_acquire
        )
    )
    {
        auto event =
            marketEventQueue_.pop();

        if (!event.has_value())
        {
            std::this_thread::yield();
            continue;
        }

        auto intent =
            strategy_.onMarketEvent(
                *event
            );

        if (!intent.has_value())
        {
            continue;
        }

        if (
            !publishIntent(
                std::move(*intent)
            )
        )
        {
            break;
        }
    }

    running_.store(
        false,
        std::memory_order_release
    );

    if (
        state_.load(
            std::memory_order_acquire
        )
        != StrategyEngineState::Failed
    )
    {
        state_.store(
            StrategyEngineState::Stopped,
            std::memory_order_release
        );
    }
}


bool StrategyEngine::publishIntent(
    OrderIntent&& intent
)
{
    while (
        !orderIntentQueue_.push(
            std::move(intent)
        )
    )
    {
        if (
            !running_.load(
                std::memory_order_acquire
            )
        )
        {
            return false;
        }

        std::this_thread::yield();
    }

    return true;
}


void StrategyEngine::stop() noexcept
{
    running_.store(
        false,
        std::memory_order_release
    );
}


StrategyEngineState
StrategyEngine::state() const noexcept
{
    return state_.load(
        std::memory_order_acquire
    );
}

} // namespace llt