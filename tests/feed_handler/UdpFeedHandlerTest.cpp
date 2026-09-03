#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdint>
#include <string_view>
#include <thread>

#include <catch2/catch_test_macros.hpp>

#include "FeedHandler.h"
#include "logging/ILogger.h"
#include "market_data/UdpMarketDataPacket.h"
#include "market_data/UdpMarketDataSource.h"
#include "market_data/SequenceRecovery.h"
#include "MockMarketDataRecoverySource.h"
#include "market_data/MarketDataMessage.h"
#include "ring_buffer/SpscRingBuffer.h"
#include "market_data/MarketEvent.h"

namespace
{

class TestLogger : public llt::ILogger
{
public:

    void log(
        llt::LogLevel,
        std::string_view
    ) override
    {
    }
};

int createSender()
{
    return ::socket(
        AF_INET,
        SOCK_DGRAM,
        0
    );
}

void sendPacket(
    int socket,
    std::uint16_t port,
    const llt::UdpMarketDataPacket& packet
)
{
    sockaddr_in address{};

    address.sin_family =
        AF_INET;

    address.sin_addr.s_addr =
        htonl(INADDR_LOOPBACK);

    address.sin_port =
        htons(port);

    const auto result =
        ::sendto(
            socket,
            &packet,
            sizeof(packet),
            0,
            reinterpret_cast<
                const sockaddr*
            >(&address),
            sizeof(address)
        );

    REQUIRE(
        result
        == static_cast<ssize_t>(
            sizeof(packet)
        )
    );
}

} // namespace

TEST_CASE(
    "UDP market data flows through FeedHandler into SPSC queue"
)
{
    constexpr std::uint16_t port =
        19003;

    TestLogger logger;

    llt::MarketEventQueue queue;

    llt::UdpMarketDataSource source(
        port
    );

    const int sender =
        createSender();

    REQUIRE(sender >= 0);

    MockMarketDataRecoverySource recoverySource;

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

    std::thread feedHandlerThread(
        [&handler]
        {
            handler.start(2);
        }
    );

    llt::UdpMarketDataPacket quote{};

    quote.type =
        llt::UdpMarketDataPacketType::Quote;

    quote.sequence =
        100;

    quote.timestamp =
        1000;

    quote.bidPrice =
        234500;

    quote.bidQuantity =
        10;

    quote.askPrice =
        234510;

    quote.askQuantity =
        12;

    quote.side =
        llt::UdpMarketDataPacketSide::Buy;

    sendPacket(
        sender,
        port,
        quote
    );

    llt::UdpMarketDataPacket trade{};

    trade.type =
        llt::UdpMarketDataPacketType::Trade;

    trade.sequence =
        101;

    trade.timestamp =
        1001;

    trade.price =
        234505;

    trade.quantity =
        3;

    trade.side =
        llt::UdpMarketDataPacketSide::Buy;

    sendPacket(
        sender,
        port,
        trade
    );

    feedHandlerThread.join();

    ::close(sender);

    REQUIRE(
        queue.size() == 2
    );

    auto quoteEvent =
        queue.pop();

    REQUIRE(
        quoteEvent.has_value()
    );

    REQUIRE(
        std::holds_alternative<llt::Quote>(
            *quoteEvent
        )
    );

    const auto& receivedQuote =
        std::get<llt::Quote>(
            *quoteEvent
        );

    REQUIRE(
        receivedQuote.sequence()
        == llt::SequenceNumber(100)
    );

    REQUIRE(
        receivedQuote.bid().price()
        == llt::Price(234500)
    );

    REQUIRE(
        receivedQuote.bid().quantity()
        == llt::Quantity(10)
    );

    REQUIRE(
        receivedQuote.ask().price()
        == llt::Price(234510)
    );

    REQUIRE(
        receivedQuote.ask().quantity()
        == llt::Quantity(12)
    );

    auto tradeEvent =
        queue.pop();

    REQUIRE(
        tradeEvent.has_value()
    );

    REQUIRE(
        std::holds_alternative<llt::Trade>(
            *tradeEvent
        )
    );

    const auto& receivedTrade =
        std::get<llt::Trade>(
            *tradeEvent
        );

    REQUIRE(
        receivedTrade.sequence()
        == llt::SequenceNumber(101)
    );

    REQUIRE(
        receivedTrade.price()
        == llt::Price(234505)
    );

    REQUIRE(
        receivedTrade.quantity()
        == llt::Quantity(3)
    );

    REQUIRE(
        receivedTrade.side()
        == llt::Side::Buy
    );

    REQUIRE(
        queue.empty()
    );
}