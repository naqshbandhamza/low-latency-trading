#include <atomic>
#include <thread>
#include <vector>
#include <catch2/catch_test_macros.hpp>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "FeedHandler.h"
#include "market_data/UdpMarketDataCodec.h"
#include "market_data/UdpMarketDataSource.h"
#include "market_data/MarketDataMessage.h"
#include "market_data/UdpMarketDataPacket.h"
#include "logging/ConsoleLogger.h"
#include "ring_buffer/SpscRingBuffer.h"
#include "market_data/ISequenceRecovery.h"

namespace
{

    // class IntegrationRecoverySource
    //     : public llt::ISequenceRecovery
    // {
    // public:
    //     bool recover(
    //         std::uint64_t fromSequence,
    //         std::uint64_t toSequence,
    //         std::vector<llt::MarketDataMessage> &messages) override
    //     {
    //         messages.clear();

    //         for (
    //             std::uint64_t sequence = fromSequence;
    //             sequence <= toSequence;
    //             ++sequence)
    //         {
    //             llt::MarketDataMessage message{};

    //             message.type =
    //                 llt::MarketDataMessageType::Trade;

    //             message.sequence =
    //                 sequence;

    //             message.timestamp =
    //                 sequence;

    //             message.price =
    //                 static_cast<std::int64_t>(
    //                     100 + sequence);

    //             message.quantity =
    //                 10;

    //             message.side =
    //                 llt::Side::Buy;

    //             messages.push_back(message);

    //             if (sequence == toSequence)
    //             {
    //                 break;
    //             }
    //         }

    //         return true;
    //     }
    // };

    class IntegrationRecoverySource
        : public llt::ISequenceRecovery
    {
    public:
        bool recover(
            std::uint64_t fromSequence,
            std::uint64_t toSequence,
            std::vector<llt::MarketDataMessage> &messages) override
        {
            messages.clear();

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

    IntegrationRecoverySource recoverySource;

    llt::ConsoleLogger logger;

    llt::MarketEventQueue queue;

    llt::FeedHandler handler(
        logger,
        queue,
        source,
        recoverySource);

    const int sender =
        ::socket(AF_INET, SOCK_DGRAM, 0);

    REQUIRE(sender >= 0);

    sockaddr_in address{};

    address.sin_family =
        AF_INET;

    address.sin_addr.s_addr =
        htonl(INADDR_LOOPBACK);

    address.sin_port =
        htons(port);

    auto sendTrade =
        [&](std::uint64_t sequence)
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
                sender,
                buffer.data(),
                buffer.size(),
                0,
                reinterpret_cast<const sockaddr *>(
                    &address),
                sizeof(address));

        REQUIRE(
            result == static_cast<ssize_t>(
                          buffer.size()));
    };

    std::thread feedThread(
        [&]()
        {
            handler.start(3);
        });

    sendTrade(1);
    sendTrade(2);

    // Sequence 3 is intentionally missing.

    sendTrade(4);

    feedThread.join();

    ::close(sender);

    REQUIRE(
        queue.size() == 4);

    auto event1 = queue.pop();

    REQUIRE(event1.has_value());

    REQUIRE(
        std::get<llt::Trade>(*event1).sequence() == llt::SequenceNumber{1});

    auto event2 = queue.pop();

    REQUIRE(event2.has_value());

    REQUIRE(
        std::get<llt::Trade>(*event2).sequence() == llt::SequenceNumber{2});

    auto event3 = queue.pop();

    REQUIRE(event3.has_value());

    REQUIRE(
        std::get<llt::Trade>(*event3).sequence() == llt::SequenceNumber{3});

    auto event4 = queue.pop();

    REQUIRE(event4.has_value());

    REQUIRE(
        std::get<llt::Trade>(*event4).sequence() == llt::SequenceNumber{4});
}