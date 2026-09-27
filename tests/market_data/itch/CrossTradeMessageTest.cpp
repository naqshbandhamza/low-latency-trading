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

std::array<std::uint8_t, 40>
makeValidCrossTrade()
{
    return {
        // Type
        'Q',

        // Stock Locate = 0x1234
        0x12, 0x34,

        // Tracking Number = 0x5678
        0x56, 0x78,

        // Timestamp = 0x010203040506
        0x01, 0x02, 0x03,
        0x04, 0x05, 0x06,

        // Shares = 100
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x64,

        // Stock = "AAPL    "
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' ',

        // Cross Price = 1874200
        0x00, 0x1C, 0x99, 0x18,

        // Match Number
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08,

        // Opening Cross
        'O'
    };
}


TEST_CASE(
    "ItchDecoder decodes Cross Trade message"
)
{
    const auto data =
        makeValidCrossTrade();

    CrossTradeMessage message;

    REQUIRE(
        ItchDecoder::decodeCrossTrade(
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

    REQUIRE(message.shares == 100);

    REQUIRE(
        message.stockView() == "AAPL"
    );

    REQUIRE(
        message.crossPrice == 1874200
    );

    REQUIRE(
        message.matchNumber ==
        0x0102030405060708ULL
    );

    REQUIRE(message.crossType == 'O');
}

TEST_CASE(
    "Cross Trade preserves eight character stock"
)
{
    auto data =
        makeValidCrossTrade();

    const std::array<char, 8> stock{
        'A', 'B', 'C', 'D',
        'E', 'F', 'G', 'H'
    };

    for (std::size_t i = 0;
         i < stock.size();
         ++i)
    {
        data[19 + i] =
            static_cast<std::uint8_t>(
                stock[i]
            );
    }

    CrossTradeMessage message;

    REQUIRE(
        ItchDecoder::decodeCrossTrade(
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
    "ItchDecoder rejects truncated Cross Trade message"
)
{
    const auto data =
        makeValidCrossTrade();

    CrossTradeMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeCrossTrade(
            data.data(),
            data.size() - 1,
            message
        )
    );
}

TEST_CASE(
    "ItchDecoder rejects oversized Cross Trade message"
)
{
    std::array<std::uint8_t, 41> data{};
    data[0] = 'Q';

    CrossTradeMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeCrossTrade(
            data.data(),
            data.size(),
            message
        )
    );
}

TEST_CASE(
    "ItchDecoder rejects wrong Cross Trade message type"
)
{
    auto data =
        makeValidCrossTrade();

    data[0] = 'P';

    CrossTradeMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeCrossTrade(
            data.data(),
            data.size(),
            message
        )
    );
}

TEST_CASE(
    "ItchDecoder rejects null Cross Trade data"
)
{
    CrossTradeMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeCrossTrade(
            nullptr,
            40,
            message
        )
    );
}

TEST_CASE(
    "ItchDecoder decodes maximum Cross Trade shares"
)
{
    auto data =
        makeValidCrossTrade();

    for (std::size_t i = 11;
         i < 19;
         ++i)
    {
        data[i] = 0xFF;
    }

    CrossTradeMessage message;

    REQUIRE(
        ItchDecoder::decodeCrossTrade(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(
        message.shares ==
        UINT64_MAX
    );
}

TEST_CASE(
    "ItchDecoder preserves Cross Trade type"
)
{
    auto data =
        makeValidCrossTrade();

    data[39] = 'C';

    CrossTradeMessage message;

    REQUIRE(
        ItchDecoder::decodeCrossTrade(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(message.crossType == 'C');
}

TEST_CASE(
    "ItchDispatcher routes Cross Trade message"
)
{
    const auto data =
        makeValidCrossTrade();

    const auto result =
        ItchDispatcher::dispatch(
            data.data(),
            data.size()
        );

    REQUIRE(result.has_value());

    REQUIRE(
        std::holds_alternative<
            CrossTradeMessage
        >(*result)
    );
}

TEST_CASE(
    "ItchReplay counts Cross Trade as supported"
)
{
    const auto payload =
        makeValidCrossTrade();

    std::string data;

    // 40 = 0x0028
    data.push_back('\x00');
    data.push_back('\x28');

    for (const auto byte : payload)
    {
        data.push_back(
            static_cast<char>(byte)
        );
    }

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
    REQUIRE(result.stats.crossTrades == 1);
    REQUIRE(result.stats.unsupportedMessages == 0);

    REQUIRE(
        result.stats.unsupportedByType[
            static_cast<std::uint8_t>('Q')
        ] == 0
    );
}



} // namespace