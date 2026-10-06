#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <limits>
#include <chrono>
#include <thread>

#include <catch2/catch_test_macros.hpp>

#include "FeedHandler.h"
#include "logging/ILogger.h"
#include "logging/ConsoleLogger.h"
#include "market_data/MarketEvent.h"
#include "MockMarketDataSource.h"
#include "market_data/SequenceRecovery.h"
#include "MockSequenceRecovery.h"
#include "MockMarketDataRecoverySource.h"
#include "market_data/MarketDataMessage.h"
#include "ring_buffer/SpscRingBuffer.h"
#include "types/SequenceCheckResult.h"

namespace
{

} // namespace


class SequenceGapMarketDataSource
    : public llt::IMarketDataSource
{
public:

    bool receive(
        llt::MarketDataMessage& message
    ) noexcept override
    {
        if (index_ >= 3)
        {
            return false;
        }

        constexpr std::uint64_t sequences[] =
        {
            100,
            101,
            103
        };

        message.type =
            llt::MarketDataMessageType::Quote;

        message.sequence =
            sequences[index_];

        message.timestamp =
            sequences[index_];

        message.bidPrice =
            234500;

        message.bidQuantity =
            10;

        message.askPrice =
            234510;

        message.askQuantity =
            12;

        ++index_;

        return true;
    }

private:

    std::size_t index_{0};
};


TEST_CASE(
    "FeedHandler publishes Quote and Trade market events"
)
{
    constexpr std::size_t eventCount = 100;

    llt::ConsoleLogger logger;

    llt::MarketEventQueue queue;

    llt::MockMarketDataSource source;

    MockMarketDataRecoverySource rsource;

    llt::SequenceRecovery recovery(
        logger,
        rsource
    );

    llt::FeedHandler handler(
        logger,
        queue,
        source,
        recovery
    );

    handler.start(eventCount);

    REQUIRE(
        queue.size() == eventCount
    );

    for (
        std::uint64_t i = 0;
        i < eventCount;
        ++i
    )
    {
        auto event = queue.pop();

        REQUIRE(event.has_value());

        std::visit(
            [&](const auto& value)
            {
                REQUIRE(
                    value.sequence()
                    == llt::SequenceNumber(i)
                );
            },
            *event
        );
    }

    REQUIRE(queue.empty());
}


TEST_CASE(
    "FeedHandler converts Quote messages"
)
{
    llt::ConsoleLogger logger;

    llt::MarketEventQueue queue;

    llt::MockMarketDataSource source;

    MockMarketDataRecoverySource rsource;

    llt::SequenceRecovery recovery(
        logger,
        rsource
    );

    llt::FeedHandler handler(
        logger,
        queue,
        source,
        recovery
    );

    handler.start(1);

    auto event = queue.pop();

    REQUIRE(event.has_value());

    REQUIRE(
        std::holds_alternative<llt::Quote>(*event)
    );

    const auto& quote =
        std::get<llt::Quote>(*event);

    REQUIRE(
        quote.instrument()
        == llt::Instrument("TXFU6")
    );

    REQUIRE(
        quote.sequence()
        == llt::SequenceNumber(0)
    );

    REQUIRE(
        quote.bid().price()
        == llt::Price(234500)
    );

    REQUIRE(
        quote.bid().quantity()
        == llt::Quantity(10)
    );

    REQUIRE(
        quote.ask().price()
        == llt::Price(234510)
    );

    REQUIRE(
        quote.ask().quantity()
        == llt::Quantity(12)
    );
}


