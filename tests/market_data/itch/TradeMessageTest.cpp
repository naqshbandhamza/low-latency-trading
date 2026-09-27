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

std::array<std::uint8_t, 44>
makeValidTrade()
{
    return {
        // Type
        'P',

        // Stock Locate = 0x1234
        0x12, 0x34,

        // Tracking Number = 0x5678
        0x56, 0x78,

        // Timestamp = 0x010203040506
        0x01, 0x02, 0x03,
        0x04, 0x05, 0x06,

        // Order Reference Number = 0
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,

        // Buy/Sell Indicator
        'B',

        // Shares = 100
        0x00, 0x00, 0x00, 0x64,

        // Stock = "AAPL    "
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' ',

        // Price = 1874200 = 0x001C9918
        0x00, 0x1C, 0x99, 0x18,

        // Match Number = 0x0102030405060708
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08
    };
}


TEST_CASE(
    "ItchDecoder decodes Trade message"
)
{
    const auto data =
        makeValidTrade();

    TradeMessage message;

    REQUIRE(
        ItchDecoder::decodeTrade(
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

    REQUIRE(
        message.orderReferenceNumber == 0
    );

    REQUIRE(
        message.buySellIndicator == 'B'
    );

    REQUIRE(message.shares == 100);

    REQUIRE(
        message.stockView() == "AAPL"
    );

    REQUIRE(message.price == 1874200);

    REQUIRE(
        message.matchNumber ==
        0x0102030405060708ULL
    );
}


TEST_CASE(
    "Trade message preserves eight character stock"
)
{
    auto data =
        makeValidTrade();

    const std::array<char, 8> stock{
        'A', 'B', 'C', 'D',
        'E', 'F', 'G', 'H'
    };

    for (std::size_t i = 0;
         i < stock.size();
         ++i)
    {
        data[24 + i] =
            static_cast<std::uint8_t>(
                stock[i]
            );
    }

    TradeMessage message;

    REQUIRE(
        ItchDecoder::decodeTrade(
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
    "ItchDecoder rejects truncated Trade message"
)
{
    const auto data =
        makeValidTrade();

    TradeMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeTrade(
            data.data(),
            data.size() - 1,
            message
        )
    );
}

TEST_CASE(
    "ItchDecoder rejects oversized Trade message"
)
{
    std::array<std::uint8_t, 45> data{};
    data[0] = 'P';

    TradeMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeTrade(
            data.data(),
            data.size(),
            message
        )
    );
}

TEST_CASE(
    "ItchDecoder rejects wrong Trade message type"
)
{
    auto data =
        makeValidTrade();

    data[0] = 'A';

    TradeMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeTrade(
            data.data(),
            data.size(),
            message
        )
    );
}

TEST_CASE(
    "ItchDecoder rejects null Trade message data"
)
{
    TradeMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeTrade(
            nullptr,
            44,
            message
        )
    );
}

TEST_CASE(
    "ItchDecoder decodes maximum Trade numeric fields"
)
{
    auto data =
        makeValidTrade();

    // Order Reference Number
    for (std::size_t i = 11; i < 19; ++i)
        data[i] = 0xFF;

    // Shares
    for (std::size_t i = 20; i < 24; ++i)
        data[i] = 0xFF;

    // Price
    for (std::size_t i = 32; i < 36; ++i)
        data[i] = 0xFF;

    // Match Number
    for (std::size_t i = 36; i < 44; ++i)
        data[i] = 0xFF;

    TradeMessage message;

    REQUIRE(
        ItchDecoder::decodeTrade(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(
        message.orderReferenceNumber ==
        UINT64_MAX
    );

    REQUIRE(
        message.shares ==
        UINT32_MAX
    );

    REQUIRE(
        message.price ==
        UINT32_MAX
    );

    REQUIRE(
        message.matchNumber ==
        UINT64_MAX
    );
}

TEST_CASE(
    "ItchDispatcher routes Trade message"
)
{
    const auto data =
        makeValidTrade();

    const auto result =
        ItchDispatcher::dispatch(
            data.data(),
            data.size()
        );

    REQUIRE(result.has_value());

    REQUIRE(
        std::holds_alternative<
            TradeMessage
        >(*result)
    );
}

TEST_CASE(
    "ItchReplay counts Trade message as supported"
)
{
    const auto payload =
        makeValidTrade();

    std::string data;

    // BinaryFILE payload length = 44 = 0x002C
    data.push_back('\x00');
    data.push_back('\x2C');

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

    REQUIRE(
        result.stats.recordsRead == 1
    );

    REQUIRE(
        result.stats.decodedMessages == 1
    );

    REQUIRE(
        result.stats.trades == 1
    );

    REQUIRE(
        result.stats.unsupportedMessages == 0
    );

    REQUIRE(
        result.stats.unsupportedByType[
            static_cast<std::uint8_t>('P')
        ] == 0
    );
}



} // namespace