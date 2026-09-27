#include <catch2/catch_test_macros.hpp>

#include <array>
#include <cstddef>
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

// ============================================================
// Big-endian writers
// ============================================================

void writeU16(
    std::uint8_t* data,
    std::uint16_t value
)
{
    data[0] =
        static_cast<std::uint8_t>(
            (value >> 8) & 0xFF
        );

    data[1] =
        static_cast<std::uint8_t>(
            value & 0xFF
        );
}

void writeU32(
    std::uint8_t* data,
    std::uint32_t value
)
{
    data[0] =
        static_cast<std::uint8_t>(
            (value >> 24) & 0xFF
        );

    data[1] =
        static_cast<std::uint8_t>(
            (value >> 16) & 0xFF
        );

    data[2] =
        static_cast<std::uint8_t>(
            (value >> 8) & 0xFF
        );

    data[3] =
        static_cast<std::uint8_t>(
            value & 0xFF
        );
}

void writeU48(
    std::uint8_t* data,
    std::uint64_t value
)
{
    for (std::size_t i = 0; i < 6; ++i)
    {
        data[i] =
            static_cast<std::uint8_t>(
                (value >> ((5 - i) * 8)) & 0xFF
            );
    }
}

void writeU64(
    std::uint8_t* data,
    std::uint64_t value
)
{
    for (std::size_t i = 0; i < 8; ++i)
    {
        data[i] =
            static_cast<std::uint8_t>(
                (value >> ((7 - i) * 8)) & 0xFF
            );
    }
}


// ============================================================
// BinaryFILE helper
// ============================================================

template <std::size_t N>
std::string makeBinaryFileRecord(
    const std::array<std::uint8_t, N>& payload
)
{
    std::string data;

    // BinaryFILE uses a two-byte big-endian
    // payload length.
    data.push_back(
        static_cast<char>(
            (N >> 8) & 0xFF
        )
    );

    data.push_back(
        static_cast<char>(
            N & 0xFF
        )
    );

    for (const auto byte : payload)
    {
        data.push_back(
            static_cast<char>(byte)
        );
    }

    // End-of-session marker used by our replay tests.
    data.push_back('\x00');
    data.push_back('\x00');

    return data;
}


// ============================================================
// Y - Reg SHO Restriction
// ============================================================

std::array<std::uint8_t, 20>
makeValidRegShoRestriction()
{
    std::array<std::uint8_t, 20> data{};

    data[0] = 'Y';

    writeU16(data.data() + 1, 0x1234);
    writeU16(data.data() + 3, 0x5678);

    writeU48(
        data.data() + 5,
        0x010203040506ULL
    );

    const std::array<char, 8> stock{
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' '
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

    data[19] = '1';

    return data;
}


// ============================================================
// J - MWCB Decline Level
// ============================================================

std::array<std::uint8_t, 35>
makeValidMwcbDeclineLevel()
{
    std::array<std::uint8_t, 35> data{};

    //data[0] = 'J';
    data[0] = 'V';

    writeU16(data.data() + 1, 0x1234);
    writeU16(data.data() + 3, 0x5678);

    writeU48(
        data.data() + 5,
        0x010203040506ULL
    );

    writeU64(
        data.data() + 11,
        100000000ULL
    );

    writeU64(
        data.data() + 19,
        200000000ULL
    );

    writeU64(
        data.data() + 27,
        300000000ULL
    );

    return data;
}


// ============================================================
// V - MWCB Status
// ============================================================

std::array<std::uint8_t, 12>
makeValidMwcbStatus()
{
    std::array<std::uint8_t, 12> data{};

    //data[0] = 'V';
    data[0] = 'W';

    writeU16(data.data() + 1, 0x1234);
    writeU16(data.data() + 3, 0x5678);

    writeU48(
        data.data() + 5,
        0x010203040506ULL
    );

    data[11] = '1';

    return data;
}


// ============================================================
// I - NOII
// ============================================================

std::array<std::uint8_t, 50>
makeValidNoii()
{
    std::array<std::uint8_t, 50> data{};

    data[0] = 'I';

    writeU16(data.data() + 1, 0x1234);
    writeU16(data.data() + 3, 0x5678);

    writeU48(
        data.data() + 5,
        0x010203040506ULL
    );

    writeU64(
        data.data() + 11,
        1000ULL
    );

    writeU64(
        data.data() + 19,
        500ULL
    );

    data[27] = 'B';

    const std::array<char, 8> stock{
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' '
    };

    for (std::size_t i = 0;
         i < stock.size();
         ++i)
    {
        data[28 + i] =
            static_cast<std::uint8_t>(
                stock[i]
            );
    }

    writeU32(
        data.data() + 36,
        1870000
    );

    writeU32(
        data.data() + 40,
        1871000
    );

    writeU32(
        data.data() + 44,
        1870500
    );

    data[48] = 'O';
    data[49] = 'A';

    return data;
}

} // namespace