TEST_CASE(
    "FeedHandler converts Trade messages"
)
{
    llt::ConsoleLogger logger;

    llt::MarketEventQueue queue;

    llt::MockMarketDataSource source;

    MockMarketDataRecoverySource rsource;

    llt::SequenceRecovery recovery(
        logger,
       rsource
    );

    llt::FeedHandler handler(
        logger,
        queue,
        source,
        recovery
    );

    // First event = Quote
    // Second event = Trade
    handler.start(2);

    auto quoteEvent = queue.pop();
    auto tradeEvent = queue.pop();

    REQUIRE(quoteEvent.has_value());
    REQUIRE(tradeEvent.has_value());

    REQUIRE(
        std::holds_alternative<llt::Trade>(*tradeEvent)
    );

    const auto& trade =
        std::get<llt::Trade>(*tradeEvent);

    REQUIRE(
        trade.instrument()
        == llt::Instrument("TXFU6")
    );

    REQUIRE(
        trade.sequence()
        == llt::SequenceNumber(1)
    );

    REQUIRE(
        trade.price()
        == llt::Price(234505)
    );

    REQUIRE(
        trade.quantity()
        == llt::Quantity(3)
    );

    REQUIRE(
        trade.side()
        == llt::Side::Buy
    );

    REQUIRE(queue.empty());
}


TEST_CASE(
    "FeedHandler recovers missing sequence"
)
{
    llt::ConsoleLogger logger;

    llt::MarketEventQueue queue;

    SequenceGapMarketDataSource source;

    MockSequenceRecovery recovery;

    llt::FeedHandler handler(
        logger,
        queue,
        source,
        recovery
    );

    handler.start(3);

    REQUIRE(
        recovery.called
    );

    REQUIRE(
        recovery.expected == 102
    );

    REQUIRE(
        recovery.received == 103
    );

    REQUIRE(
        queue.size() == 4
    );

    const auto event100 = queue.pop();
    const auto event101 = queue.pop();
    const auto event102 = queue.pop();
    const auto event103 = queue.pop();

    REQUIRE(event100.has_value());
    REQUIRE(event101.has_value());
    REQUIRE(event102.has_value());
    REQUIRE(event103.has_value());

    REQUIRE(
        std::visit(
            [](const auto& event)
            {
                return event.sequence()
                    == llt::SequenceNumber(100);
            },
            *event100
        )
    );

    REQUIRE(
        std::visit(
            [](const auto& event)
            {
                return event.sequence()
                    == llt::SequenceNumber(101);
            },
            *event101
        )
    );

    REQUIRE(
        std::visit(
            [](const auto& event)
            {
                return event.sequence()
                    == llt::SequenceNumber(102);
            },
            *event102
        )
    );

    REQUIRE(
        std::visit(
            [](const auto& event)
            {
                return event.sequence()
                    == llt::SequenceNumber(103);
            },
            *event103
        )
    );

    REQUIRE(
        queue.empty()
    );
}

TEST_CASE(
    "SequenceRecovery logs sequence gap"
)
{
    llt::ConsoleLogger logger;

    MockMarketDataRecoverySource source;

    llt::SequenceRecovery recovery(
        logger,
        source
    );

    std::vector<llt::MarketDataMessage> recoveredMessages;

    const llt::SequenceCheckResult recovered =
    recovery.recover(
        102,
        103,
        recoveredMessages
    );

    REQUIRE(
        recovered == llt::SequenceCheckResult::Process
    );

    REQUIRE(
        source.called
    );

    REQUIRE(
        source.from == 102
    );

    REQUIRE(
        source.to == 103
    );

    REQUIRE(
        recoveredMessages.size() == 1
    );

    REQUIRE(
        recoveredMessages[0].sequence == 102
    );

    REQUIRE(
        logger.warningCount == 1
    );

    REQUIRE(
        logger.lastWarning
        == "Sequence recovery required: expected=102 received=103"
    );
}

