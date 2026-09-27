#include <catch2/catch_test_macros.hpp>

#include <array>
#include <cstdint>
#include <sstream>
#include <string>

#include "market_data/itch/ItchDecoder.h"
#include "market_data/itch/ItchReplay.h"
#include "market_data/itch/ItchDispatcher.h"

using namespace llt::itch;

namespace
{

std::array<std::uint8_t, 40>
makeValidAddOrderWithMpid()
{
    return {
        // Type
        'F',

        // Stock Locate = 0x1234
        0x12, 0x34,

        // Tracking Number = 0x5678
        0x56, 0x78,

        // Timestamp = 0x010203040506
        0x01, 0x02, 0x03,
        0x04, 0x05, 0x06,

        // Order Reference Number
        // = 0x0102030405060708
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08,

        // Buy/Sell
        'B',

        // Shares = 100
        0x00, 0x00, 0x00, 0x64,

        // Stock = "AAPL    "
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' ',

        // Price = 1874200
        // 0x001C9918
        0x00, 0x1C, 0x99, 0x18,

        // Attribution = "ABCD"
        'A', 'B', 'C', 'D'
    };
}

}




TEST_CASE(
    "ItchDecoder decodes Add Order with MPID"
)
{
    const auto data =
        makeValidAddOrderWithMpid();

    AddOrderWithMpidMessage message;

    REQUIRE(
        ItchDecoder::decodeAddOrderWithMpid(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(
        message.stockLocate == 0x1234
    );

    REQUIRE(
        message.trackingNumber == 0x5678
    );

    REQUIRE(
        message.timestamp ==
        0x010203040506ULL
    );

    REQUIRE(
        message.orderReferenceNumber ==
        0x0102030405060708ULL
    );

    REQUIRE(
        message.buySellIndicator == 'B'
    );

    REQUIRE(
        message.shares == 100
    );

    REQUIRE(
        message.stockView() == "AAPL"
    );

    REQUIRE(
        message.price == 1874200
    );

    REQUIRE(
        message.attributionView() == "ABCD"
    );
}


TEST_CASE(
    "ItchDecoder rejects truncated Add Order with MPID"
)
{
    const auto data =
        makeValidAddOrderWithMpid();

    AddOrderWithMpidMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeAddOrderWithMpid(
            data.data(),
            data.size() - 1,
            message
        )
    );
}

TEST_CASE(
    "ItchDecoder rejects oversized Add Order with MPID"
)
{
    std::array<std::uint8_t, 41> data{};

    data[0] = 'F';

    AddOrderWithMpidMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeAddOrderWithMpid(
            data.data(),
            data.size(),
            message
        )
    );
}

TEST_CASE(
    "ItchDecoder rejects wrong Add Order with MPID type"
)
{
    auto data =
        makeValidAddOrderWithMpid();

    data[0] = 'A';

    AddOrderWithMpidMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeAddOrderWithMpid(
            data.data(),
            data.size(),
            message
        )
    );
}

TEST_CASE(
    "ItchDecoder rejects null Add Order with MPID data"
)
{
    AddOrderWithMpidMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeAddOrderWithMpid(
            nullptr,
            40,
            message
        )
    );
}


TEST_CASE(
    "Add Order with MPID trims padded attribution"
)
{
    auto data =
        makeValidAddOrderWithMpid();

    data[36] = 'X';
    data[37] = 'Y';
    data[38] = ' ';
    data[39] = ' ';

    AddOrderWithMpidMessage message;

    REQUIRE(
        ItchDecoder::decodeAddOrderWithMpid(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(
        message.attributionView() == "XY"
    );
}

TEST_CASE(
    "ItchDispatcher routes Add Order with MPID"
)
{
    std::array<std::uint8_t, 40> data{};

    data[0] = 'F';

    const auto result =
        ItchDispatcher::dispatch(
            data.data(),
            data.size()
        );

    REQUIRE(result.has_value());

    REQUIRE(
        std::holds_alternative<
            AddOrderWithMpidMessage
        >(*result)
    );
}


TEST_CASE(
    "ItchReplay counts Add Order with MPID as supported"
)
{
    std::string data;

    std::array<std::uint8_t, 40> payload{};
    payload[0] = 'F';

    // BinaryFILE length = 40
    data.push_back('\x00');
    data.push_back('\x28');

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
        result.stats.addOrdersWithMpid == 1
    );

    REQUIRE(
        result.stats.unsupportedMessages == 0
    );

    REQUIRE(
        result.stats.unsupportedByType[
            static_cast<std::uint8_t>('F')
        ] == 0
    );
}