// ============================================================
// Y TESTS
// ============================================================

TEST_CASE(
    "ItchDecoder decodes Reg SHO Restriction message"
)
{
    const auto data =
        makeValidRegShoRestriction();

    RegShoRestrictionMessage message;

    REQUIRE(
        ItchDecoder::decodeRegShoRestriction(
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
    REQUIRE(message.regShoAction == '1');
}


TEST_CASE(
    "Reg SHO Restriction preserves eight character stock"
)
{
    auto data =
        makeValidRegShoRestriction();

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

    RegShoRestrictionMessage message;

    REQUIRE(
        ItchDecoder::decodeRegShoRestriction(
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
    "Reg SHO Restriction preserves action"
)
{
    auto data =
        makeValidRegShoRestriction();

    data[19] = '2';

    RegShoRestrictionMessage message;

    REQUIRE(
        ItchDecoder::decodeRegShoRestriction(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(message.regShoAction == '2');
}


TEST_CASE(
    "ItchDecoder rejects malformed Reg SHO Restriction messages"
)
{
    const auto data =
        makeValidRegShoRestriction();

    RegShoRestrictionMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeRegShoRestriction(
            nullptr,
            data.size(),
            message
        )
    );

    REQUIRE_FALSE(
        ItchDecoder::decodeRegShoRestriction(
            data.data(),
            data.size() - 1,
            message
        )
    );

    std::array<std::uint8_t, 21> oversized{};
    oversized[0] = 'Y';

    REQUIRE_FALSE(
        ItchDecoder::decodeRegShoRestriction(
            oversized.data(),
            oversized.size(),
            message
        )
    );

    auto wrongType = data;
    wrongType[0] = 'A';

    REQUIRE_FALSE(
        ItchDecoder::decodeRegShoRestriction(
            wrongType.data(),
            wrongType.size(),
            message
        )
    );
}


TEST_CASE(
    "ItchDispatcher routes Reg SHO Restriction"
)
{
    const auto data =
        makeValidRegShoRestriction();

    const auto result =
        ItchDispatcher::dispatch(
            data.data(),
            data.size()
        );

    REQUIRE(result.has_value());

    REQUIRE(
        std::holds_alternative<
            RegShoRestrictionMessage
        >(*result)
    );
}


TEST_CASE(
    "ItchReplay counts Reg SHO Restriction"
)
{
    const auto payload =
        makeValidRegShoRestriction();

    const auto binary =
        makeBinaryFileRecord(payload);

    std::istringstream stream(binary);

    const auto result =
        ItchReplay::run(stream);

    REQUIRE(
        result.status ==
        ItchReplayStatus::Complete
    );

    REQUIRE(result.stats.recordsRead == 1);
    REQUIRE(result.stats.decodedMessages == 1);

    REQUIRE(
        result.stats.regShoRestrictions == 1
    );

    REQUIRE(result.stats.unsupportedMessages == 0);
    REQUIRE(result.stats.malformedMessages == 0);

    REQUIRE(
        result.stats.unsupportedByType[
            static_cast<std::uint8_t>('Y')
        ] == 0
    );
}


// ============================================================
// J TESTS
// ============================================================

TEST_CASE(
    "ItchDecoder decodes MWCB Decline Level message"
)
{
    const auto data =
        makeValidMwcbDeclineLevel();

    MwcbDeclineLevelMessage message;

    REQUIRE(
        ItchDecoder::decodeMwcbDeclineLevel(
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

    REQUIRE(message.level1 == 100000000ULL);
    REQUIRE(message.level2 == 200000000ULL);
    REQUIRE(message.level3 == 300000000ULL);
}


TEST_CASE(
    "MWCB Decline Level supports maximum 64 bit values"
)
{
    auto data =
        makeValidMwcbDeclineLevel();

    writeU64(
        data.data() + 11,
        UINT64_MAX
    );

    writeU64(
        data.data() + 19,
        UINT64_MAX
    );

    writeU64(
        data.data() + 27,
        UINT64_MAX
    );

    MwcbDeclineLevelMessage message;

    REQUIRE(
        ItchDecoder::decodeMwcbDeclineLevel(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(message.level1 == UINT64_MAX);
    REQUIRE(message.level2 == UINT64_MAX);
    REQUIRE(message.level3 == UINT64_MAX);
}


// TEST_CASE(
//     "ItchDecoder rejects malformed MWCB Decline Level messages"
// )
// {
//     const auto data =
//         makeValidMwcbDeclineLevel();

//     MwcbDeclineLevelMessage message;

//     REQUIRE_FALSE(
//         ItchDecoder::decodeMwcbDeclineLevel(
//             nullptr,
//             data.size(),
//             message
//         )
//     );

//     REQUIRE_FALSE(
//         ItchDecoder::decodeMwcbDeclineLevel(
//             data.data(),
//             data.size() - 1,
//             message
//         )
//     );

//     std::array<std::uint8_t, 36> oversized{};
//     oversized[0] = 'J';

//     REQUIRE_FALSE(
//         ItchDecoder::decodeMwcbDeclineLevel(
//             oversized.data(),
//             oversized.size(),
//             message
//         )
//     );

//     auto wrongType = data;
//     wrongType[0] = 'V';

//     REQUIRE_FALSE(
//         ItchDecoder::decodeMwcbDeclineLevel(
//             wrongType.data(),
//             wrongType.size(),
//             message
//         )
//     );
// }


TEST_CASE(
    "ItchDecoder rejects malformed MWCB Decline Level messages"
)
{
    const auto data =
        makeValidMwcbDeclineLevel();

    MwcbDeclineLevelMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeMwcbDeclineLevel(
            nullptr,
            data.size(),
            message
        )
    );

    REQUIRE_FALSE(
        ItchDecoder::decodeMwcbDeclineLevel(
            data.data(),
            data.size() - 1,
            message
        )
    );

    std::array<std::uint8_t, 36> oversized{};
    oversized[0] = 'V';

    REQUIRE_FALSE(
        ItchDecoder::decodeMwcbDeclineLevel(
            oversized.data(),
            oversized.size(),
            message
        )
    );

    auto wrongType = data;
    wrongType[0] = '?';

    REQUIRE_FALSE(
        ItchDecoder::decodeMwcbDeclineLevel(
            wrongType.data(),
            wrongType.size(),
            message
        )
    );
}

TEST_CASE(
    "ItchDispatcher routes MWCB Decline Level"
)
{
    const auto data =
        makeValidMwcbDeclineLevel();

    const auto result =
        ItchDispatcher::dispatch(
            data.data(),
            data.size()
        );

    REQUIRE(result.has_value());

    REQUIRE(
        std::holds_alternative<
            MwcbDeclineLevelMessage
        >(*result)
    );
}


TEST_CASE(
    "ItchReplay counts MWCB Decline Level"
)
{
    const auto payload =
        makeValidMwcbDeclineLevel();

    const auto binary =
        makeBinaryFileRecord(payload);

    std::istringstream stream(binary);

    const auto result =
        ItchReplay::run(stream);

    REQUIRE(
        result.status ==
        ItchReplayStatus::Complete
    );

    REQUIRE(result.stats.recordsRead == 1);
    REQUIRE(result.stats.decodedMessages == 1);

    REQUIRE(
        result.stats.mwcbDeclineLevels == 1
    );

    REQUIRE(result.stats.unsupportedMessages == 0);
    REQUIRE(result.stats.malformedMessages == 0);
}


// ============================================================
// V TESTS
// ============================================================

TEST_CASE(
    "ItchDecoder decodes MWCB Status message"
)
{
    const auto data =
        makeValidMwcbStatus();

    MwcbStatusMessage message;

    REQUIRE(
        ItchDecoder::decodeMwcbStatus(
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

    REQUIRE(message.breachedLevel == '1');
}


TEST_CASE(
    "MWCB Status preserves breached level"
)
{
    auto data =
        makeValidMwcbStatus();

    data[11] = '3';

    MwcbStatusMessage message;

    REQUIRE(
        ItchDecoder::decodeMwcbStatus(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(message.breachedLevel == '3');
}


TEST_CASE(
    "ItchDecoder rejects malformed MWCB Status messages"
)
{
    const auto data =
        makeValidMwcbStatus();

    MwcbStatusMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeMwcbStatus(
            nullptr,
            data.size(),
            message
        )
    );

    REQUIRE_FALSE(
        ItchDecoder::decodeMwcbStatus(
            data.data(),
            data.size() - 1,
            message
        )
    );

    std::array<std::uint8_t, 13> oversized{};
    oversized[0] = 'V';

    REQUIRE_FALSE(
        ItchDecoder::decodeMwcbStatus(
            oversized.data(),
            oversized.size(),
            message
        )
    );

    auto wrongType = data;
    wrongType[0] = 'J';

    REQUIRE_FALSE(
        ItchDecoder::decodeMwcbStatus(
            wrongType.data(),
            wrongType.size(),
            message
        )
    );
}


TEST_CASE(
    "ItchDispatcher routes MWCB Status"
)
{
    const auto data =
        makeValidMwcbStatus();

    const auto result =
        ItchDispatcher::dispatch(
            data.data(),
            data.size()
        );

    REQUIRE(result.has_value());

    REQUIRE(
        std::holds_alternative<
            MwcbStatusMessage
        >(*result)
    );
}


TEST_CASE(
    "ItchReplay counts MWCB Status"
)
{
    const auto payload =
        makeValidMwcbStatus();

    const auto binary =
        makeBinaryFileRecord(payload);

    std::istringstream stream(binary);

    const auto result =
        ItchReplay::run(stream);

    REQUIRE(
        result.status ==
        ItchReplayStatus::Complete
    );

    REQUIRE(result.stats.recordsRead == 1);
    REQUIRE(result.stats.decodedMessages == 1);

    REQUIRE(
        result.stats.mwcbStatuses == 1
    );

    REQUIRE(result.stats.unsupportedMessages == 0);
    REQUIRE(result.stats.malformedMessages == 0);
}


// ============================================================
// I / NOII TESTS
// ============================================================

TEST_CASE(
    "ItchDecoder decodes NOII message"
)
{
    const auto data =
        makeValidNoii();

    NoiiMessage message;

    REQUIRE(
        ItchDecoder::decodeNoii(
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

    REQUIRE(message.pairedShares == 1000ULL);
    REQUIRE(message.imbalanceShares == 500ULL);

    REQUIRE(message.imbalanceDirection == 'B');
    REQUIRE(message.stockView() == "AAPL");

    REQUIRE(message.farPrice == 1870000);
    REQUIRE(message.nearPrice == 1871000);

    REQUIRE(
        message.currentReferencePrice ==
        1870500
    );

    REQUIRE(message.crossType == 'O');

    REQUIRE(
        message.priceVariationIndicator ==
        'A'
    );
}


TEST_CASE(
    "NOII supports maximum 64 bit share quantities"
)
{
    auto data =
        makeValidNoii();

    writeU64(
        data.data() + 11,
        UINT64_MAX
    );

    writeU64(
        data.data() + 19,
        UINT64_MAX
    );

    NoiiMessage message;

    REQUIRE(
        ItchDecoder::decodeNoii(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(
        message.pairedShares ==
        UINT64_MAX
    );

    REQUIRE(
        message.imbalanceShares ==
        UINT64_MAX
    );
}


TEST_CASE(
    "NOII supports maximum price values"
)
{
    auto data =
        makeValidNoii();

    writeU32(
        data.data() + 36,
        UINT32_MAX
    );

    writeU32(
        data.data() + 40,
        UINT32_MAX
    );

    writeU32(
        data.data() + 44,
        UINT32_MAX
    );

    NoiiMessage message;

    REQUIRE(
        ItchDecoder::decodeNoii(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(
        message.farPrice ==
        UINT32_MAX
    );

    REQUIRE(
        message.nearPrice ==
        UINT32_MAX
    );

    REQUIRE(
        message.currentReferencePrice ==
        UINT32_MAX
    );
}


TEST_CASE(
    "NOII preserves eight character stock"
)
{
    auto data =
        makeValidNoii();

    const std::array<char, 8> stock{
        'A', 'B', 'C', 'D',
        'E', 'F', 'G', 'H'
    };

    for (std::size_t i = 0;
         i < stock.size();
         ++i)
    {
        data[28 + i] =
            static_cast<std::uint8_t>(
                stock[i]
            );
    }

    NoiiMessage message;

    REQUIRE(
        ItchDecoder::decodeNoii(
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
    "NOII preserves imbalance and auction indicators"
)
{
    auto data =
        makeValidNoii();

    data[27] = 'S';
    data[48] = 'C';
    data[49] = 'B';

    NoiiMessage message;

    REQUIRE(
        ItchDecoder::decodeNoii(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(
        message.imbalanceDirection ==
        'S'
    );

    REQUIRE(message.crossType == 'C');

    REQUIRE(
        message.priceVariationIndicator ==
        'B'
    );
}


TEST_CASE(
    "ItchDecoder rejects malformed NOII messages"
)
{
    const auto data =
        makeValidNoii();

    NoiiMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeNoii(
            nullptr,
            data.size(),
            message
        )
    );

    REQUIRE_FALSE(
        ItchDecoder::decodeNoii(
            data.data(),
            data.size() - 1,
            message
        )
    );

    std::array<std::uint8_t, 51> oversized{};
    oversized[0] = 'I';

    REQUIRE_FALSE(
        ItchDecoder::decodeNoii(
            oversized.data(),
            oversized.size(),
            message
        )
    );

    auto wrongType = data;
    wrongType[0] = 'Q';

    REQUIRE_FALSE(
        ItchDecoder::decodeNoii(
            wrongType.data(),
            wrongType.size(),
            message
        )
    );
}


TEST_CASE(
    "ItchDispatcher routes NOII"
)
{
    const auto data =
        makeValidNoii();

    const auto result =
        ItchDispatcher::dispatch(
            data.data(),
            data.size()
        );

    REQUIRE(result.has_value());

    REQUIRE(
        std::holds_alternative<
            NoiiMessage
        >(*result)
    );

    const auto& message =
        std::get<NoiiMessage>(*result);

    REQUIRE(message.stockView() == "AAPL");

    REQUIRE(
        message.pairedShares ==
        1000ULL
    );
}


TEST_CASE(
    "ItchReplay counts NOII as supported"
)
{
    const auto payload =
        makeValidNoii();

    const auto binary =
        makeBinaryFileRecord(payload);

    std::istringstream stream(binary);

    const auto result =
        ItchReplay::run(stream);

    REQUIRE(
        result.status ==
        ItchReplayStatus::Complete
    );

    REQUIRE(result.stats.recordsRead == 1);
    REQUIRE(result.stats.decodedMessages == 1);

    REQUIRE(
        result.stats.noiiMessages == 1
    );

    REQUIRE(result.stats.unsupportedMessages == 0);
    REQUIRE(result.stats.malformedMessages == 0);

    REQUIRE(
        result.stats.unsupportedByType[
            static_cast<std::uint8_t>('I')
        ] == 0
    );
}



std::array<std::uint8_t, 35>
makeValidLuldAuctionCollar()
{
    std::array<std::uint8_t, 35> data{};

    data[0] = 'J';

    writeU16(
        data.data() + 1,
        0x1234
    );

    writeU16(
        data.data() + 3,
        0x5678
    );

    writeU48(
        data.data() + 5,
        0x010203040506ULL
    );

    const std::array<char, 8> stock{
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' '
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

    writeU32(data.data() + 19, 1870000);
    writeU32(data.data() + 23, 1900000);
    writeU32(data.data() + 27, 1840000);
    writeU32(data.data() + 31, 2);

    return data;
}

TEST_CASE(
    "ItchDecoder decodes LULD Auction Collar"
)
{
    const auto data =
        makeValidLuldAuctionCollar();

    LuldAuctionCollarMessage message;

    REQUIRE(
        ItchDecoder::decodeLuldAuctionCollar(
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

    REQUIRE(
        message.auctionCollarReferencePrice ==
        1870000
    );

    REQUIRE(
        message.upperAuctionCollarPrice ==
        1900000
    );

    REQUIRE(
        message.lowerAuctionCollarPrice ==
        1840000
    );

    REQUIRE(
        message.auctionCollarExtension == 2
    );
}


TEST_CASE(
    "LULD Auction Collar supports maximum values"
)
{
    auto data =
        makeValidLuldAuctionCollar();

    writeU32(data.data() + 19, UINT32_MAX);
    writeU32(data.data() + 23, UINT32_MAX);
    writeU32(data.data() + 27, UINT32_MAX);
    writeU32(data.data() + 31, UINT32_MAX);

    LuldAuctionCollarMessage message;

    REQUIRE(
        ItchDecoder::decodeLuldAuctionCollar(
            data.data(),
            data.size(),
            message
        )
    );

    REQUIRE(
        message.auctionCollarReferencePrice ==
        UINT32_MAX
    );

    REQUIRE(
        message.upperAuctionCollarPrice ==
        UINT32_MAX
    );

    REQUIRE(
        message.lowerAuctionCollarPrice ==
        UINT32_MAX
    );

    REQUIRE(
        message.auctionCollarExtension ==
        UINT32_MAX
    );
}


TEST_CASE(
    "ItchDecoder rejects malformed LULD Auction Collar"
)
{
    const auto data =
        makeValidLuldAuctionCollar();

    LuldAuctionCollarMessage message;

    REQUIRE_FALSE(
        ItchDecoder::decodeLuldAuctionCollar(
            nullptr,
            data.size(),
            message
        )
    );

    REQUIRE_FALSE(
        ItchDecoder::decodeLuldAuctionCollar(
            data.data(),
            data.size() - 1,
            message
        )
    );

    std::array<std::uint8_t, 36> oversized{};
    oversized[0] = 'J';

    REQUIRE_FALSE(
        ItchDecoder::decodeLuldAuctionCollar(
            oversized.data(),
            oversized.size(),
            message
        )
    );

    auto wrongType = data;
    wrongType[0] = '?';

    REQUIRE_FALSE(
        ItchDecoder::decodeLuldAuctionCollar(
            wrongType.data(),
            wrongType.size(),
            message
        )
    );
}


TEST_CASE(
    "ItchDispatcher routes LULD Auction Collar"
)
{
    const auto data =
        makeValidLuldAuctionCollar();

    const auto result =
        ItchDispatcher::dispatch(
            data.data(),
            data.size()
        );

    REQUIRE(result.has_value());

    REQUIRE(
        std::holds_alternative<
            LuldAuctionCollarMessage
        >(*result)
    );
}


TEST_CASE(
    "ItchReplay counts LULD Auction Collar"
)
{
    const auto payload =
        makeValidLuldAuctionCollar();

    const auto binary =
        makeBinaryFileRecord(payload);

    std::istringstream stream(binary);

    const auto result =
        ItchReplay::run(stream);

    REQUIRE(
        result.status ==
        ItchReplayStatus::Complete
    );

    REQUIRE(result.stats.recordsRead == 1);
    REQUIRE(result.stats.decodedMessages == 1);

    REQUIRE(
        result.stats.luldAuctionCollars == 1
    );

    REQUIRE(
        result.stats.unsupportedMessages == 0
    );

    REQUIRE(
        result.stats.malformedMessages == 0
    );

    REQUIRE(
        result.stats.unsupportedByType[
            static_cast<std::uint8_t>('J')
        ] == 0
    );
}