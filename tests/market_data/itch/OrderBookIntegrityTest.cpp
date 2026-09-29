#include <catch2/catch_test_macros.hpp>

#include "market_data/itch/ItchMarketState.h"
#include "market_data/itch/ItchMessage.h"
#include "market_data/itch/messages/AddOrderMessage.h"
#include "market_data/itch/messages/OrderExecutedMessage.h"
#include "market_data/itch/messages/OrderCancelMessage.h"
#include "market_data/itch/messages/OrderDeleteMessage.h"

namespace
{

llt::itch::AddOrderMessage makeAdd()
{
    llt::itch::AddOrderMessage message{};

    message.stockLocate = 42;
    message.timestamp = 1000;
    message.orderReferenceNumber = 100;
    message.buySellIndicator = 'B';
    message.shares = 500;
    message.price = 1000000;

    return message;
}

}

TEST_CASE(
    "Valid order execution produces no book integrity errors"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{makeAdd()}
    );

    llt::itch::OrderExecutedMessage message{};
    message.orderReferenceNumber = 100;
    message.executedShares = 200;

    state.onMessage(
        llt::itch::ItchMessage{message}
    );

    REQUIRE(state.missingBooks() == 0);
    REQUIRE(state.failedBookReductions() == 0);
    REQUIRE(state.failedBookRemovals() == 0);
}

TEST_CASE(
    "Valid full execution produces no book integrity errors"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{makeAdd()}
    );

    llt::itch::OrderExecutedMessage message{};
    message.orderReferenceNumber = 100;
    message.executedShares = 500;

    state.onMessage(
        llt::itch::ItchMessage{message}
    );

    REQUIRE(state.orders().find(100) == nullptr);

    REQUIRE(state.missingBooks() == 0);
    REQUIRE(state.failedBookReductions() == 0);
    REQUIRE(state.failedBookRemovals() == 0);
}

TEST_CASE(
    "Valid cancellation produces no book integrity errors"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{makeAdd()}
    );

    llt::itch::OrderCancelMessage message{};
    message.orderReferenceNumber = 100;
    message.cancelledShares = 200;

    state.onMessage(
        llt::itch::ItchMessage{message}
    );

    REQUIRE(state.missingBooks() == 0);
    REQUIRE(state.failedBookReductions() == 0);
    REQUIRE(state.failedBookRemovals() == 0);
}

TEST_CASE(
    "Valid deletion produces no book integrity errors"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{makeAdd()}
    );

    llt::itch::OrderDeleteMessage message{};
    message.orderReferenceNumber = 100;

    state.onMessage(
        llt::itch::ItchMessage{message}
    );

    REQUIRE(state.orders().find(100) == nullptr);

    REQUIRE(state.missingBooks() == 0);
    REQUIRE(state.failedBookReductions() == 0);
    REQUIRE(state.failedBookRemovals() == 0);
}

TEST_CASE(
    "Complete lifecycle leaves no book integrity errors"
)
{
    llt::itch::ItchMarketState state;

    state.onMessage(
        llt::itch::ItchMessage{makeAdd()}
    );

    llt::itch::OrderExecutedMessage execution{};
    execution.orderReferenceNumber = 100;
    execution.executedShares = 100;

    state.onMessage(
        llt::itch::ItchMessage{execution}
    );

    llt::itch::OrderCancelMessage cancel{};
    cancel.orderReferenceNumber = 100;
    cancel.cancelledShares = 100;

    state.onMessage(
        llt::itch::ItchMessage{cancel}
    );

    llt::itch::OrderDeleteMessage deletion{};
    deletion.orderReferenceNumber = 100;

    state.onMessage(
        llt::itch::ItchMessage{deletion}
    );

    REQUIRE(state.orders().find(100) == nullptr);

    REQUIRE(state.missingBooks() == 0);
    REQUIRE(state.failedBookReductions() == 0);
    REQUIRE(state.failedBookRemovals() == 0);
}