#include <thread>
#include <vector>
#include <catch2/catch_test_macros.hpp>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "FeedHandler.h"
#include "market_data/IMarketDataRecoverySource.h"
#include "market_data/UdpMarketDataCodec.h"
#include "market_data/UdpMarketDataSource.h"
#include "market_data/MarketDataMessage.h"
#include "market_data/UdpMarketDataPacket.h"
#include "logging/ConsoleLogger.h"
#include "ring_buffer/SpscRingBuffer.h"
#include "market_data/ISequenceRecovery.h"
#include "market_data/SequenceRecovery.h"

namespace
{

class IntegrationRecoverySource
    : public llt::IMarketDataRecoverySource
{
public:
    bool shouldRecover = true;

    bool called = false;

    std::uint64_t receivedFrom = 0;
    std::uint64_t receivedTo = 0;

    bool recover(
        std::uint64_t fromSequence,
        std::uint64_t toSequence,
        std::vector<llt::MarketDataMessage> &messages) override
    {
        called = true;

        receivedFrom = fromSequence;
        receivedTo = toSequence;

        messages.clear();

        if (!shouldRecover)
        {
            return false;
        }

        for (
            std::uint64_t sequence = fromSequence;
            sequence < toSequence;
            ++sequence)
        {
            llt::MarketDataMessage message{};

            message.type =
                llt::MarketDataMessageType::Trade;

            message.sequence =
                sequence;

            message.timestamp =
                sequence;

            message.price =
                static_cast<std::int64_t>(
                    100 + sequence);

            message.quantity =
                10;

            message.side =
                llt::Side::Buy;

            messages.push_back(message);
        }

        return true;
    }
};

class UdpTestSender
{
public:
    explicit UdpTestSender(
        std::uint16_t port)
    {
        sender_ =
            ::socket(
                AF_INET,
                SOCK_DGRAM,
                0);

        REQUIRE(sender_ >= 0);

        address_.sin_family =
            AF_INET;

        address_.sin_addr.s_addr =
            htonl(INADDR_LOOPBACK);

        address_.sin_port =
            htons(port);
    }

    ~UdpTestSender()
    {
        if (sender_ >= 0)
        {
            ::close(sender_);
        }
    }

    void sendTrade(
        std::uint64_t sequence)
    {
        llt::UdpMarketDataPacket packet{};

        packet.type =
            llt::UdpMarketDataPacketType::Trade;

        packet.sequence =
            sequence;

        packet.timestamp =
            sequence;

        packet.price =
            static_cast<std::int64_t>(
                100 + sequence);

        packet.quantity =
            10;

        packet.side =
            llt::UdpMarketDataPacketSide::Buy;

        const auto buffer =
            llt::UdpMarketDataCodec::encode(
                packet);

        const auto result =
            ::sendto(
                sender_,
                buffer.data(),
                buffer.size(),
                0,
                reinterpret_cast<const sockaddr *>(
                    &address_),
                sizeof(address_));

        REQUIRE(
            result == static_cast<ssize_t>(
                          buffer.size()));
    }

private:
    int sender_ = -1;

    sockaddr_in address_{};
};

void requireTradeSequence(
    llt::MarketEventQueue &queue,
    std::uint64_t sequence)
{
    auto event = queue.pop();

    REQUIRE(event.has_value());

    REQUIRE(
        std::get<llt::Trade>(*event).sequence()
        == llt::SequenceNumber{sequence});
}

}

TEST_CASE(
    "UDP FeedHandler recovers missing packet sequence")
{
    constexpr std::uint16_t port =
        19008;

    constexpr std::uint32_t timeoutMs =
        100;

    llt::UdpMarketDataSource source(
        port,
        timeoutMs);

    llt::ConsoleLogger logger;

    IntegrationRecoverySource recoverySource1;
    llt::SequenceRecovery recoverySource(logger,recoverySource1);


    llt::MarketEventQueue queue;

    llt::FeedHandler handler(
        logger,
        queue,
        source,
        recoverySource);

    UdpTestSender sender(port);

    std::thread feedThread(
        [&]()
        {
            handler.start(3);
        });

    sender.sendTrade(1);
    sender.sendTrade(2);

    // Sequence 3 is intentionally missing.

    sender.sendTrade(4);

    feedThread.join();

    REQUIRE(
        queue.size() == 4);

    requireTradeSequence(queue, 1);
    requireTradeSequence(queue, 2);
    requireTradeSequence(queue, 3);
    requireTradeSequence(queue, 4);
}


TEST_CASE(
    "UDP FeedHandler handles normal contiguous sequence")
{
    constexpr std::uint16_t port =
        19009;

    constexpr std::uint32_t timeoutMs =
        100;

    llt::UdpMarketDataSource source(
        port,
        timeoutMs);


    llt::ConsoleLogger logger;

    IntegrationRecoverySource recoverySource1;
    llt::SequenceRecovery recoverySource(logger,recoverySource1);

    llt::MarketEventQueue queue;

    llt::FeedHandler handler(
        logger,
        queue,
        source,
        recoverySource);

    UdpTestSender sender(port);

    std::thread feedThread(
        [&]()
        {
            handler.start(4);
        });

    sender.sendTrade(1);
    sender.sendTrade(2);
    sender.sendTrade(3);
    sender.sendTrade(4);

    feedThread.join();

    REQUIRE_FALSE(
        recoverySource1.called);

    REQUIRE(
        queue.size() == 4);

    requireTradeSequence(queue, 1);
    requireTradeSequence(queue, 2);
    requireTradeSequence(queue, 3);
    requireTradeSequence(queue, 4);
}


