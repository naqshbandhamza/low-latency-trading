#include <array>
#include <cstdint>
#include <variant>

#include <catch2/catch_test_macros.hpp>

#include "market_data/itch/ItchDispatcher.h"
#include "market_data/itch/messages/SystemEventMessage.h"
#include "market_data/itch/messages/StockDirectoryMessage.h"
#include "market_data/itch/messages/AddOrderMessage.h"
#include "market_data/itch/messages/OrderExecutedMessage.h"
#include "market_data/itch/messages/OrderExecutedWithPriceMessage.h"
#include "market_data/itch/messages/OrderCancelMessage.h"
#include "market_data/itch/messages/OrderDeleteMessage.h"
#include "market_data/itch/messages/OrderReplaceMessage.h"



TEST_CASE(
    "ITCH dispatcher routes System Event message"
)
{
    std::array<std::uint8_t, 12>
        data{};

    data[0] = 'S';
    data[11] = 'O';

    const auto result =
        llt::itch::ItchDispatcher::dispatch(
            data.data(),
            data.size()
        );

    REQUIRE(result.has_value());

    REQUIRE(
        std::holds_alternative<
            llt::itch::SystemEventMessage
        >(*result)
    );

    const auto& message =
        std::get<
            llt::itch::SystemEventMessage
        >(*result);

    REQUIRE(message.eventCode == 'O');
}

TEST_CASE(
    "ITCH dispatcher routes Stock Directory message"
)
{
    std::array<std::uint8_t, 39>
        data{};

    data[0] = 'R';

    data[11] = 'A';
    data[12] = 'A';
    data[13] = 'P';
    data[14] = 'L';
    data[15] = ' ';
    data[16] = ' ';
    data[17] = ' ';
    data[18] = ' ';

    const auto result =
        llt::itch::ItchDispatcher::dispatch(
            data.data(),
            data.size()
        );

    REQUIRE(result.has_value());

    REQUIRE(
        std::holds_alternative<
            llt::itch::StockDirectoryMessage
        >(*result)
    );

    const auto& message =
        std::get<
            llt::itch::StockDirectoryMessage
        >(*result);

    REQUIRE(
        message.stockView() == "AAPL"
    );
}

TEST_CASE(
    "ITCH dispatcher routes Add Order message"
)
{
    std::array<std::uint8_t, 36>
        data{};

    data[0] = 'A';
    data[19] = 'B';

    const auto result =
        llt::itch::ItchDispatcher::dispatch(
            data.data(),
            data.size()
        );

    REQUIRE(result.has_value());

    REQUIRE(
        std::holds_alternative<
            llt::itch::AddOrderMessage
        >(*result)
    );

    const auto& message =
        std::get<
            llt::itch::AddOrderMessage
        >(*result);

    REQUIRE(
        message.buySellIndicator == 'B'
    );
}

TEST_CASE(
    "ITCH dispatcher routes Order Executed message"
)
{
    std::array<std::uint8_t, 31>
        data{};

    data[0] = 'E';

    const auto result =
        llt::itch::ItchDispatcher::dispatch(
            data.data(),
            data.size()
        );

    REQUIRE(result.has_value());

    REQUIRE(
        std::holds_alternative<
            llt::itch::OrderExecutedMessage
        >(*result)
    );
}

TEST_CASE(
    "ITCH dispatcher routes Order Executed With Price message"
)
{
    std::array<std::uint8_t, 36>
        data{};

    data[0] = 'C';
    data[31] = 'Y';

    const auto result =
        llt::itch::ItchDispatcher::dispatch(
            data.data(),
            data.size()
        );

    REQUIRE(result.has_value());

    REQUIRE(
        std::holds_alternative<
            llt::itch::OrderExecutedWithPriceMessage
        >(*result)
    );

    const auto& message =
        std::get<
            llt::itch::OrderExecutedWithPriceMessage
        >(*result);

    REQUIRE(
        message.printable == 'Y'
    );
}

TEST_CASE(
    "ITCH dispatcher routes Order Cancel message"
)
{
    std::array<std::uint8_t, 23>
        data{};

    data[0] = 'X';

    const auto result =
        llt::itch::ItchDispatcher::dispatch(
            data.data(),
            data.size()
        );

    REQUIRE(result.has_value());

    REQUIRE(
        std::holds_alternative<
            llt::itch::OrderCancelMessage
        >(*result)
    );
}

TEST_CASE(
    "ITCH dispatcher routes Order Delete message"
)
{
    std::array<std::uint8_t, 19>
        data{};

    data[0] = 'D';

    const auto result =
        llt::itch::ItchDispatcher::dispatch(
            data.data(),
            data.size()
        );

    REQUIRE(result.has_value());

    REQUIRE(
        std::holds_alternative<
            llt::itch::OrderDeleteMessage
        >(*result)
    );
}

TEST_CASE(
    "ITCH dispatcher routes Order Replace message"
)
{
    std::array<std::uint8_t, 35>
        data{};

    data[0] = 'U';

    const auto result =
        llt::itch::ItchDispatcher::dispatch(
            data.data(),
            data.size()
        );

    REQUIRE(result.has_value());

    REQUIRE(
        std::holds_alternative<
            llt::itch::OrderReplaceMessage
        >(*result)
    );
}

TEST_CASE(
    "ITCH dispatcher rejects unsupported message type"
)
{
    const std::array<std::uint8_t, 1>
        data{
            'Z'
        };

    const auto result =
        llt::itch::ItchDispatcher::dispatch(
            data.data(),
            data.size()
        );

    REQUIRE_FALSE(
        result.has_value()
    );
}

TEST_CASE(
    "ITCH dispatcher rejects null data"
)
{
    const auto result =
        llt::itch::ItchDispatcher::dispatch(
            nullptr,
            12
        );

    REQUIRE_FALSE(
        result.has_value()
    );
}

TEST_CASE(
    "ITCH dispatcher rejects zero length message"
)
{
    const std::uint8_t byte = 'S';

    const auto result =
        llt::itch::ItchDispatcher::dispatch(
            &byte,
            0
        );

    REQUIRE_FALSE(
        result.has_value()
    );
}

TEST_CASE(
    "ITCH dispatcher rejects known message type with incorrect length"
)
{
    const std::array<std::uint8_t, 35>
        data{
            'A'
        };

    const auto result =
        llt::itch::ItchDispatcher::dispatch(
            data.data(),
            data.size()
        );

    REQUIRE_FALSE(
        result.has_value()
    );
}

