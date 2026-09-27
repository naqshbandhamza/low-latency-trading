#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "market_data/itch/ItchReplay.h"
#include "market_data/itch/ItchMarketState.h"

namespace
{

    void appendRecord(
        std::string &stream,
        const std::vector<std::uint8_t> &payload)
    {
        const auto size =
            static_cast<std::uint16_t>(
                payload.size());

        stream.push_back(
            static_cast<char>(
                (size >> 8) & 0xFF));

        stream.push_back(
            static_cast<char>(
                size & 0xFF));

        for (const auto byte : payload)
        {
            stream.push_back(
                static_cast<char>(byte));
        }
    }

    void appendEndOfSession(
        std::string &stream)
    {
        stream.push_back('\x00');
        stream.push_back('\x00');
    }

    std::vector<std::uint8_t>
    makeValidStockDirectory()
    {
        std::vector<std::uint8_t> data(
            39,
            0);

        data[0] = 'R';

        // Stock Locate = 42
        data[1] = 0x00;
        data[2] = 0x2A;

        // Tracking Number = 1
        data[3] = 0x00;
        data[4] = 0x01;

        // Timestamp = 1
        data[10] = 0x01;

        // Stock = "AAPL    "
        data[11] = 'A';
        data[12] = 'A';
        data[13] = 'P';
        data[14] = 'L';
        data[15] = ' ';
        data[16] = ' ';
        data[17] = ' ';
        data[18] = ' ';

        return data;
    }

    

} // namespace

TEST_CASE(
    "ITCH replay processes complete BinaryFILE session")
{
    std::string bytes;

    std::vector<std::uint8_t>
        systemEvent(12);

    systemEvent[0] = 'S';
    systemEvent[11] = 'O';

    appendRecord(
        bytes,
        systemEvent);

    std::vector<std::uint8_t>
        addOrder(36);

    addOrder[0] = 'A';
    addOrder[19] = 'B';

    appendRecord(
        bytes,
        addOrder);

    appendEndOfSession(bytes);

    std::istringstream stream(
        bytes,
        std::ios::binary);

    const auto result =
        llt::itch::ItchReplay::run(
            stream);

    REQUIRE(
        result.status ==
        llt::itch::ItchReplayStatus::Complete);

    REQUIRE(result.stats.sessionComplete);

    REQUIRE(
        result.stats.recordsRead == 2);

    REQUIRE(
        result.stats.decodedMessages == 2);

    REQUIRE(
        result.stats.systemEvents == 1);

    REQUIRE(
        result.stats.addOrders == 1);

    REQUIRE(
        result.stats.unsupportedMessages == 0);

    REQUIRE(
        result.stats.malformedMessages == 0);
}

TEST_CASE("ITCH replay skips unsupported message types")
{
    std::string data;

    // Deliberately unknown message type.
    // Do not use a legitimate ITCH type here because
    // we may support it later.
    data.push_back('\x00');
    data.push_back('\x01');
    data.push_back('?');

    data.push_back('\x00');
    data.push_back('\x00');

    std::istringstream stream(data);

    const auto result =
        llt::itch::ItchReplay::run(stream);

    REQUIRE(
        result.status ==
        llt::itch::ItchReplayStatus::Complete);

    REQUIRE(result.stats.recordsRead == 1);
    REQUIRE(result.stats.decodedMessages == 0);
    REQUIRE(result.stats.unsupportedMessages == 1);
    REQUIRE(result.stats.malformedMessages == 0);

    REQUIRE(
        result.stats.unsupportedByType[static_cast<std::uint8_t>('?')] == 1);
}

TEST_CASE(
    "ITCH replay counts malformed supported message")
{
    std::string bytes;

    // A must be 36 bytes.
    std::vector<std::uint8_t>
        malformedAddOrder(35);

    malformedAddOrder[0] = 'A';

    appendRecord(
        bytes,
        malformedAddOrder);

    appendEndOfSession(bytes);

    std::istringstream stream(
        bytes,
        std::ios::binary);

    const auto result =
        llt::itch::ItchReplay::run(
            stream);

    REQUIRE(
        result.status ==
        llt::itch::ItchReplayStatus::Complete);

    REQUIRE(
        result.stats.recordsRead == 1);

    REQUIRE(
        result.stats.malformedMessages == 1);

    REQUIRE(
        result.stats.unsupportedMessages == 0);

    REQUIRE(
        result.stats.decodedMessages == 0);
}

TEST_CASE(
    "ITCH replay reports incomplete BinaryFILE")
{
    std::string bytes;

    std::vector<std::uint8_t>
        systemEvent(12);

    systemEvent[0] = 'S';

    appendRecord(
        bytes,
        systemEvent);

    // Intentionally no 00 00 end marker.

    std::istringstream stream(
        bytes,
        std::ios::binary);

    const auto result =
        llt::itch::ItchReplay::run(
            stream);

    REQUIRE(
        result.status ==
        llt::itch::ItchReplayStatus::
            IncompleteStream);

    REQUIRE_FALSE(
        result.stats.sessionComplete);

    REQUIRE(
        result.stats.recordsRead == 1);

    REQUIRE(
        result.stats.decodedMessages == 1);
}

