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

std::array<std::uint8_t, 19>
makeValidBrokenTrade()
{
    return {
        // Message Type
        'B',

        // Stock Locate = 0x1234
        0x12, 0x34,

        // Tracking Number = 0x5678
        0x56, 0x78,

        // Timestamp = 0x010203040506
        0x01, 0x02, 0x03,
        0x04, 0x05, 0x06,

        // Match Number = 0x0102030405060708
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08
    };
}

} // namespace


TEST_CASE(
    "ItchDecoder decodes Broken Trade message"
)
{
    const auto data =
        makeValidBrokenTrade();

    BrokenTradeMessage message;

    REQUIRE(
        ItchDecoder::decodeBrokenTrade(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(
        message.stockLocate ==
        0x1234
    );

    REQUIRE(
        message.trackingNumber ==
        0x5678
    );

    REQUIRE(
        message.timestamp ==
        0x010203040506ULL
    );

    REQUIRE(
        message.matchNumber ==
        0x0102030405060708ULL
    );
}


TEST_CASE(
    "ItchDecoder rejects truncated Broken Trade message"
)
{
    const auto data =
        makeValidBrokenTrade();

    BrokenTradeMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeBrokenTrade(
            data.data(),
            data.size() - 1,
            message
        )
    );
}


TEST_CASE(
    "ItchDecoder rejects oversized Broken Trade message"
)
{
    std::array<std::uint8_t, 20> data{};

    data[0] = 'B';

    BrokenTradeMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeBrokenTrade(
            data.data(),
            data.size(),
            message
        )
    );
}


TEST_CASE(
    "ItchDecoder rejects wrong Broken Trade message type"
)
{
    auto data =
        makeValidBrokenTrade();

    data[0] = 'P';

    BrokenTradeMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeBrokenTrade(
            data.data(),
            data.size(),
            message
        )
    );
}


TEST_CASE(
    "ItchDecoder rejects null Broken Trade data"
)
{
    BrokenTradeMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeBrokenTrade(
            nullptr,
            19,
            message
        )
    );
}


TEST_CASE(
    "ItchDecoder decodes maximum Broken Trade match number"
)
{
    auto data =
        makeValidBrokenTrade();

    for (std::size_t i = 11;
         i < 19;
         ++i)
    {
        data[i] = 0xFF;
    }

    BrokenTradeMessage message;

    REQUIRE(
        ItchDecoder::decodeBrokenTrade(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(
        message.matchNumber ==
        UINT64_MAX
    );
}


TEST_CASE(
    "ItchDecoder decodes maximum Broken Trade header fields"
)
{
    auto data =
        makeValidBrokenTrade();

    // Stock Locate
    data[1] = 0xFF;
    data[2] = 0xFF;

    // Tracking Number
    data[3] = 0xFF;
    data[4] = 0xFF;

    // 48-bit timestamp
    for (std::size_t i = 5;
         i < 11;
         ++i)
    {
        data[i] = 0xFF;
    }

    BrokenTradeMessage message;

    REQUIRE(
        ItchDecoder::decodeBrokenTrade(
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
    "ItchDispatcher routes Broken Trade message"
)
{
    const auto data =
        makeValidBrokenTrade();

    const auto result =
        ItchDispatcher::dispatch(
            data.data(),
            data.size()
        );

    REQUIRE(result.has_value());

    REQUIRE(
        std::holds_alternative<
            BrokenTradeMessage
        >(*result)
    );

    const auto& message =
        std::get<BrokenTradeMessage>(
            *result
        );

    REQUIRE(
        message.matchNumber ==
        0x0102030405060708ULL
    );
}


TEST_CASE(
    "ItchDispatcher rejects malformed Broken Trade message"
)
{
    const auto data =
        makeValidBrokenTrade();

    const auto result =
        ItchDispatcher::dispatch(
            data.data(),
            data.size() - 1
        );

    REQUIRE_FALSE(
        result.has_value()
    );
}


TEST_CASE(
    "ItchReplay counts Broken Trade as supported"
)
{
    const auto payload =
        makeValidBrokenTrade();

    std::string data;

    // BinaryFILE payload length:
    // 19 decimal = 0x0013
    data.push_back('\x00');
    data.push_back('\x13');

    for (const auto byte : payload)
    {
        data.push_back(
            static_cast<char>(byte)
        );
    }

    // End-of-session marker
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
        result.stats.brokenTrades == 1
    );

    REQUIRE(
        result.stats.unsupportedMessages == 0
    );

    REQUIRE(
        result.stats.malformedMessages == 0
    );

    REQUIRE(
        result.stats.unsupportedByType[
            static_cast<std::uint8_t>('B')
        ] == 0
    );
}