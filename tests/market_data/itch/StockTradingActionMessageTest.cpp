#include <catch2/catch_test_macros.hpp>

#include <array>
#include <cstdint>
#include <sstream>
#include <string>
#include <variant>

#include "market_data/itch/ItchDecoder.h"
#include "market_data/itch/ItchDispatcher.h"
#include "market_data/itch/ItchReplay.h"

using namespace llt::itch;

namespace
{

std::array<std::uint8_t, 25>
makeValidStockTradingAction()
{
    return {
        // Type
        'H',

        // Stock Locate
        0x12, 0x34,

        // Tracking Number
        0x56, 0x78,

        // Timestamp = 0x010203040506
        0x01, 0x02, 0x03,
        0x04, 0x05, 0x06,

        // Stock = "AAPL    "
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' ',

        // Trading State = Trading
        'T',

        // Reserved
        ' ',

        // Reason
        ' ', ' ', ' ', ' '
    };
}

} // namespace


TEST_CASE(
    "ItchDecoder decodes Stock Trading Action message"
)
{
    const auto data =
        makeValidStockTradingAction();

    StockTradingActionMessage message;

    REQUIRE(
        ItchDecoder::decodeStockTradingAction(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(message.stockLocate == 0x1234);
    REQUIRE(message.trackingNumber == 0x5678);

    REQUIRE(
        message.timestamp ==
        0x010203040506ULL
    );

    REQUIRE(message.stockView() == "AAPL");
    REQUIRE(message.tradingState == 'T');
    REQUIRE(message.reserved == ' ');
    REQUIRE(message.reasonView().empty());
}


TEST_CASE(
    "Stock Trading Action preserves trading state"
)
{
    auto data =
        makeValidStockTradingAction();

    data[19] = 'H';

    StockTradingActionMessage message;

    REQUIRE(
        ItchDecoder::decodeStockTradingAction(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(message.tradingState == 'H');
}


TEST_CASE(
    "Stock Trading Action preserves reason code"
)
{
    auto data =
        makeValidStockTradingAction();

    data[19] = 'H';

    data[21] = 'L';
    data[22] = 'U';
    data[23] = 'D';
    data[24] = 'P';

    StockTradingActionMessage message;

    REQUIRE(
        ItchDecoder::decodeStockTradingAction(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(
        message.reasonView() == "LUDP"
    );
}


TEST_CASE(
    "Stock Trading Action trims padded reason"
)
{
    auto data =
        makeValidStockTradingAction();

    data[21] = 'X';
    data[22] = 'Y';
    data[23] = ' ';
    data[24] = ' ';

    StockTradingActionMessage message;

    REQUIRE(
        ItchDecoder::decodeStockTradingAction(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(
        message.reasonView() == "XY"
    );
}


TEST_CASE(
    "Stock Trading Action preserves eight character stock"
)
{
    auto data =
        makeValidStockTradingAction();

    const std::array<char, 8> stock{
        'A', 'B', 'C', 'D',
        'E', 'F', 'G', 'H'
    };

    for (std::size_t i = 0;
         i < stock.size();
         ++i)
    {
        data[11 + i] =
            static_cast<std::uint8_t>(
                stock[i]
            );
    }

    StockTradingActionMessage message;

    REQUIRE(
        ItchDecoder::decodeStockTradingAction(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(
        message.stockView() ==
        "ABCDEFGH"
    );
}


TEST_CASE(
    "ItchDecoder rejects truncated Stock Trading Action"
)
{
    const auto data =
        makeValidStockTradingAction();

    StockTradingActionMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeStockTradingAction(
            data.data(),
            data.size() - 1,
            message
        )
    );
}


TEST_CASE(
    "ItchDecoder rejects oversized Stock Trading Action"
)
{
    std::array<std::uint8_t, 26> data{};

    data[0] = 'H';

    StockTradingActionMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeStockTradingAction(
            data.data(),
            data.size(),
            message
        )
    );
}


TEST_CASE(
    "ItchDecoder rejects wrong Stock Trading Action type"
)
{
    auto data =
        makeValidStockTradingAction();

    data[0] = 'A';

    StockTradingActionMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeStockTradingAction(
            data.data(),
            data.size(),
            message
        )
    );
}


TEST_CASE(
    "ItchDecoder rejects null Stock Trading Action data"
)
{
    StockTradingActionMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeStockTradingAction(
            nullptr,
            25,
            message
        )
    );
}


TEST_CASE(
    "ItchDecoder decodes maximum Stock Trading Action header fields"
)
{
    auto data =
        makeValidStockTradingAction();

    data[1] = 0xFF;
    data[2] = 0xFF;

    data[3] = 0xFF;
    data[4] = 0xFF;

    for (std::size_t i = 5;
         i < 11;
         ++i)
    {
        data[i] = 0xFF;
    }

    StockTradingActionMessage message;

    REQUIRE(
        ItchDecoder::decodeStockTradingAction(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(
        message.stockLocate ==
        UINT16_MAX
    );

    REQUIRE(
        message.trackingNumber ==
        UINT16_MAX
    );

    REQUIRE(
        message.timestamp ==
        0xFFFFFFFFFFFFULL
    );
}


TEST_CASE(
    "ItchDispatcher routes Stock Trading Action"
)
{
    const auto data =
        makeValidStockTradingAction();

    const auto result =
        ItchDispatcher::dispatch(
            data.data(),
            data.size()
        );

    REQUIRE(result.has_value());

    REQUIRE(
        std::holds_alternative<
            StockTradingActionMessage
        >(*result)
    );

    const auto& message =
        std::get<
            StockTradingActionMessage
        >(*result);

    REQUIRE(message.stockView() == "AAPL");
    REQUIRE(message.tradingState == 'T');
}


TEST_CASE(
    "ItchDispatcher rejects malformed Stock Trading Action"
)
{
    const auto data =
        makeValidStockTradingAction();

    const auto result =
        ItchDispatcher::dispatch(
            data.data(),
            data.size() - 1
        );

    REQUIRE_FALSE(result.has_value());
}


TEST_CASE(
    "ItchReplay counts Stock Trading Action as supported"
)
{
    const auto payload =
        makeValidStockTradingAction();

    std::string data;

    // 25 decimal = 0x0019
    data.push_back('\x00');
    data.push_back('\x19');

    for (const auto byte : payload)
    {
        data.push_back(
            static_cast<char>(byte)
        );
    }

    // BinaryFILE terminator
    data.push_back('\x00');
    data.push_back('\x00');

    std::istringstream stream(data);

    const auto result =
        ItchReplay::run(stream);

    REQUIRE(
        result.status ==
        ItchReplayStatus::Complete
    );

    REQUIRE(result.stats.recordsRead == 1);
    REQUIRE(result.stats.decodedMessages == 1);

    REQUIRE(
        result.stats.stockTradingActions == 1
    );

    REQUIRE(
        result.stats.unsupportedMessages == 0
    );

    REQUIRE(
        result.stats.malformedMessages == 0
    );

    REQUIRE(
        result.stats.unsupportedByType[
            static_cast<std::uint8_t>('H')
        ] == 0
    );
}