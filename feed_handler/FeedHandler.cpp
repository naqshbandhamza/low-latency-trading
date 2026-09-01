#include "FeedHandler.h"

#include <thread>

namespace llt
{

FeedHandler::FeedHandler(
    ILogger& logger,
    MarketEventQueue& queue,
    IMarketDataSource& source
) noexcept
    : logger_(logger)
    , queue_(queue)
    , source_(source)
{
}

MarketEvent FeedHandler::createMarketEvent(
    const MarketDataMessage& message
)
{
    if (
        message.type
        == MarketDataMessageType::Quote
    )
    {
        return Quote(
            Instrument("TXFU6"),

            SequenceNumber(
                message.sequence
            ),

            Timestamp(
                message.timestamp
            ),

            Level(
                Price(message.bidPrice),
                Quantity(message.bidQuantity)
            ),

            Level(
                Price(message.askPrice),
                Quantity(message.askQuantity)
            )
        );
    }

    return Trade(
        Instrument("TXFU6"),

        SequenceNumber(
            message.sequence
        ),

        Timestamp(
            message.timestamp
        ),

        Price(message.price),

        Quantity(message.quantity),

        message.side
    );
}

void FeedHandler::start(
    std::size_t eventCount
)
{
    logger_.info(
        "Feed handler started"
    );

    MarketDataMessage message;

    for (
        std::size_t i = 0;
        i < eventCount;
        ++i
    )
    {
        if (!source_.receive(message))
        {
            continue;
        }

        MarketEvent event =
            createMarketEvent(message);

        while (!queue_.push(std::move(event)))
        {
            std::this_thread::yield();
        }
    }

    logger_.debug(
        "Feed handler stopped"
    );
}

} // namespace llt