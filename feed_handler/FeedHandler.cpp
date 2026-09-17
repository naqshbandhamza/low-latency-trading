#include "FeedHandler.h"
#include <string>
#include <thread>
#include <vector>
#include "logging/ILogger.h"
#include "market_data/IMarketDataSource.h"
#include "ring_buffer/SpscRingBuffer.h"
#include "market_data/ISequenceRecovery.h"
#include "market_data/MarketDataMessage.h"

namespace llt
{

FeedHandler::FeedHandler(
    ILogger& logger,
    MarketEventQueue& queue,
    IMarketDataSource& source,
    ISequenceRecovery& recovery
) noexcept
    : logger_(logger)
    , queue_(queue)
    , source_(source)
    , recovery_(recovery)
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

// void FeedHandler::start(
//     std::size_t eventCount
// )
// {
//     logger_.info(
//         "Feed handler started"
//     );

//     MarketDataMessage message;

//     // for (
//     //     std::size_t i = 0;
//     //     i < eventCount;
//     //     ++i
//     // )
//     // {
//     //     if (!source_.receive(message))
//     //     {
//     //         continue;
//     //     }

//     //     checkSequence(
//     //         message.sequence
//     //     );        

//     //     MarketEvent event =
//     //         createMarketEvent(message);

//     //     while (!queue_.push(std::move(event)))
//     //     {
//     //         std::this_thread::yield();
//     //     }
//     // }

//     std::size_t receivedEvents = 0;

//     while (receivedEvents < eventCount)
//     {
//         if (!source_.receive(message))
//         {
//             continue;
//         }

//         checkSequence(
//             message.sequence
//         );

//         MarketEvent event =
//             createMarketEvent(message);

//         while (!queue_.push(std::move(event)))
//         {
//             std::this_thread::yield();
//         }

//         ++receivedEvents;
//     }

//     logger_.debug(
//         "Feed handler stopped"
//     );
// }


// void FeedHandler::start(
//     std::size_t eventCount
// )
// {
//     logger_.info(
//         "Feed handler started"
//     );

//     MarketDataMessage message;

//     std::size_t receivedEvents = 0;

//     while (receivedEvents < eventCount)
//     {
//         if (!source_.receive(message))
//         {
//             continue;
//         }

//         if (!checkSequence(message.sequence))
//         {
//             break;
//         }

//         processMessage(message);

//         ++receivedEvents;
//     }

//     logger_.debug(
//         "Feed handler stopped"
//     );
// }


void FeedHandler::start(
    std::size_t eventCount
)
{
    logger_.info("Feed handler started");

    MarketDataMessage message;
    std::size_t receivedEvents = 0;

    while (receivedEvents < eventCount)
    {
        if (!source_.receive(message))
        {
            continue;
        }

        const auto sequenceResult =
            checkSequence(message.sequence);

        if (sequenceResult
            == SequenceCheckResult::Stop)
        {
            break;
        }

        if (sequenceResult
            == SequenceCheckResult::Ignore)
        {
            continue;
        }

        processMessage(message);
        ++receivedEvents;
    }

    logger_.debug("Feed handler stopped");
}


llt::SequenceCheckResult FeedHandler::checkSequence(
    std::uint64_t sequence
)
{
    if (!hasSequence_)
    {
        expectedSequence_ =
            sequence + 1;

        hasSequence_ = true;

        return SequenceCheckResult::Process;
    }

    // Expected sequence arrived.
    if (sequence == expectedSequence_)
    {
        expectedSequence_ =
            sequence + 1;

        return SequenceCheckResult::Process;

    }

    // A gap was detected.
    if (sequence > expectedSequence_)
    {
        std::vector<MarketDataMessage> recoveredMessages;

        const bool recovered =
            recovery_.recover(
                expectedSequence_,
                sequence,
                recoveredMessages
            );

        if (!recovered)
        {
            logger_.error(
                "Market data sequence recovery failed"
            );

            return SequenceCheckResult::Stop;
        }

        for (
            const auto& recoveredMessage
            : recoveredMessages
        )
        {
            processMessage(
                recoveredMessage
            );
        }

        expectedSequence_ =
            sequence + 1;

        return SequenceCheckResult::Process;
    }

    // Older / duplicate packet.
    return SequenceCheckResult::Ignore;
}

void FeedHandler::processMessage(
    const MarketDataMessage& message
)
{
    MarketEvent event =
        createMarketEvent(message);

    while (!queue_.push(std::move(event)))
    {
        std::this_thread::yield();
    }
}


} // namespace llt