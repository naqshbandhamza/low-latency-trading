#include <cstdint>
#include <sstream>
#include <string>

#include <catch2/catch_test_macros.hpp>

#include "market_data/itch/ItchStreamReader.h"

TEST_CASE(
    "ITCH stream reader reads BinaryFILE message"
)
{
    const std::string bytes{
        '\x00',
        '\x0C',

        'S',
        '\x00',
        '\x00',
        '\x00',
        '\x00',
        '\x00',
        '\x00',
        '\x00',
        '\x00',
        '\x00',
        '\x00',
        'O'
    };

    std::istringstream stream(
        bytes,
        std::ios::binary
    );

    llt::itch::ItchStreamReader
        reader(stream);

    const auto result =
        reader.readNext();

    REQUIRE(
        result.status ==
        llt::itch::ItchStreamReadStatus::Message
    );

    REQUIRE(
        result.size == 12
    );

    REQUIRE(
        result.data[0] == 'S'
    );

    REQUIRE(
        result.data[11] == 'O'
    );
}

TEST_CASE(
    "ITCH stream reader reads multiple BinaryFILE messages"
)
{
    std::string bytes;

    // S — 12 bytes
    bytes.push_back('\x00');
    bytes.push_back('\x0C');

    bytes.push_back('S');

    for (int i = 0; i < 11; ++i)
    {
        bytes.push_back('\x00');
    }

    // D — 19 bytes
    bytes.push_back('\x00');
    bytes.push_back('\x13');

    bytes.push_back('D');

    for (int i = 0; i < 18; ++i)
    {
        bytes.push_back('\x00');
    }

    std::istringstream stream(
        bytes,
        std::ios::binary
    );

    llt::itch::ItchStreamReader
        reader(stream);

    const auto first =
        reader.readNext();

    REQUIRE(
        first.status ==
        llt::itch::ItchStreamReadStatus::Message
    );

    REQUIRE(
        first.size == 12
    );

    REQUIRE(
        first.data[0] == 'S'
    );

    // Important:
    // first.data points into the reader-owned reusable buffer.
    // Finish inspecting it before calling readNext() again.

    const auto second =
        reader.readNext();

    REQUIRE(
        second.status ==
        llt::itch::ItchStreamReadStatus::Message
    );

    REQUIRE(
        second.size == 19
    );

    REQUIRE(
        second.data[0] == 'D'
    );
}

TEST_CASE(
    "ITCH stream reader recognizes BinaryFILE end of session"
)
{
    const std::string bytes{
        '\x00',
        '\x00'
    };

    std::istringstream stream(
        bytes,
        std::ios::binary
    );

    llt::itch::ItchStreamReader
        reader(stream);

    const auto result =
        reader.readNext();

    REQUIRE(
        result.status ==
        llt::itch::ItchStreamReadStatus::
            EndOfSession
    );

    REQUIRE(
        result.empty()
    );
}

TEST_CASE(
    "ITCH stream reader reports empty BinaryFILE as incomplete"
)
{
    const std::string bytes{};

    std::istringstream stream(
        bytes,
        std::ios::binary
    );

    llt::itch::ItchStreamReader
        reader(stream);

    const auto result =
        reader.readNext();

    REQUIRE(
        result.status ==
        llt::itch::ItchStreamReadStatus::
            Incomplete
    );

    REQUIRE(
        result.empty()
    );
}

TEST_CASE(
    "ITCH stream reader rejects truncated BinaryFILE length prefix"
)
{
    const std::string bytes{
        '\x00'
    };

    std::istringstream stream(
        bytes,
        std::ios::binary
    );

    llt::itch::ItchStreamReader
        reader(stream);

    const auto result =
        reader.readNext();

    REQUIRE(
        result.status ==
        llt::itch::ItchStreamReadStatus::
            Incomplete
    );

    REQUIRE(
        result.empty()
    );
}

TEST_CASE(
    "ITCH stream reader rejects truncated BinaryFILE payload"
)
{
    std::string bytes;

    // Claims payload is 12 bytes.
    bytes.push_back('\x00');
    bytes.push_back('\x0C');

    // But provides only 5.
    bytes.push_back('S');
    bytes.push_back('\x00');
    bytes.push_back('\x00');
    bytes.push_back('\x00');
    bytes.push_back('\x00');

    std::istringstream stream(
        bytes,
        std::ios::binary
    );

    llt::itch::ItchStreamReader
        reader(stream);

    const auto result =
        reader.readNext();

    REQUIRE(
        result.status ==
        llt::itch::ItchStreamReadStatus::
            Incomplete
    );

    REQUIRE(
        result.empty()
    );
}

TEST_CASE(
    "ITCH stream reader reads message then end of session"
)
{
    std::string bytes;

    // One 12-byte System Event.
    bytes.push_back('\x00');
    bytes.push_back('\x0C');

    bytes.push_back('S');

    for (int i = 0; i < 11; ++i)
    {
        bytes.push_back('\x00');
    }

    // BinaryFILE end-of-session.
    bytes.push_back('\x00');
    bytes.push_back('\x00');

    std::istringstream stream(
        bytes,
        std::ios::binary
    );

    llt::itch::ItchStreamReader
        reader(stream);

    const auto message =
        reader.readNext();

    REQUIRE(
        message.status ==
        llt::itch::ItchStreamReadStatus::Message
    );

    REQUIRE(
        message.size == 12
    );

    REQUIRE(
        message.data[0] == 'S'
    );

    // Inspect message before the next read because the
    // underlying buffer belongs to ItchStreamReader.
    const auto end =
        reader.readNext();

    REQUIRE(
        end.status ==
        llt::itch::ItchStreamReadStatus::
            EndOfSession
    );

    REQUIRE(
        end.empty()
    );
}