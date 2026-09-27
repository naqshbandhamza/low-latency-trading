#include <catch2/catch_test_macros.hpp>

#include <array>
#include <cstdint>

#include "market_data/itch/ItchDecoder.h"

TEST_CASE(
    "ITCH decoder decodes System Event message")
{
    const std::array<std::uint8_t, 12> data{
        'S',

        // Stock Locate = 0
        0x00,
        0x00,

        // Tracking Number = 42
        0x00,
        0x2A,

        // Timestamp = 1000
        0x00,
        0x00,
        0x00,
        0x00,
        0x03,
        0xE8,

        // Start of Messages
        'O'};

    llt::itch::SystemEventMessage message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeSystemEvent(
                data.data(),
                data.size(),
                message));

    REQUIRE(
        message.stockLocate == 0);

    REQUIRE(
        message.trackingNumber == 42);

    REQUIRE(
        message.timestamp == 1000);

    REQUIRE(
        message.eventCode == 'O');
}

TEST_CASE(
    "ITCH decoder rejects truncated System Event message")
{
    const std::array<std::uint8_t, 11> data{
        'S'};

    llt::itch::SystemEventMessage message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeSystemEvent(
                data.data(),
                data.size(),
                message));
}

TEST_CASE(
    "ITCH decoder rejects oversized System Event message")
{
    const std::array<std::uint8_t, 13> data{
        'S'};

    llt::itch::SystemEventMessage message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeSystemEvent(
                data.data(),
                data.size(),
                message));
}

TEST_CASE(
    "ITCH decoder rejects incorrect message type")
{
    const std::array<std::uint8_t, 12> data{
        'R'};

    llt::itch::SystemEventMessage message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeSystemEvent(
                data.data(),
                data.size(),
                message));
}

TEST_CASE(
    "ITCH decoder rejects null data")
{
    llt::itch::SystemEventMessage message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeSystemEvent(
                nullptr,
                12,
                message));
}

TEST_CASE(
    "ITCH decoder decodes maximum 48 bit timestamp")
{
    const std::array<std::uint8_t, 12> data{
        'S',

        0x00,
        0x00,

        0xFF,
        0xFF,

        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,

        'C'};

    llt::itch::SystemEventMessage message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeSystemEvent(
                data.data(),
                data.size(),
                message));

    REQUIRE(
        message.trackingNumber ==
        65535);

    REQUIRE(
        message.timestamp ==
        0xFFFFFFFFFFFFULL);

    REQUIRE(
        message.eventCode == 'C');
}

TEST_CASE(
    "ITCH decoder decodes Stock Directory message")
{
    const std::array<std::uint8_t, 39> data{
        'R',

        // Stock Locate = 1234
        0x04,
        0xD2,

        // Tracking Number = 42
        0x00,
        0x2A,

        // Timestamp = 1000
        0x00,
        0x00,
        0x00,
        0x00,
        0x03,
        0xE8,

        // Stock = "AAPL    "
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' ',

        // Market Category
        'Q',

        // Financial Status Indicator
        'N',

        // Round Lot Size = 100
        0x00,
        0x00,
        0x00,
        0x64,

        // Round Lots Only
        'N',

        // Issue Classification
        'C',

        // Issue Sub-Type
        'Z', ' ',

        // Authenticity
        'P',

        // Short Sale Threshold Indicator
        'N',

        // IPO Flag
        'N',

        // LULD Reference Price Tier
        '1',

        // ETP Flag
        'N',

        // ETP Leverage Factor = 1
        0x00,
        0x00,
        0x00,
        0x01,

        // Inverse Indicator
        'N'};

    llt::itch::StockDirectoryMessage
        message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeStockDirectory(
                data.data(),
                data.size(),
                message));

    REQUIRE(
        message.stockLocate == 1234);

    REQUIRE(
        message.trackingNumber == 42);

    REQUIRE(
        message.timestamp == 1000);

    REQUIRE(
        message.stockView() == "AAPL");

    REQUIRE(
        message.marketCategory == 'Q');

    REQUIRE(
        message.financialStatusIndicator == 'N');

    REQUIRE(
        message.roundLotSize == 100);

    REQUIRE(
        message.roundLotsOnly == 'N');

    REQUIRE(
        message.etpLeverageFactor == 1);

    REQUIRE(
        message.inverseIndicator == 'N');
}

