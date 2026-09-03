#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "FeedHandler.h"
#include "logging/ILogger.h"
#include "market_data/MarketEvent.h"
#include "market_data/MockMarketDataSource.h"
#include "market_data/SequenceRecovery.h"
#include "MockSequenceRecovery.h"
#include "MockMarketDataRecoverySource.h"
#include "market_data/MarketDataMessage.h"
#include "ring_buffer/SpscRingBuffer.h"

namespace
{

class TestLogger : public llt::ILogger
{
public:

    void log(
        llt::LogLevel level,
        std::string_view message
    ) override
    {
        if (level == llt::LogLevel::Warning)
        {
            ++warningCount;
            lastWarning = std::string(message);
        }
    }

    std::size_t warningCount{0};

    std::string lastWarning;
};

} // namespace


// class TestSequenceRecovery
//     : public llt::ISequenceRecovery
// {
// public:

//     bool recover(
//         std::uint64_t expectedSequence,
//         std::uint64_t receivedSequence,
//         std::vector<llt::MarketDataMessage>& recoveredMessages
//     ) override
//     {
//         called = true;

//         expected = expectedSequence;
//         received = receivedSequence;

//         recoveredMessages.clear();

//         for (
//             std::uint64_t sequence = expectedSequence;
//             sequence < receivedSequence;
//             ++sequence
//         )
//         {
//             llt::MarketDataMessage message;

//             message.type =
//                 llt::MarketDataMessageType::Quote;

//             message.sequence =
//                 sequence;

//             message.timestamp =
//                 sequence;

//             message.bidPrice =
//                 234500;

//             message.bidQuantity =
//                 10;

//             message.askPrice =
//                 234510;

//             message.askQuantity =
//                 12;

//             recoveredMessages.push_back(
//                 message
//             );
//         }

//         return true;
//     }

//     bool called{false};

//     std::uint64_t expected{0};

//     std::uint64_t received{0};
// };


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

    TestLogger logger;

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
    TestLogger logger;

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
    TestLogger logger;

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
    TestLogger logger;

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
    TestLogger logger;

    MockMarketDataRecoverySource source;

    llt::SequenceRecovery recovery(
        logger,
        source
    );

    std::vector<llt::MarketDataMessage> recoveredMessages;

    const bool recovered =
        recovery.recover(
            102,
            103,
            recoveredMessages
        );

    REQUIRE(
        recovered == true
    );

    REQUIRE(
        source.called
    );

    REQUIRE(
        source.from == 102
    );

    REQUIRE(
        source.to == 102
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
    TestLogger logger;

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
        recoverySource.to == 102
    );

    REQUIRE(
        logger.warningCount == 1
    );
}




TEST_CASE(
    "SequenceRecovery validates recovered sequence range"
)
{
    TestLogger logger;

    MockMarketDataRecoverySource source;

    llt::SequenceRecovery recovery(
        logger,
        source
    );

    std::vector<llt::MarketDataMessage> recoveredMessages;

    const bool recovered =
        recovery.recover(
            102,
            105,
            recoveredMessages
        );

    REQUIRE(recovered);

    REQUIRE(recoveredMessages.size() == 3);

    REQUIRE(recoveredMessages[0].sequence == 102);
    REQUIRE(recoveredMessages[1].sequence == 103);
    REQUIRE(recoveredMessages[2].sequence == 104);
}


TEST_CASE(
    "SequenceRecovery rejects incomplete recovered sequence"
)
{
    TestLogger logger;

    MockMarketDataRecoverySource source;

    source.skipSequence = 103;

    llt::SequenceRecovery recovery(
        logger,
        source
    );

    std::vector<llt::MarketDataMessage> recoveredMessages;

    const bool recovered =
        recovery.recover(
            102,
            105,
            recoveredMessages
        );

    REQUIRE(
        recovered == false
    );
}



TEST_CASE(
    "SequenceRecovery rejects out-of-order recovered sequence"
)
{
    TestLogger logger;

    MockMarketDataRecoverySource source;

    source.outOfOrder = true;

    llt::SequenceRecovery recovery(
        logger,
        source
    );

    std::vector<llt::MarketDataMessage> recoveredMessages;

    const bool recovered =
        recovery.recover(
            102,
            105,
            recoveredMessages
        );

    REQUIRE(
        recovered == false
    );
}

TEST_CASE(
    "SequenceRecovery rejects backward sequence"
)
{
    TestLogger logger;

    MockMarketDataRecoverySource source;

    llt::SequenceRecovery recovery(
        logger,
        source
    );

    std::vector<llt::MarketDataMessage> recoveredMessages;

    const bool recovered =
        recovery.recover(
            105,
            103,
            recoveredMessages
        );

    REQUIRE(
        recovered == false
    );
}




TEST_CASE(
    "SequenceRecovery rejects zero received sequence"
)
{
    TestLogger logger;

    MockMarketDataRecoverySource source;

    llt::SequenceRecovery recovery(
        logger,
        source
    );

    std::vector<llt::MarketDataMessage> recoveredMessages;

    const bool recovered =
        recovery.recover(
            10,
            0,
            recoveredMessages
        );

    REQUIRE(
        recovered == false
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
    TestLogger logger;

    MockMarketDataRecoverySource source;

    llt::SequenceRecovery recovery(
        logger,
        source
    );

    std::vector<llt::MarketDataMessage> recoveredMessages;

    const std::uint64_t maxSequence =
        std::numeric_limits<std::uint64_t>::max();

    const bool recovered =
        recovery.recover(
            maxSequence - 2,
            maxSequence,
            recoveredMessages
        );

    REQUIRE(
        recovered == true
    );

    REQUIRE(
        source.called == true
    );

    REQUIRE(
        source.from == maxSequence - 2
    );

    REQUIRE(
        source.to == maxSequence - 1
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




