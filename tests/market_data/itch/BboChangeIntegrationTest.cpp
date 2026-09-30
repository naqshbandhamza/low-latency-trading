#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <vector>

#include "market_data/itch/ItchMarketState.h"
#include "market_data/itch/ItchMessage.h"

#include "market_data/itch/messages/AddOrderMessage.h"
#include "market_data/itch/messages/OrderDeleteMessage.h"

namespace
{

    struct BboNotification
    {
        llt::market_data::InstrumentId instrumentId;
        llt::market_data::Bbo bbo;
    };

    llt::itch::AddOrderMessage makeAdd(
        std::uint64_t orderId,
        std::uint16_t instrumentId,
        char side,
        std::uint32_t quantity,
        std::uint32_t price)
    {
        llt::itch::AddOrderMessage message{};

        message.stockLocate =
            instrumentId;

        message.timestamp =
            1000;

        message.orderReferenceNumber =
            orderId;

        message.buySellIndicator =
            side;

        message.shares =
            quantity;

        message.price =
            price;

        return message;
    }

    llt::itch::OrderDeleteMessage makeDelete(
        std::uint64_t orderId,
        std::uint64_t timestamp = 2000)
    {
        llt::itch::OrderDeleteMessage message{};

        message.timestamp =
            timestamp;

        message.orderReferenceNumber =
            orderId;

        return message;
    }

} // namespace

TEST_CASE(
    "Adding first bid emits BBO change")
{
    std::vector<BboNotification> notifications;

    llt::itch::ItchMarketState state{
        [&notifications](
            llt::market_data::InstrumentId instrumentId,
            llt::market_data::Timestamp,
            const llt::market_data::Bbo &bbo)
        {
            notifications.push_back(
                BboNotification{
                    instrumentId,
                    bbo});
        }};

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(
                100,
                42,
                'B',
                500,
                1000000)});

    REQUIRE(
        notifications.size() ==
        1);

    const auto &notification =
        notifications[0];

    REQUIRE(
        notification.instrumentId ==
        42);

    REQUIRE(
        notification.bbo.hasBid);

    REQUIRE(
        notification.bbo.bidPrice ==
        1000000);

    REQUIRE(
        notification.bbo.bidQuantity ==
        500);

    REQUIRE_FALSE(
        notification.bbo.hasAsk);
}

TEST_CASE(
    "Adding order behind best bid does not emit BBO change")
{
    std::vector<BboNotification> notifications;

    llt::itch::ItchMarketState state{
        [&notifications](
            llt::market_data::InstrumentId instrumentId,
            llt::market_data::Timestamp,
            const llt::market_data::Bbo &bbo)
        {
            notifications.push_back(
                BboNotification{
                    instrumentId,
                    bbo});
        }};

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(
                100,
                42,
                'B',
                500,
                1000000)});

    REQUIRE(
        notifications.size() ==
        1);

    // Worse bid.
    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(
                200,
                42,
                'B',
                300,
                999900)});

    // Book changed internally, but BBO did not.
    REQUIRE(
        notifications.size() ==
        1);

    const auto *book =
        state.books().find(42);

    REQUIRE(
        book != nullptr);

    REQUIRE(
        book->bids().levelCount() ==
        2);
}

TEST_CASE(
    "Adding quantity at current best bid emits BBO change")
{
    std::vector<BboNotification> notifications;

    llt::itch::ItchMarketState state{
        [&notifications](
            llt::market_data::InstrumentId instrumentId,
            llt::market_data::Timestamp,
            const llt::market_data::Bbo &bbo)
        {
            notifications.push_back(
                BboNotification{
                    instrumentId,
                    bbo});
        }};

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(
                100,
                42,
                'B',
                500,
                1000000)});

    REQUIRE(
        notifications.size() ==
        1);

    // Same best price, additional quantity.
    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(
                200,
                42,
                'B',
                300,
                1000000)});

    REQUIRE(
        notifications.size() ==
        2);

    const auto &latest =
        notifications.back();

    REQUIRE(
        latest.instrumentId ==
        42);

    REQUIRE(
        latest.bbo.hasBid);

    REQUIRE(
        latest.bbo.bidPrice ==
        1000000);

    REQUIRE(
        latest.bbo.bidQuantity ==
        800);
}