TEST_CASE(
    "ITCH decoder preserves eight character stock symbol")
{
    std::array<std::uint8_t, 39> data{};

    data[0] = 'R';

    const char stock[8]{
        'A', 'B', 'C', 'D',
        'E', 'F', 'G', 'H'};

    for (
        std::size_t i = 0;
        i < 8;
        ++i)
    {
        data[11 + i] =
            static_cast<std::uint8_t>(
                stock[i]);
    }

    llt::itch::StockDirectoryMessage
        message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeStockDirectory(
                data.data(),
                data.size(),
                message));

    REQUIRE(
        message.stockView() ==
        "ABCDEFGH");
}

TEST_CASE(
    "ITCH decoder rejects truncated Stock Directory message")
{
    const std::array<std::uint8_t, 38>
        data{
            'R'};

    llt::itch::StockDirectoryMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeStockDirectory(
                data.data(),
                data.size(),
                message));
}

TEST_CASE(
    "ITCH decoder rejects incorrect Stock Directory message type")
{
    const std::array<std::uint8_t, 39>
        data{
            'S'};

    llt::itch::StockDirectoryMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeStockDirectory(
                data.data(),
                data.size(),
                message));
}

TEST_CASE(
    "ITCH decoder decodes Add Order message")
{
    const std::array<std::uint8_t, 36> data{
        'A',

        // Stock Locate = 1234
        0x04,
        0xD2,

        // Tracking Number = 42
        0x00,
        0x2A,

        // Timestamp = 1000
        0x00,
        0x00,
        0x00,
        0x00,
        0x03,
        0xE8,

        // Order Reference Number = 123456789
        0x00,
        0x00,
        0x00,
        0x00,
        0x07,
        0x5B,
        0xCD,
        0x15,

        // Buy
        'B',

        // Shares = 500
        0x00,
        0x00,
        0x01,
        0xF4,

        // Stock = "AAPL    "
        'A',
        'A',
        'P',
        'L',
        ' ',
        ' ',
        ' ',
        ' ',

        // Price = 1,874,200
        // Price(4) = $187.4200
        0x00,
        0x1C,
        0x99,
        0x18};

    llt::itch::AddOrderMessage
        message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeAddOrder(
                data.data(),
                data.size(),
                message));

    REQUIRE(
        message.stockLocate == 1234);

    REQUIRE(
        message.trackingNumber == 42);

    REQUIRE(
        message.timestamp == 1000);

    REQUIRE(
        message.orderReferenceNumber ==
        123456789ULL);

    REQUIRE(
        message.buySellIndicator == 'B');

    REQUIRE(
        message.shares == 500);

    REQUIRE(
        message.stockView() == "AAPL");

    REQUIRE(
        message.price == 1874200);
}

TEST_CASE(
    "ITCH decoder decodes sell Add Order message")
{
    std::array<std::uint8_t, 36>
        data{};

    data[0] = 'A';

    data[19] = 'S';

    llt::itch::AddOrderMessage
        message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeAddOrder(
                data.data(),
                data.size(),
                message));

    REQUIRE(
        message.buySellIndicator == 'S');
}

TEST_CASE(
    "ITCH decoder preserves 64 bit order reference number")
{
    std::array<std::uint8_t, 36>
        data{};

    data[0] = 'A';

    // 0xFEDCBA9876543210
    data[11] = 0xFE;
    data[12] = 0xDC;
    data[13] = 0xBA;
    data[14] = 0x98;
    data[15] = 0x76;
    data[16] = 0x54;
    data[17] = 0x32;
    data[18] = 0x10;

    llt::itch::AddOrderMessage
        message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeAddOrder(
                data.data(),
                data.size(),
                message));

    REQUIRE(
        message.orderReferenceNumber ==
        0xFEDCBA9876543210ULL);
}