TEST_CASE(
    "ITCH replay counts every supported message type")
{
    std::string bytes;

    std::vector<std::uint8_t> s(12);
    s[0] = 'S';

    std::vector<std::uint8_t> r(39);
    r[0] = 'R';

    std::vector<std::uint8_t> a(36);
    a[0] = 'A';

    std::vector<std::uint8_t> e(31);
    e[0] = 'E';

    std::vector<std::uint8_t> c(36);
    c[0] = 'C';

    std::vector<std::uint8_t> x(23);
    x[0] = 'X';

    std::vector<std::uint8_t> d(19);
    d[0] = 'D';

    std::vector<std::uint8_t> u(35);
    u[0] = 'U';

    appendRecord(bytes, s);
    appendRecord(bytes, r);
    appendRecord(bytes, a);
    appendRecord(bytes, e);
    appendRecord(bytes, c);
    appendRecord(bytes, x);
    appendRecord(bytes, d);
    appendRecord(bytes, u);

    appendEndOfSession(bytes);

    std::istringstream stream(
        bytes,
        std::ios::binary);

    const auto result =
        llt::itch::ItchReplay::run(
            stream);

    REQUIRE(
        result.status ==
        llt::itch::ItchReplayStatus::Complete);

    REQUIRE(
        result.stats.recordsRead == 8);

    REQUIRE(
        result.stats.decodedMessages == 8);

    REQUIRE(
        result.stats.systemEvents == 1);

    REQUIRE(
        result.stats.stockDirectories == 1);

    REQUIRE(
        result.stats.addOrders == 1);

    REQUIRE(
        result.stats.orderExecutions == 1);

    REQUIRE(
        result.stats.orderExecutionsWithPrice == 1);

    REQUIRE(
        result.stats.orderCancels == 1);

    REQUIRE(
        result.stats.orderDeletes == 1);

    REQUIRE(
        result.stats.orderReplaces == 1);
}

TEST_CASE("ItchReplay counts unsupported messages by type")
{
    std::string data;

    // Unknown type '?'
    data.push_back('\x00');
    data.push_back('\x01');
    data.push_back('?');

    // Another unknown type '?'
    data.push_back('\x00');
    data.push_back('\x01');
    data.push_back('?');

    // Different unknown type '#'
    data.push_back('\x00');
    data.push_back('\x01');
    data.push_back('#');

    // End-of-session marker
    data.push_back('\x00');
    data.push_back('\x00');

    std::istringstream stream(data);

    const auto result =
        llt::itch::ItchReplay::run(stream);

    REQUIRE(
        result.status ==
        llt::itch::ItchReplayStatus::Complete);

    REQUIRE(result.stats.recordsRead == 3);
    REQUIRE(result.stats.decodedMessages == 0);
    REQUIRE(result.stats.unsupportedMessages == 3);
    REQUIRE(result.stats.malformedMessages == 0);

    REQUIRE(
        result.stats.unsupportedByType[static_cast<std::uint8_t>('?')] == 2);

    REQUIRE(
        result.stats.unsupportedByType[static_cast<std::uint8_t>('#')] == 1);

    // Known supported message types must not
    // appear in unsupported counters.
    REQUIRE(
        result.stats.unsupportedByType[static_cast<std::uint8_t>('P')] == 0);

    REQUIRE(
        result.stats.unsupportedByType[static_cast<std::uint8_t>('Q')] == 0);
}

TEST_CASE(
    "ITCH replay delivers decoded messages to handler")
{
    std::string data;

    const auto payload =
        makeValidStockDirectory();

    appendRecord(
        data,
        payload);

    appendEndOfSession(data);

    std::istringstream stream(
        data,
        std::ios::binary);

    std::size_t delivered = 0;

    const auto result =
        llt::itch::ItchReplay::run(
            stream,
            [&delivered](
                const llt::itch::ItchMessage &)
            {
                ++delivered;
            });

    REQUIRE(
        result.status ==
        llt::itch::ItchReplayStatus::Complete);

    REQUIRE(
        result.stats.decodedMessages == 1);

    REQUIRE(delivered == 1);
}

TEST_CASE(
    "ITCH replay can populate market state")
{
    std::string data;

    const auto payload =
        makeValidStockDirectory();

    appendRecord(
        data,
        payload);

    appendEndOfSession(data);

    std::istringstream stream(
        data,
        std::ios::binary);

    llt::itch::ItchMarketState state;

    const auto result =
        llt::itch::ItchReplay::run(
            stream,
            [&state](
                const llt::itch::ItchMessage &message)
            {
                state.onMessage(message);
            });

    REQUIRE(
        result.status ==
        llt::itch::ItchReplayStatus::Complete);

    REQUIRE(
        result.stats.decodedMessages == 1);

    REQUIRE(
        state.instruments().size() == 1);

    const auto *instrument =
        state.instruments().find(42);

    REQUIRE(instrument != nullptr);

    REQUIRE(
        instrument->symbolView() ==
        "AAPL");
}

TEST_CASE(
    "ITCH replay does not deliver unsupported messages to handler")
{
    std::string data;

    data.push_back('\x00');
    data.push_back('\x01');
    data.push_back('?');

    appendEndOfSession(data);

    std::istringstream stream(
        data,
        std::ios::binary);

    std::size_t delivered = 0;

    const auto result =
        llt::itch::ItchReplay::run(
            stream,
            [&delivered](
                const llt::itch::ItchMessage &)
            {
                ++delivered;
            });

    REQUIRE(
        result.status ==
        llt::itch::ItchReplayStatus::Complete);

    REQUIRE(
        result.stats.unsupportedMessages == 1);

    REQUIRE(
        result.stats.decodedMessages == 0);

    REQUIRE(delivered == 0);
}