#include "strategy/SimpleStrategy.h"

#include <type_traits>
#include <variant>

namespace llt
{

std::optional<OrderIntent>
SimpleStrategy::onMarketEvent(
    const MarketEvent& event
)
{
    return std::visit(
        [this](const auto& value)
            -> std::optional<OrderIntent>
        {
            using Event =
                std::decay_t<
                    decltype(value)
                >;

            if constexpr (
                std::is_same_v<
                    Event,
                    Quote
                >
            )
            {
                return onQuote(value);
            }
            else
            {
                return onTrade(value);
            }
        },
        event
    );
}


std::optional<OrderIntent>
SimpleStrategy::onQuote(
    const Quote& quote
)
{
    const auto bid =
        quote.bid().price().value();

    const auto ask =
        quote.ask().price().value();

    if (ask <= bid)
    {
        return std::nullopt;
    }

    const auto spread =
        ask - bid;

    if (spread > 10)
    {
        return std::nullopt;
    }

    return OrderIntent{
        quote.instrument(),
        Side::Buy,
        quote.ask().price(),
        Quantity(1)
    };
}


std::optional<OrderIntent>
SimpleStrategy::onTrade(
    const Trade&
)
{
    return std::nullopt;
}

} // namespace llt