TEST_CASE(
    "ITCH decoder preserves maximum Add Order integer fields")
{
    std::array<std::uint8_t, 36>
        data{};

    data[0] = 'A';

    // Shares
    data[20] = 0xFF;
    data[21] = 0xFF;
    data[22] = 0xFF;
    data[23] = 0xFF;

    // Price
    data[32] = 0xFF;
    data[33] = 0xFF;
    data[34] = 0xFF;
    data[35] = 0xFF;

    llt::itch::AddOrderMessage
        message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeAddOrder(
                data.data(),
                data.size(),
                message));

    REQUIRE(
        message.shares ==
        0xFFFFFFFFU);

    REQUIRE(
        message.price ==
        0xFFFFFFFFU);
}

TEST_CASE(
    "ITCH decoder rejects truncated Add Order message")
{
    const std::array<std::uint8_t, 35>
        data{
            'A'};

    llt::itch::AddOrderMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeAddOrder(
                data.data(),
                data.size(),
                message));
}

TEST_CASE(
    "ITCH decoder rejects oversized Add Order message")
{
    const std::array<std::uint8_t, 37>
        data{
            'A'};

    llt::itch::AddOrderMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeAddOrder(
                data.data(),
                data.size(),
                message));
}

TEST_CASE(
    "ITCH decoder rejects incorrect Add Order message type")
{
    const std::array<std::uint8_t, 36>
        data{
            'R'};

    llt::itch::AddOrderMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeAddOrder(
                data.data(),
                data.size(),
                message));
}



TEST_CASE(
    "ITCH decoder decodes Order Executed message"
)
{
    const std::array<std::uint8_t, 31> data{
        'E',

        // Stock Locate = 1234
        0x04,
        0xD2,

        // Tracking Number = 42
        0x00,
        0x2A,

        // Timestamp = 1000
        0x00,
        0x00,
        0x00,
        0x00,
        0x03,
        0xE8,

        // Order Reference Number = 123456789
        0x00,
        0x00,
        0x00,
        0x00,
        0x07,
        0x5B,
        0xCD,
        0x15,

        // Executed Shares = 200
        0x00,
        0x00,
        0x00,
        0xC8,

        // Match Number = 987654321
        0x00,
        0x00,
        0x00,
        0x00,
        0x3A,
        0xDE,
        0x68,
        0xB1
    };

    llt::itch::OrderExecutedMessage
        message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeOrderExecuted(
                data.data(),
                data.size(),
                message
            )
    );

    REQUIRE(
        message.stockLocate == 1234
    );

    REQUIRE(
        message.trackingNumber == 42
    );

    REQUIRE(
        message.timestamp == 1000
    );

    REQUIRE(
        message.orderReferenceNumber ==
        123456789ULL
    );

    REQUIRE(
        message.executedShares == 200
    );

    REQUIRE(
        message.matchNumber ==
        987654321ULL
    );
}


TEST_CASE(
    "ITCH decoder preserves 64 bit Order Executed match number"
)
{
    std::array<std::uint8_t, 31>
        data{};

    data[0] = 'E';

    data[23] = 0xFE;
    data[24] = 0xDC;
    data[25] = 0xBA;
    data[26] = 0x98;
    data[27] = 0x76;
    data[28] = 0x54;
    data[29] = 0x32;
    data[30] = 0x10;

    llt::itch::OrderExecutedMessage
        message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeOrderExecuted(
                data.data(),
                data.size(),
                message
            )
    );

    REQUIRE(
        message.matchNumber ==
        0xFEDCBA9876543210ULL
    );
}

TEST_CASE(
    "ITCH decoder preserves maximum Order Executed shares"
)
{
    std::array<std::uint8_t, 31>
        data{};

    data[0] = 'E';

    data[19] = 0xFF;
    data[20] = 0xFF;
    data[21] = 0xFF;
    data[22] = 0xFF;

    llt::itch::OrderExecutedMessage
        message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeOrderExecuted(
                data.data(),
                data.size(),
                message
            )
    );

    REQUIRE(
        message.executedShares ==
        0xFFFFFFFFU
    );
}

TEST_CASE(
    "ITCH decoder rejects truncated Order Executed message"
)
{
    const std::array<std::uint8_t, 30>
        data{
            'E'
        };

    llt::itch::OrderExecutedMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeOrderExecuted(
                data.data(),
                data.size(),
                message
            )
    );
}

