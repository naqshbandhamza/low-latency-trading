#include "FeedHandler.h"

#include <string>
#include <thread>
#include <vector>

#include "logging/ILogger.h"
#include "market_data/IMarketDataSource.h"
#include "market_data/ISequenceRecovery.h"
#include "market_data/MarketDataMessage.h"
#include "ring_buffer/SpscRingBuffer.h"
#include "types/SequenceCheckResult.h"

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


void FeedHandler::start(
    std::size_t eventCount
)
{
    logger_.info(
        "Feed handler started"
    );

    state_.store(
        FeedHandlerState::Running,
        std::memory_order_release
    );

    running_.store(
        true,
        std::memory_order_release
    );

    MarketDataMessage message;
    std::size_t receivedEvents = 0;

    while (
        running_.load(
            std::memory_order_acquire
        )
        && receivedEvents < eventCount
    )
    {
        if (!source_.receive(message))
        {
            continue;
        }

        const auto sequenceResult =
            checkSequence(
                message.sequence
            );

        if (
            sequenceResult
            == SequenceCheckResult::Stop
        )
        {
            state_.store(
                FeedHandlerState::Failed,
                std::memory_order_release
            );

            break;
        }

        if (
            sequenceResult
            == SequenceCheckResult::Ignore
        )
        {
            continue;
        }

        if (!processMessage(message))
        {
            break;
        }

        ++receivedEvents;
    }

    running_.store(
        false,
        std::memory_order_release
    );

    if (
        state_.load(
            std::memory_order_acquire
        )
        != FeedHandlerState::Failed
    )
    {
        state_.store(
            FeedHandlerState::Stopped,
            std::memory_order_release
        );
    }

    logger_.debug(
        "Feed handler stopped"
    );
}


SequenceCheckResult FeedHandler::checkSequence(
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
        std::vector<MarketDataMessage>
            recoveredMessages;

        const SequenceCheckResult recovered =
            recovery_.recover(
                expectedSequence_,
                sequence,
                recoveredMessages
            );

        if (
            recovered
            == SequenceCheckResult::Stop
        )
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
            if (
                !processMessage(
                    recoveredMessage
                )
            )
            {
                // Publication was interrupted because the
                // FeedHandler is shutting down.
                //
                // This is not a sequence-recovery failure.
                if (
                    !running_.load(
                        std::memory_order_acquire
                    )
                )
                {
                    return SequenceCheckResult::Ignore;
                }

                return SequenceCheckResult::Stop;
            }
        }

        expectedSequence_ =
            sequence + 1;

        return SequenceCheckResult::Process;
    }

    // Older / duplicate packet.
    return SequenceCheckResult::Ignore;
}


bool FeedHandler::processMessage(
    const MarketDataMessage& message
)
{
    MarketEvent event =
        createMarketEvent(message);

    while (!queue_.push(std::move(event)))
    {
        // Never allow a full SPSC queue to prevent the
        // FeedHandler from shutting down.
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


void FeedHandler::run()
{
    logger_.info(
        "Feed handler started"
    );

    state_.store(
        FeedHandlerState::Running,
        std::memory_order_release
    );

    running_.store(
        true,
        std::memory_order_release
    );

    MarketDataMessage message;

    while (
        running_.load(
            std::memory_order_acquire
        )
    )
    {
        if (!source_.receive(message))
        {
            continue;
        }

        const auto sequenceResult =
            checkSequence(
                message.sequence
            );

        if (
            sequenceResult
            == SequenceCheckResult::Stop
        )
        {
            state_.store(
                FeedHandlerState::Failed,
                std::memory_order_release
            );

            break;
        }

        if (
            sequenceResult
            == SequenceCheckResult::Ignore
        )
        {
            continue;
        }

        if (!processMessage(message))
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
        != FeedHandlerState::Failed
    )
    {
        state_.store(
            FeedHandlerState::Stopped,
            std::memory_order_release
        );
    }

    logger_.debug(
        "Feed handler stopped"
    );
}


FeedHandlerState FeedHandler::state() const noexcept
{
    return state_.load(
        std::memory_order_acquire
    );
}


void FeedHandler::stop() noexcept
{
    running_.store(
        false,
        std::memory_order_release
    );
}

} // namespace llt