TEST_CASE(
    "FeedHandler stops when sequence recovery fails"
)
{
    llt::ConsoleLogger logger;

    llt::MarketEventQueue queue;

    SequenceGapMarketDataSource source;

    MockMarketDataRecoverySource recoverySource;

    recoverySource.shouldRecover =
        false;

    llt::SequenceRecovery recovery(
        logger,
        recoverySource
    );

    llt::FeedHandler handler(
        logger,
        queue,
        source,
        recovery
    );

    handler.start(3);

    REQUIRE(
        queue.size() == 2
    );

    const auto event100 =
        queue.pop();

    const auto event101 =
        queue.pop();

    REQUIRE(
        event100.has_value()
    );

    REQUIRE(
        event101.has_value()
    );

    REQUIRE(
        std::visit(
            [](const auto& event)
            {
                return event.sequence()
                    == llt::SequenceNumber(100);
            },
            *event100
        )
    );

    REQUIRE(
        std::visit(
            [](const auto& event)
            {
                return event.sequence()
                    == llt::SequenceNumber(101);
            },
            *event101
        )
    );

    REQUIRE(
        queue.empty()
    );

    REQUIRE(
        recoverySource.called
    );

    REQUIRE(
        recoverySource.from == 102
    );

    REQUIRE(
        recoverySource.to == 103
    );

    REQUIRE(
        logger.warningCount == 1
    );
}

TEST_CASE(
    "SequenceRecovery validates recovered sequence range"
)
{
    llt::ConsoleLogger logger;

    MockMarketDataRecoverySource source;

    llt::SequenceRecovery recovery(
        logger,
        source
    );

    std::vector<llt::MarketDataMessage> recoveredMessages;

    const llt::SequenceCheckResult recovered =
    recovery.recover(
        102,
        105,
        recoveredMessages
    );

    REQUIRE(
        recovered == llt::SequenceCheckResult::Process
    );

    REQUIRE(recoveredMessages.size() == 3);

    REQUIRE(recoveredMessages[0].sequence == 102);
    REQUIRE(recoveredMessages[1].sequence == 103);
    REQUIRE(recoveredMessages[2].sequence == 104);
}

TEST_CASE(
    "SequenceRecovery rejects incomplete recovered sequence"
)
{
    llt::ConsoleLogger logger;

    MockMarketDataRecoverySource source;

    source.skipSequence = 103;

    llt::SequenceRecovery recovery(
        logger,
        source
    );

    std::vector<llt::MarketDataMessage> recoveredMessages;

    const llt::SequenceCheckResult recovered =
    recovery.recover(
        102,
        105,
        recoveredMessages
    );

    REQUIRE(
        recovered == llt::SequenceCheckResult::Stop
    );
}

TEST_CASE(
    "SequenceRecovery rejects out-of-order recovered sequence"
)
{
    llt::ConsoleLogger logger;

    MockMarketDataRecoverySource source;

    source.outOfOrder = true;

    llt::SequenceRecovery recovery(
        logger,
        source
    );

    std::vector<llt::MarketDataMessage> recoveredMessages;

    const llt::SequenceCheckResult recovered =
    recovery.recover(
        102,
        105,
        recoveredMessages
    );

    REQUIRE(
        recovered == llt::SequenceCheckResult::Stop
    );
}

TEST_CASE(
    "SequenceRecovery rejects backward sequence"
)
{
    llt::ConsoleLogger logger;

    MockMarketDataRecoverySource source;

    llt::SequenceRecovery recovery(
        logger,
        source
    );

    std::vector<llt::MarketDataMessage> recoveredMessages;

    const llt::SequenceCheckResult recovered =
    recovery.recover(
        105,
        103,
        recoveredMessages
    );

    REQUIRE(
        recovered == llt::SequenceCheckResult::Ignore
    );
}

TEST_CASE(
    "SequenceRecovery rejects zero received sequence"
)
{
    llt::ConsoleLogger logger;

    MockMarketDataRecoverySource source;

    llt::SequenceRecovery recovery(
        logger,
        source
    );

    std::vector<llt::MarketDataMessage> recoveredMessages;

    const llt::SequenceCheckResult recovered =
    recovery.recover(
        10,
        0,
        recoveredMessages
    );

    REQUIRE(
        recovered == llt::SequenceCheckResult::Ignore
    );

    REQUIRE(
        recoveredMessages.empty()
    );

    REQUIRE(
        source.called == false
    );
}