TEST_CASE(
    "ITCH decoder rejects oversized Order Executed message"
)
{
    const std::array<std::uint8_t, 32>
        data{
            'E'
        };

    llt::itch::OrderExecutedMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeOrderExecuted(
                data.data(),
                data.size(),
                message
            )
    );
}

TEST_CASE(
    "ITCH decoder rejects incorrect Order Executed message type"
)
{
    const std::array<std::uint8_t, 31>
        data{
            'A'
        };

    llt::itch::OrderExecutedMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeOrderExecuted(
                data.data(),
                data.size(),
                message
            )
    );
}



TEST_CASE(
    "ITCH decoder decodes Order Executed With Price message"
)
{
    const std::array<std::uint8_t, 36> data{
        'C',

        // Stock Locate = 1234
        0x04,
        0xD2,

        // Tracking Number = 42
        0x00,
        0x2A,

        // Timestamp = 1000
        0x00,
        0x00,
        0x00,
        0x00,
        0x03,
        0xE8,

        // Order Reference Number = 123456789
        0x00,
        0x00,
        0x00,
        0x00,
        0x07,
        0x5B,
        0xCD,
        0x15,

        // Executed Shares = 200
        0x00,
        0x00,
        0x00,
        0xC8,

        // Match Number = 987654321
        0x00,
        0x00,
        0x00,
        0x00,
        0x3A,
        0xDE,
        0x68,
        0xB1,

        // Printable
        'Y',

        // Execution Price = 1,874,200
        // $187.4200
        0x00,
        0x1C,
        0x99,
        0x18
    };

    llt::itch::OrderExecutedWithPriceMessage
        message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeOrderExecutedWithPrice(
                data.data(),
                data.size(),
                message
            )
    );

    REQUIRE(
        message.stockLocate == 1234
    );

    REQUIRE(
        message.trackingNumber == 42
    );

    REQUIRE(
        message.timestamp == 1000
    );

    REQUIRE(
        message.orderReferenceNumber ==
        123456789ULL
    );

    REQUIRE(
        message.executedShares == 200
    );

    REQUIRE(
        message.matchNumber ==
        987654321ULL
    );

    REQUIRE(
        message.printable == 'Y'
    );

    REQUIRE(
        message.executionPrice ==
        1874200U
    );
}


TEST_CASE(
    "ITCH decoder decodes non printable Order Executed With Price message"
)
{
    std::array<std::uint8_t, 36>
        data{};

    data[0] = 'C';

    data[31] = 'N';

    llt::itch::OrderExecutedWithPriceMessage
        message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeOrderExecutedWithPrice(
                data.data(),
                data.size(),
                message
            )
    );

    REQUIRE(
        message.printable == 'N'
    );
}

TEST_CASE(
    "ITCH decoder preserves 64 bit Order Executed With Price fields"
)
{
    std::array<std::uint8_t, 36>
        data{};

    data[0] = 'C';

    // Order Reference Number
    data[11] = 0x01;
    data[12] = 0x23;
    data[13] = 0x45;
    data[14] = 0x67;
    data[15] = 0x89;
    data[16] = 0xAB;
    data[17] = 0xCD;
    data[18] = 0xEF;

    // Match Number
    data[23] = 0xFE;
    data[24] = 0xDC;
    data[25] = 0xBA;
    data[26] = 0x98;
    data[27] = 0x76;
    data[28] = 0x54;
    data[29] = 0x32;
    data[30] = 0x10;

    llt::itch::OrderExecutedWithPriceMessage
        message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeOrderExecutedWithPrice(
                data.data(),
                data.size(),
                message
            )
    );

    REQUIRE(
        message.orderReferenceNumber ==
        0x0123456789ABCDEFULL
    );

    REQUIRE(
        message.matchNumber ==
        0xFEDCBA9876543210ULL
    );
}

