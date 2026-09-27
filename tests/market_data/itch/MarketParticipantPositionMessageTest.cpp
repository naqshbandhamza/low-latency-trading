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

std::array<std::uint8_t, 26>
makeValidMarketParticipantPosition()
{
    return {
        // Type
        'L',

        // Stock Locate
        0x12, 0x34,

        // Tracking Number
        0x56, 0x78,

        // Timestamp = 0x010203040506
        0x01, 0x02, 0x03,
        0x04, 0x05, 0x06,

        // MPID = "ABCD"
        'A', 'B', 'C', 'D',

        // Stock = "AAPL    "
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' ',

        // Primary Market Maker
        'Y',

        // Market Maker Mode = Normal
        'N',

        // Market Participant State = Active
        'A'
    };
}

} // namespace


TEST_CASE(
    "ItchDecoder decodes Market Participant Position message"
)
{
    const auto data =
        makeValidMarketParticipantPosition();

    MarketParticipantPositionMessage message;

    REQUIRE(
        ItchDecoder::decodeMarketParticipantPosition(
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

    REQUIRE(message.mpidView() == "ABCD");
    REQUIRE(message.stockView() == "AAPL");

    REQUIRE(
        message.primaryMarketMaker == 'Y'
    );

    REQUIRE(
        message.marketMakerMode == 'N'
    );

    REQUIRE(
        message.marketParticipantState == 'A'
    );
}


TEST_CASE(
    "Market Participant Position preserves four character MPID"
)
{
    auto data =
        makeValidMarketParticipantPosition();

    data[11] = 'X';
    data[12] = 'Y';
    data[13] = 'Z';
    data[14] = 'W';

    MarketParticipantPositionMessage message;

    REQUIRE(
        ItchDecoder::decodeMarketParticipantPosition(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(
        message.mpidView() == "XYZW"
    );
}


TEST_CASE(
    "Market Participant Position trims padded MPID"
)
{
    auto data =
        makeValidMarketParticipantPosition();

    data[11] = 'X';
    data[12] = 'Y';
    data[13] = ' ';
    data[14] = ' ';

    MarketParticipantPositionMessage message;

    REQUIRE(
        ItchDecoder::decodeMarketParticipantPosition(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(
        message.mpidView() == "XY"
    );
}


TEST_CASE(
    "Market Participant Position preserves eight character stock"
)
{
    auto data =
        makeValidMarketParticipantPosition();

    const std::array<char, 8> stock{
        'A', 'B', 'C', 'D',
        'E', 'F', 'G', 'H'
    };

    for (std::size_t i = 0;
         i < stock.size();
         ++i)
    {
        data[15 + i] =
            static_cast<std::uint8_t>(
                stock[i]
            );
    }

    MarketParticipantPositionMessage message;

    REQUIRE(
        ItchDecoder::decodeMarketParticipantPosition(
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
    "Market Participant Position preserves status fields"
)
{
    auto data =
        makeValidMarketParticipantPosition();

    data[23] = 'N';
    data[24] = 'P';
    data[25] = 'S';

    MarketParticipantPositionMessage message;

    REQUIRE(
        ItchDecoder::decodeMarketParticipantPosition(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(
        message.primaryMarketMaker == 'N'
    );

    REQUIRE(
        message.marketMakerMode == 'P'
    );

    REQUIRE(
        message.marketParticipantState == 'S'
    );
}


TEST_CASE(
    "ItchDecoder rejects truncated Market Participant Position"
)
{
    const auto data =
        makeValidMarketParticipantPosition();

    MarketParticipantPositionMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeMarketParticipantPosition(
            data.data(),
            data.size() - 1,
            message
        )
    );
}


TEST_CASE(
    "ItchDecoder rejects oversized Market Participant Position"
)
{
    std::array<std::uint8_t, 27> data{};

    data[0] = 'L';

    MarketParticipantPositionMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeMarketParticipantPosition(
            data.data(),
            data.size(),
            message
        )
    );
}


TEST_CASE(
    "ItchDecoder rejects wrong Market Participant Position type"
)
{
    auto data =
        makeValidMarketParticipantPosition();

    data[0] = 'A';

    MarketParticipantPositionMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeMarketParticipantPosition(
            data.data(),
            data.size(),
            message
        )
    );
}


TEST_CASE(
    "ItchDecoder rejects null Market Participant Position data"
)
{
    MarketParticipantPositionMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeMarketParticipantPosition(
            nullptr,
            26,
            message
        )
    );
}


TEST_CASE(
    "ItchDecoder decodes maximum Market Participant Position header fields"
)
{
    auto data =
        makeValidMarketParticipantPosition();

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

    MarketParticipantPositionMessage message;

    REQUIRE(
        ItchDecoder::decodeMarketParticipantPosition(
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
    "ItchDispatcher routes Market Participant Position"
)
{
    const auto data =
        makeValidMarketParticipantPosition();

    const auto result =
        ItchDispatcher::dispatch(
            data.data(),
            data.size()
        );

    REQUIRE(result.has_value());

    REQUIRE(
        std::holds_alternative<
            MarketParticipantPositionMessage
        >(*result)
    );

    const auto& message =
        std::get<
            MarketParticipantPositionMessage
        >(*result);

    REQUIRE(message.mpidView() == "ABCD");
    REQUIRE(message.stockView() == "AAPL");
}


TEST_CASE(
    "ItchDispatcher rejects malformed Market Participant Position"
)
{
    const auto data =
        makeValidMarketParticipantPosition();

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
    "ItchReplay counts Market Participant Position as supported"
)
{
    const auto payload =
        makeValidMarketParticipantPosition();

    std::string data;

    // 26 decimal = 0x001A
    data.push_back('\x00');
    data.push_back('\x1A');

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
        result.stats.marketParticipantPositions == 1
    );

    REQUIRE(
        result.stats.unsupportedMessages == 0
    );

    REQUIRE(
        result.stats.malformedMessages == 0
    );

    REQUIRE(
        result.stats.unsupportedByType[
            static_cast<std::uint8_t>('L')
        ] == 0
    );
}