TEST_CASE(
    "SequenceRecovery handles maximum sequence"
)
{
    llt::ConsoleLogger logger;

    MockMarketDataRecoverySource source;

    llt::SequenceRecovery recovery(
        logger,
        source
    );

    std::vector<llt::MarketDataMessage> recoveredMessages;

    const std::uint64_t maxSequence =
        std::numeric_limits<std::uint64_t>::max();

    const llt::SequenceCheckResult recovered =
    recovery.recover(
        maxSequence - 2,
        maxSequence,
        recoveredMessages
    );
    
    REQUIRE(
        recovered == llt::SequenceCheckResult::Process
    );

    REQUIRE(
        source.called == true
    );

    REQUIRE(
        source.from == maxSequence - 2
    );

    REQUIRE(
        source.to == maxSequence
    );

    REQUIRE(
        recoveredMessages.size() == 2
    );

    REQUIRE(
        recoveredMessages[0].sequence == maxSequence - 2
    );

    REQUIRE(
        recoveredMessages[1].sequence == maxSequence - 1
    );
}




TEST_CASE(
    "FeedHandler stops cleanly while market event queue is full"
)
{
    llt::ConsoleLogger logger;

    llt::MarketEventQueue queue;

    // SpscRingBuffer<..., 16384> reserves one slot
    // to distinguish full from empty.
    //
    // Therefore its usable capacity is 16383.
    constexpr std::size_t usableCapacity =
        65535;

    // ---------------------------------------------------------
    // Completely fill the SPSC queue.
    // ---------------------------------------------------------

    for (
        std::size_t i = 0;
        i < usableCapacity;
        ++i
    )
    {
        llt::MarketEvent event =
            llt::Trade(
                llt::Instrument("TXFU6"),

                llt::SequenceNumber(
                    static_cast<std::uint64_t>(
                        i + 1
                    )
                ),

                llt::Timestamp(
                    static_cast<std::uint64_t>(
                        i + 1
                    )
                ),

                llt::Price(100),

                llt::Quantity(1),

                llt::Side::Buy
            );

        REQUIRE(
            queue.push(
                std::move(event)
            )
        );
    }

    REQUIRE(
        queue.size()
        == usableCapacity
    );

    REQUIRE(
        queue.full()
    );

    // ---------------------------------------------------------
    // FeedHandler will receive another market-data message.
    //
    // Since the SPSC queue is already full, processMessage()
    // will retry publication until either:
    //
    // 1. space becomes available, or
    // 2. stop() sets running_ = false.
    // ---------------------------------------------------------

    llt::MockMarketDataSource source;

    MockMarketDataRecoverySource
        recoverySource;

    llt::SequenceRecovery recovery(
        logger,
        recoverySource
    );

    llt::FeedHandler handler(
        logger,
        queue,
        source,
        recovery
    );

    std::thread feedThread(
        [&handler]
        {
            handler.run();
        }
    );

    // Wait until run() has definitely initialized its
    // lifecycle state.
    while (
        handler.state()
        != llt::FeedHandlerState::Running
    )
    {
        std::this_thread::yield();
    }

    // Give FeedHandler enough time to receive its first
    // message and hit the full queue.
    std::this_thread::sleep_for(
        std::chrono::milliseconds(20)
    );

    handler.stop();

    feedThread.join();

    // ---------------------------------------------------------
    // Critical behavior:
    //
    // processMessage() must observe running_ == false and
    // escape instead of spinning forever on the full queue.
    // ---------------------------------------------------------

    REQUIRE(
        handler.state()
        == llt::FeedHandlerState::Stopped
    );

    // No consumer removed anything and FeedHandler could not
    // publish its additional event.
    REQUIRE(
        queue.size()
        == usableCapacity
    );

    REQUIRE(
        queue.full()
    );
}