TEST_CASE(
    "ITCH decoder preserves maximum Order Executed With Price integer fields"
)
{
    std::array<std::uint8_t, 36>
        data{};

    data[0] = 'C';

    // Executed Shares
    data[19] = 0xFF;
    data[20] = 0xFF;
    data[21] = 0xFF;
    data[22] = 0xFF;

    // Execution Price
    data[32] = 0xFF;
    data[33] = 0xFF;
    data[34] = 0xFF;
    data[35] = 0xFF;

    llt::itch::OrderExecutedWithPriceMessage
        message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeOrderExecutedWithPrice(
                data.data(),
                data.size(),
                message
            )
    );

    REQUIRE(
        message.executedShares ==
        0xFFFFFFFFU
    );

    REQUIRE(
        message.executionPrice ==
        0xFFFFFFFFU
    );
}

TEST_CASE(
    "ITCH decoder rejects truncated Order Executed With Price message"
)
{
    const std::array<std::uint8_t, 35>
        data{
            'C'
        };

    llt::itch::OrderExecutedWithPriceMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeOrderExecutedWithPrice(
                data.data(),
                data.size(),
                message
            )
    );
}


TEST_CASE(
    "ITCH decoder rejects oversized Order Executed With Price message"
)
{
    const std::array<std::uint8_t, 37>
        data{
            'C'
        };

    llt::itch::OrderExecutedWithPriceMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeOrderExecutedWithPrice(
                data.data(),
                data.size(),
                message
            )
    );
}


TEST_CASE(
    "ITCH decoder rejects incorrect Order Executed With Price message type"
)
{
    const std::array<std::uint8_t, 36>
        data{
            'E'
        };

    llt::itch::OrderExecutedWithPriceMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeOrderExecutedWithPrice(
                data.data(),
                data.size(),
                message
            )
    );
}

TEST_CASE(
    "ITCH decoder decodes Order Cancel message"
)
{
    const std::array<std::uint8_t, 23> data{
        'X',

        // Stock Locate = 1234
        0x04,
        0xD2,

        // Tracking Number = 42
        0x00,
        0x2A,

        // Timestamp = 1000
        0x00,
        0x00,
        0x00,
        0x00,
        0x03,
        0xE8,

        // Order Reference Number = 123456789
        0x00,
        0x00,
        0x00,
        0x00,
        0x07,
        0x5B,
        0xCD,
        0x15,

        // Cancelled Shares = 150
        0x00,
        0x00,
        0x00,
        0x96
    };

    llt::itch::OrderCancelMessage
        message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeOrderCancel(
                data.data(),
                data.size(),
                message
            )
    );

    REQUIRE(
        message.stockLocate == 1234
    );

    REQUIRE(
        message.trackingNumber == 42
    );

    REQUIRE(
        message.timestamp == 1000
    );

    REQUIRE(
        message.orderReferenceNumber ==
        123456789ULL
    );

    REQUIRE(
        message.cancelledShares == 150
    );
}

TEST_CASE(
    "ITCH decoder preserves 64 bit Order Cancel reference number"
)
{
    std::array<std::uint8_t, 23>
        data{};

    data[0] = 'X';

    data[11] = 0xFE;
    data[12] = 0xDC;
    data[13] = 0xBA;
    data[14] = 0x98;
    data[15] = 0x76;
    data[16] = 0x54;
    data[17] = 0x32;
    data[18] = 0x10;

    llt::itch::OrderCancelMessage
        message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeOrderCancel(
                data.data(),
                data.size(),
                message
            )
    );

    REQUIRE(
        message.orderReferenceNumber ==
        0xFEDCBA9876543210ULL
    );
}
TEST_CASE(
    "ITCH decoder preserves maximum Order Cancel shares"
)
{
    std::array<std::uint8_t, 23>
        data{};

    data[0] = 'X';

    data[19] = 0xFF;
    data[20] = 0xFF;
    data[21] = 0xFF;
    data[22] = 0xFF;

    llt::itch::OrderCancelMessage
        message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeOrderCancel(
                data.data(),
                data.size(),
                message
            )
    );

    REQUIRE(
        message.cancelledShares ==
        0xFFFFFFFFU
    );
}

TEST_CASE(
    "ITCH decoder rejects null Order Cancel data"
)
{
    llt::itch::OrderCancelMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeOrderCancel(
                nullptr,
                llt::itch::ItchDecoder::
                    OrderCancelMessageSize,
                message
            )
    );
}