TEST_CASE(
    "Adding better bid emits BBO change")
{
    std::vector<BboNotification> notifications;

    llt::itch::ItchMarketState state{
        [&notifications](
            llt::market_data::InstrumentId instrumentId,
            llt::market_data::Timestamp,
            const llt::market_data::Bbo &bbo)
        {
            notifications.push_back(
                BboNotification{
                    instrumentId,
                    bbo});
        }};

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(
                100,
                42,
                'B',
                500,
                1000000)});

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(
                200,
                42,
                'B',
                300,
                1000100)});

    REQUIRE(
        notifications.size() ==
        2);

    const auto &latest =
        notifications.back();

    REQUIRE(
        latest.instrumentId ==
        42);

    REQUIRE(
        latest.bbo.hasBid);

    REQUIRE(
        latest.bbo.bidPrice ==
        1000100);

    REQUIRE(
        latest.bbo.bidQuantity ==
        300);
}

TEST_CASE(
    "Deleting best bid emits next best BBO")
{
    std::vector<BboNotification> notifications;

    llt::itch::ItchMarketState state{
        [&notifications](
            llt::market_data::InstrumentId instrumentId,
            llt::market_data::Timestamp,
            const llt::market_data::Bbo &bbo)
        {
            notifications.push_back(
                BboNotification{
                    instrumentId,
                    bbo});
        }};

    // Best.
    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(
                100,
                42,
                'B',
                500,
                1000000)});

    // Worse level. No BBO notification.
    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(
                200,
                42,
                'B',
                300,
                999900)});

    REQUIRE(
        notifications.size() ==
        1);

    // Delete current best.
    state.onMessage(
        llt::itch::ItchMessage{
            makeDelete(100)});

    REQUIRE(
        notifications.size() ==
        2);

    const auto &latest =
        notifications.back();

    REQUIRE(
        latest.instrumentId ==
        42);

    REQUIRE(
        latest.bbo.hasBid);

    REQUIRE(
        latest.bbo.bidPrice ==
        999900);

    REQUIRE(
        latest.bbo.bidQuantity ==
        300);
}

TEST_CASE(
    "BBO notifications remain independent between instruments")
{
    std::vector<BboNotification> notifications;

    llt::itch::ItchMarketState state{
        [&notifications](
            llt::market_data::InstrumentId instrumentId,
            llt::market_data::Timestamp,
            const llt::market_data::Bbo &bbo)
        {
            notifications.push_back(
                BboNotification{
                    instrumentId,
                    bbo});
        }};

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(
                100,
                42,
                'B',
                500,
                1000000)});

    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(
                200,
                77,
                'B',
                300,
                2000000)});

    REQUIRE(
        notifications.size() ==
        2);

    REQUIRE(
        notifications[0].instrumentId ==
        42);

    REQUIRE(
        notifications[0].bbo.bidPrice ==
        1000000);

    REQUIRE(
        notifications[0].bbo.bidQuantity ==
        500);

    REQUIRE(
        notifications[1].instrumentId ==
        77);

    REQUIRE(
        notifications[1].bbo.bidPrice ==
        2000000);

    REQUIRE(
        notifications[1].bbo.bidQuantity ==
        300);

    // Change instrument 42 again.
    state.onMessage(
        llt::itch::ItchMessage{
            makeAdd(
                300,
                42,
                'B',
                100,
                1000100)});

    REQUIRE(
        notifications.size() ==
        3);

    REQUIRE(
        notifications.back().instrumentId ==
        42);

    REQUIRE(
        notifications.back().bbo.bidPrice ==
        1000100);

    // Instrument 77 remains unchanged.
    const auto *book77 =
        state.books().find(77);

    REQUIRE(
        book77 != nullptr);

    const auto bbo77 =
        book77->bbo();

    REQUIRE(
        bbo77.bidPrice ==
        2000000);

    REQUIRE(
        bbo77.bidQuantity ==
        300);
}