TEST_CASE(
    "UDP FeedHandler recovers multiple missing packets")
{
    constexpr std::uint16_t port =
        19010;

    constexpr std::uint32_t timeoutMs =
        100;

    llt::UdpMarketDataSource source(
        port,
        timeoutMs);

    

    llt::ConsoleLogger logger;

    IntegrationRecoverySource recoverySource1;
    llt::SequenceRecovery recoverySource(logger,recoverySource1);

    llt::MarketEventQueue queue;

    llt::FeedHandler handler(
        logger,
        queue,
        source,
        recoverySource);

    UdpTestSender sender(port);

    std::thread feedThread(
        [&]()
        {
            handler.start(3);
        });

    sender.sendTrade(1);
    sender.sendTrade(2);

    // Sequences 3 and 4 are intentionally missing.

    sender.sendTrade(5);

    feedThread.join();

    REQUIRE(
        recoverySource1.called);

    REQUIRE(
        recoverySource1.receivedFrom == 3);

    REQUIRE(
        recoverySource1.receivedTo == 5);

    REQUIRE(
        queue.size() == 5);

    requireTradeSequence(queue, 1);
    requireTradeSequence(queue, 2);
    requireTradeSequence(queue, 3);
    requireTradeSequence(queue, 4);
    requireTradeSequence(queue, 5);
}


TEST_CASE(
    "UDP FeedHandler ignores duplicate old packet")
{
    constexpr std::uint16_t port =
        19011;

    constexpr std::uint32_t timeoutMs =
        100;

    llt::UdpMarketDataSource source(
        port,
        timeoutMs);

    

    llt::ConsoleLogger logger;

    IntegrationRecoverySource recoverySource1;
    llt::SequenceRecovery recoverySource(logger,recoverySource1);

    llt::MarketEventQueue queue;

    llt::FeedHandler handler(
        logger,
        queue,
        source,
        recoverySource);

    UdpTestSender sender(port);

    std::thread feedThread(
        [&]()
        {
            handler.start(3);
        });

    sender.sendTrade(1);
    sender.sendTrade(2);

    // Duplicate sequence 2.

    sender.sendTrade(2);

    sender.sendTrade(3);

    feedThread.join();

    REQUIRE_FALSE(
        recoverySource1.called);

    REQUIRE(
        queue.size() == 3);

    requireTradeSequence(queue, 1);
    requireTradeSequence(queue, 2);
    requireTradeSequence(queue, 3);
}


TEST_CASE(
    "UDP FeedHandler stops safely when recovery fails")
{
    constexpr std::uint16_t port =
        19012;

    constexpr std::uint32_t timeoutMs =
        100;

    llt::UdpMarketDataSource source(
        port,
        timeoutMs);

    


    llt::ConsoleLogger logger;

    IntegrationRecoverySource recoverySource1;
    recoverySource1.shouldRecover =
        false;
    llt::SequenceRecovery recoverySource(logger,recoverySource1);

    llt::MarketEventQueue queue;

    llt::FeedHandler handler(
        logger,
        queue,
        source,
        recoverySource);

    UdpTestSender sender(port);

    std::thread feedThread(
        [&]()
        {
            handler.start(3);
        });

    sender.sendTrade(1);
    sender.sendTrade(2);

    // Sequence 3 is missing.

    sender.sendTrade(4);

    feedThread.join();

    REQUIRE(
        recoverySource1.called);

    REQUIRE(
        recoverySource1.receivedFrom == 3);

    REQUIRE(
        recoverySource1.receivedTo == 4);

    // Only the valid packets received before
    // the failed recovery should be queued.

    REQUIRE(
        queue.size() == 2);

    requireTradeSequence(queue, 1);
    requireTradeSequence(queue, 2);
}


TEST_CASE(
    "UDP FeedHandler establishes sequence from non-zero first packet")
{
    constexpr std::uint16_t port =
        19013;

    constexpr std::uint32_t timeoutMs =
        100;

    llt::UdpMarketDataSource source(
        port,
        timeoutMs);

   

    llt::ConsoleLogger logger;

    IntegrationRecoverySource recoverySource1;
    llt::SequenceRecovery recoverySource(logger,recoverySource1);

    llt::MarketEventQueue queue;

    llt::FeedHandler handler(
        logger,
        queue,
        source,
        recoverySource);

    UdpTestSender sender(port);

    std::thread feedThread(
        [&]()
        {
            handler.start(2);
        });

    sender.sendTrade(100);
    sender.sendTrade(101);

    feedThread.join();

    REQUIRE_FALSE(
        recoverySource1.called);

    REQUIRE(
        queue.size() == 2);

    requireTradeSequence(queue, 100);
    requireTradeSequence(queue, 101);
}