TEST_CASE(
    "ITCH decoder rejects truncated Order Cancel message"
)
{
    const std::array<std::uint8_t, 22>
        data{
            'X'
        };

    llt::itch::OrderCancelMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeOrderCancel(
                data.data(),
                data.size(),
                message
            )
    );
}

TEST_CASE(
    "ITCH decoder rejects oversized Order Cancel message"
)
{
    const std::array<std::uint8_t, 24>
        data{
            'X'
        };

    llt::itch::OrderCancelMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeOrderCancel(
                data.data(),
                data.size(),
                message
            )
    );
}

TEST_CASE(
    "ITCH decoder rejects incorrect Order Cancel message type"
)
{
    const std::array<std::uint8_t, 23>
        data{
            'A'
        };

    llt::itch::OrderCancelMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeOrderCancel(
                data.data(),
                data.size(),
                message
            )
    );
}


TEST_CASE(
    "ITCH decoder decodes Order Delete message"
)
{
    const std::array<std::uint8_t, 19> data{
        'D',

        // Stock Locate = 1234
        0x04,
        0xD2,

        // Tracking Number = 42
        0x00,
        0x2A,

        // Timestamp = 1000
        0x00,
        0x00,
        0x00,
        0x00,
        0x03,
        0xE8,

        // Order Reference Number = 123456789
        0x00,
        0x00,
        0x00,
        0x00,
        0x07,
        0x5B,
        0xCD,
        0x15
    };

    llt::itch::OrderDeleteMessage
        message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeOrderDelete(
                data.data(),
                data.size(),
                message
            )
    );

    REQUIRE(
        message.stockLocate == 1234
    );

    REQUIRE(
        message.trackingNumber == 42
    );

    REQUIRE(
        message.timestamp == 1000
    );

    REQUIRE(
        message.orderReferenceNumber ==
        123456789ULL
    );
}

TEST_CASE(
    "ITCH decoder preserves 64 bit Order Delete reference number"
)
{
    std::array<std::uint8_t, 19>
        data{};

    data[0] = 'D';

    data[11] = 0xFE;
    data[12] = 0xDC;
    data[13] = 0xBA;
    data[14] = 0x98;
    data[15] = 0x76;
    data[16] = 0x54;
    data[17] = 0x32;
    data[18] = 0x10;

    llt::itch::OrderDeleteMessage
        message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeOrderDelete(
                data.data(),
                data.size(),
                message
            )
    );

    REQUIRE(
        message.orderReferenceNumber ==
        0xFEDCBA9876543210ULL
    );
}

TEST_CASE(
    "ITCH decoder rejects null Order Delete data"
)
{
    llt::itch::OrderDeleteMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeOrderDelete(
                nullptr,
                llt::itch::ItchDecoder::
                    OrderDeleteMessageSize,
                message
            )
    );
}

TEST_CASE(
    "ITCH decoder rejects truncated Order Delete message"
)
{
    const std::array<std::uint8_t, 18>
        data{
            'D'
        };

    llt::itch::OrderDeleteMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeOrderDelete(
                data.data(),
                data.size(),
                message
            )
    );
}

TEST_CASE(
    "ITCH decoder rejects oversized Order Delete message"
)
{
    const std::array<std::uint8_t, 20>
        data{
            'D'
        };

    llt::itch::OrderDeleteMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeOrderDelete(
                data.data(),
                data.size(),
                message
            )
    );
}

TEST_CASE(
    "ITCH decoder rejects incorrect Order Delete message type"
)
{
    const std::array<std::uint8_t, 19>
        data{
            'X'
        };

    llt::itch::OrderDeleteMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeOrderDelete(
                data.data(),
                data.size(),
                message
            )
    );
}


