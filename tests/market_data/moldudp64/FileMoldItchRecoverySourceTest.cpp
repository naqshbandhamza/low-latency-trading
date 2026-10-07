#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "market_data/moldudp64/FileMoldItchRecoverySource.h"
#include "market_data/moldudp64/SequencedItchMessage.h"

namespace
{

//
// Write one BinaryFILE record:
//
//     2-byte big-endian payload length
//     N-byte ITCH payload
//
void writeRecord(
    std::ofstream& file,
    const std::vector<std::uint8_t>& payload)
{
    const auto size =
        static_cast<std::uint16_t>(
            payload.size());

    const std::uint8_t lengthBytes[2] = {
        static_cast<std::uint8_t>(
            (size >> 8) & 0xFF),
        static_cast<std::uint8_t>(
            size & 0xFF)
    };

    file.write(
        reinterpret_cast<const char*>(
            lengthBytes),
        sizeof(lengthBytes));

    file.write(
        reinterpret_cast<const char*>(
            payload.data()),
        static_cast<std::streamsize>(
            payload.size()));
}


//
// Create a minimal valid ITCH System Event message.
//
// Layout:
//
// type             1 byte
// stock locate     2 bytes
// tracking number  2 bytes
// timestamp        6 bytes
// event code       1 byte
//
// Total: 12 bytes
//
std::vector<std::uint8_t>
makeSystemEvent(
    std::uint64_t timestamp,
    char eventCode)
{
    std::vector<std::uint8_t>
        payload(
            12,
            0);

    payload[0] =
        static_cast<std::uint8_t>(
            'S');

    payload[5] =
        static_cast<std::uint8_t>(
            (timestamp >> 40) &
            0xFF);

    payload[6] =
        static_cast<std::uint8_t>(
            (timestamp >> 32) &
            0xFF);

    payload[7] =
        static_cast<std::uint8_t>(
            (timestamp >> 24) &
            0xFF);

    payload[8] =
        static_cast<std::uint8_t>(
            (timestamp >> 16) &
            0xFF);

    payload[9] =
        static_cast<std::uint8_t>(
            (timestamp >> 8) &
            0xFF);

    payload[10] =
        static_cast<std::uint8_t>(
            timestamp &
            0xFF);

    payload[11] =
        static_cast<std::uint8_t>(
            eventCode);

    return payload;
}


//
// Create five BinaryFILE records:
//
// record 1 -> Mold message sequence 1
// record 2 -> Mold message sequence 2
// ...
// record 5 -> Mold message sequence 5
//
std::filesystem::path
createTestBinaryFile()
{
    const auto path =
        std::filesystem::temp_directory_path() /
        "llt_file_mold_itch_recovery_test.bin";

    std::ofstream file(
        path,
        std::ios::binary |
        std::ios::trunc);

    REQUIRE(
        file.is_open());

    writeRecord(
        file,
        makeSystemEvent(
            1001,
            'O'));

    writeRecord(
        file,
        makeSystemEvent(
            1002,
            'S'));

    writeRecord(
        file,
        makeSystemEvent(
            1003,
            'Q'));

    writeRecord(
        file,
        makeSystemEvent(
            1004,
            'M'));

    writeRecord(
        file,
        makeSystemEvent(
            1005,
            'E'));

    file.close();

    return path;
}

} // namespace


TEST_CASE(
    "FileMoldItchRecoverySource recovers sequenced ITCH messages")
{
    const auto path =
        createTestBinaryFile();

    //
    // Small interval deliberately creates:
    //
    // sequence 1 -> checkpoint
    // sequence 3 -> checkpoint
    // sequence 5 -> checkpoint
    //
    // This lets the recovery request exercise
    // checkpoint lookup + forward scanning.
    //
    llt::moldudp64::FileMoldItchRecoverySource source{
        path.string(),
        2};

    REQUIRE(
        source.indexReady());

    REQUIRE(
        source.indexedMessages() ==
        5);

    REQUIRE(
        source.checkpointInterval() ==
        2);

    REQUIRE(
        source.checkpointCount() ==
        3);

    std::vector<
        llt::moldudp64::SequencedItchMessage>
        messages;

    //
    // Half-open range:
    //
    // [2, 5)
    //
    // should recover:
    //
    // 2
    // 3
    // 4
    //
    REQUIRE(
        source.recover(
            2,
            5,
            messages));

    REQUIRE(
        messages.size() ==
        3);

    REQUIRE(
        messages[0].sequence ==
        2);

    REQUIRE(
        messages[1].sequence ==
        3);

    REQUIRE(
        messages[2].sequence ==
        4);

    REQUIRE(
        messages[0].payloadSize ==
        12);

    REQUIRE(
        messages[1].payloadSize ==
        12);

    REQUIRE(
        messages[2].payloadSize ==
        12);

    //
    // All recovered records are System Event
    // messages.
    //
    REQUIRE(
        messages[0].payload[0] ==
        static_cast<std::uint8_t>(
            'S'));

    REQUIRE(
        messages[1].payload[0] ==
        static_cast<std::uint8_t>(
            'S'));

    REQUIRE(
        messages[2].payload[0] ==
        static_cast<std::uint8_t>(
            'S'));

    //
    // Verify we recovered the correct BinaryFILE
    // records, not merely the correct count.
    //
    REQUIRE(
        messages[0].payload[11] ==
        static_cast<std::uint8_t>(
            'S'));

    REQUIRE(
        messages[1].payload[11] ==
        static_cast<std::uint8_t>(
            'Q'));

    REQUIRE(
        messages[2].payload[11] ==
        static_cast<std::uint8_t>(
            'M'));

    REQUIRE(
        source.recoveryRequests() ==
        1);

    REQUIRE(
        source.recoveredMessages() ==
        3);

    std::filesystem::remove(
        path);
}