TEST_CASE(
    "ITCH decoder decodes Order Replace message"
)
{
    const std::array<std::uint8_t, 35> data{
        'U',

        // Stock Locate = 1234
        0x04,
        0xD2,

        // Tracking Number = 42
        0x00,
        0x2A,

        // Timestamp = 1000
        0x00,
        0x00,
        0x00,
        0x00,
        0x03,
        0xE8,

        // Original Order Reference Number = 123456789
        0x00,
        0x00,
        0x00,
        0x00,
        0x07,
        0x5B,
        0xCD,
        0x15,

        // New Order Reference Number = 987654321
        0x00,
        0x00,
        0x00,
        0x00,
        0x3A,
        0xDE,
        0x68,
        0xB1,

        // New total displayed quantity = 300
        0x00,
        0x00,
        0x01,
        0x2C,

        // New Price = 1,875,000
        // Price(4) = $187.5000
        0x00,
        0x1C,
        0x9C,
        0x38
    };

    llt::itch::OrderReplaceMessage
        message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeOrderReplace(
                data.data(),
                data.size(),
                message
            )
    );

    REQUIRE(
        message.stockLocate == 1234
    );

    REQUIRE(
        message.trackingNumber == 42
    );

    REQUIRE(
        message.timestamp == 1000
    );

    REQUIRE(
        message.originalOrderReferenceNumber ==
        123456789ULL
    );

    REQUIRE(
        message.newOrderReferenceNumber ==
        987654321ULL
    );

    REQUIRE(
        message.shares == 300
    );

    REQUIRE(
        message.price == 1875000U
    );
}

TEST_CASE(
    "ITCH decoder preserves 64 bit Order Replace reference numbers"
)
{
    std::array<std::uint8_t, 35>
        data{};

    data[0] = 'U';

    // Original reference
    data[11] = 0x01;
    data[12] = 0x23;
    data[13] = 0x45;
    data[14] = 0x67;
    data[15] = 0x89;
    data[16] = 0xAB;
    data[17] = 0xCD;
    data[18] = 0xEF;

    // New reference
    data[19] = 0xFE;
    data[20] = 0xDC;
    data[21] = 0xBA;
    data[22] = 0x98;
    data[23] = 0x76;
    data[24] = 0x54;
    data[25] = 0x32;
    data[26] = 0x10;

    llt::itch::OrderReplaceMessage
        message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeOrderReplace(
                data.data(),
                data.size(),
                message
            )
    );

    REQUIRE(
        message.originalOrderReferenceNumber ==
        0x0123456789ABCDEFULL
    );

    REQUIRE(
        message.newOrderReferenceNumber ==
        0xFEDCBA9876543210ULL
    );
}

TEST_CASE(
    "ITCH decoder preserves maximum Order Replace shares and price"
)
{
    std::array<std::uint8_t, 35>
        data{};

    data[0] = 'U';

    data[27] = 0xFF;
    data[28] = 0xFF;
    data[29] = 0xFF;
    data[30] = 0xFF;

    data[31] = 0xFF;
    data[32] = 0xFF;
    data[33] = 0xFF;
    data[34] = 0xFF;

    llt::itch::OrderReplaceMessage
        message{};

    REQUIRE(
        llt::itch::ItchDecoder::
            decodeOrderReplace(
                data.data(),
                data.size(),
                message
            )
    );

    REQUIRE(
        message.shares ==
        0xFFFFFFFFU
    );

    REQUIRE(
        message.price ==
        0xFFFFFFFFU
    );
}

TEST_CASE(
    "ITCH decoder rejects null Order Replace data"
)
{
    llt::itch::OrderReplaceMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeOrderReplace(
                nullptr,
                llt::itch::ItchDecoder::
                    OrderReplaceMessageSize,
                message
            )
    );
}

TEST_CASE(
    "ITCH decoder rejects truncated Order Replace message"
)
{
    const std::array<std::uint8_t, 34>
        data{
            'U'
        };

    llt::itch::OrderReplaceMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeOrderReplace(
                data.data(),
                data.size(),
                message
            )
    );
}

TEST_CASE(
    "ITCH decoder rejects oversized Order Replace message"
)
{
    const std::array<std::uint8_t, 36>
        data{
            'U'
        };

    llt::itch::OrderReplaceMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeOrderReplace(
                data.data(),
                data.size(),
                message
            )
    );
}

TEST_CASE(
    "ITCH decoder rejects incorrect Order Replace message type"
)
{
    const std::array<std::uint8_t, 35>
        data{
            'D'
        };

    llt::itch::OrderReplaceMessage
        message{};

    REQUIRE_FALSE(
        llt::itch::ItchDecoder::
            decodeOrderReplace(
                data.data(),
                data.size(),
                message
            )
    );
}




