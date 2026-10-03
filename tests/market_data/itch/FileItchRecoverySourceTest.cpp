#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "market_data/itch/FileItchRecoverySource.h"
#include "market_data/itch/ItchUdpPacket.h"

namespace
{

    //
    // Write one BinaryFILE record:
    //
    //     2-byte big-endian payload length
    //     N-byte ITCH payload
    //
    void writeRecord(
        std::ofstream &file,
        const std::vector<std::uint8_t> &payload)
    {
        const auto size =
            static_cast<std::uint16_t>(
                payload.size());

        const std::uint8_t lengthBytes[2] = {
            static_cast<std::uint8_t>(
                (size >> 8) & 0xFF),
            static_cast<std::uint8_t>(
                size & 0xFF)};

        file.write(
            reinterpret_cast<const char *>(
                lengthBytes),
            sizeof(lengthBytes));

        file.write(
            reinterpret_cast<const char *>(
                payload.data()),
            static_cast<std::streamsize>(
                payload.size()));
    }

    //
    // Create a minimal System Event message.
    //
    // ITCH System Event:
    //     type             1 byte
    //     stock locate     2 bytes
    //     tracking number  2 bytes
    //     timestamp        6 bytes
    //     event code       1 byte
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
            static_cast<std::uint8_t>('S');

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

    std::filesystem::path
    createTestBinaryFile()
    {
        const auto path =
            std::filesystem::temp_directory_path() /
            "llt_file_itch_recovery_test.bin";

        std::ofstream file(
            path,
            std::ios::binary |
                std::ios::trunc);

        REQUIRE(file.is_open());

        //
        // Transport sequence mapping:
        //
        // record 1 -> sequence 1
        // record 2 -> sequence 2
        // ...
        //
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

    class TestFile
    {
    public:
        TestFile()
            : path_(
                  createTestBinaryFile())
        {
        }

        ~TestFile()
        {
            std::error_code error;

            std::filesystem::remove(
                path_,
                error);

            error.clear();

            std::filesystem::remove(
                path_.string() + ".idx",
                error);

            error.clear();

            std::filesystem::remove(
                path_.string() + ".idx.tmp",
                error);
        }

        [[nodiscard]]
        const std::filesystem::path &
        path() const noexcept
        {
            return path_;
        }

    private:
        std::filesystem::path path_;
    };

} // namespace

TEST_CASE(
    "File ITCH recovery source builds record index")
{
    TestFile testFile;

    llt::itch::FileItchRecoverySource
        source{
            testFile.path().string()};

    REQUIRE(
        source.indexReady());

    REQUIRE(
        source.indexedRecords() ==
        5);
}

TEST_CASE(
    "File ITCH recovery source recovers exact packet range")
{
    TestFile testFile;

    llt::itch::FileItchRecoverySource
        source{
            testFile.path().string()};

    std::vector<
        llt::itch::ItchUdpPacket>
        packets;

    //
    // Half-open:
    //
    // [2, 5)
    //
    // should return:
    //
    // 2, 3, 4
    //
    REQUIRE(
        source.recover(
            2,
            5,
            packets));

    REQUIRE(
        packets.size() ==
        3);

    REQUIRE(
        packets[0].sequence ==
        2);

    REQUIRE(
        packets[1].sequence ==
        3);

    REQUIRE(
        packets[2].sequence ==
        4);

    //
    // Make sure these really are the corresponding
    // underlying ITCH records.
    //
    REQUIRE(
        packets[0].payload[0] ==
        static_cast<std::uint8_t>('S'));

    REQUIRE(
        packets[0].payload[11] ==
        static_cast<std::uint8_t>('S'));

    REQUIRE(
        packets[1].payload[11] ==
        static_cast<std::uint8_t>('Q'));

    REQUIRE(
        packets[2].payload[11] ==
        static_cast<std::uint8_t>('M'));

    REQUIRE(
        source.recoveryRequests() ==
        1);

    REQUIRE(
        source.recoveredPackets() ==
        3);
}

TEST_CASE(
    "File ITCH recovery source recovers single packet")
{
    TestFile testFile;

    llt::itch::FileItchRecoverySource
        source{
            testFile.path().string()};

    std::vector<
        llt::itch::ItchUdpPacket>
        packets;

    REQUIRE(
        source.recover(
            3,
            4,
            packets));

    REQUIRE(
        packets.size() ==
        1);

    REQUIRE(
        packets[0].sequence ==
        3);

    REQUIRE(
        packets[0].payload[0] ==
        static_cast<std::uint8_t>('S'));

    REQUIRE(
        packets[0].payload[11] ==
        static_cast<std::uint8_t>('Q'));
}

TEST_CASE(
    "File ITCH recovery source rejects invalid range")
{
    TestFile testFile;

    llt::itch::FileItchRecoverySource
        source{
            testFile.path().string()};

    std::vector<
        llt::itch::ItchUdpPacket>
        packets;

    SECTION("equal range")
    {
        REQUIRE_FALSE(
            source.recover(
                3,
                3,
                packets));

        REQUIRE(
            packets.empty());
    }

    SECTION("backward range")
    {
        REQUIRE_FALSE(
            source.recover(
                4,
                3,
                packets));

        REQUIRE(
            packets.empty());
    }
}

TEST_CASE(
    "File ITCH recovery source rejects sequence zero")
{
    TestFile testFile;

    llt::itch::FileItchRecoverySource
        source{
            testFile.path().string()};

    std::vector<
        llt::itch::ItchUdpPacket>
        packets;

    REQUIRE_FALSE(
        source.recover(
            0,
            1,
            packets));

    REQUIRE(
        packets.empty());
}

TEST_CASE(
    "File ITCH recovery source rejects range beyond indexed file")
{
    TestFile testFile;

    llt::itch::FileItchRecoverySource
        source{
            testFile.path().string()};

    std::vector<
        llt::itch::ItchUdpPacket>
        packets;

    //
    // Only sequences 1..5 exist.
    //
    REQUIRE_FALSE(
        source.recover(
            5,
            7,
            packets));

    REQUIRE(
        packets.empty());
}

TEST_CASE(
    "File ITCH recovery source fails for missing file")
{
    const auto path =
        std::filesystem::temp_directory_path() /
        "llt_file_that_does_not_exist.bin";

    std::error_code error;

    std::filesystem::remove(
        path,
        error);

    llt::itch::FileItchRecoverySource
        source{
            path.string()};

    REQUIRE_FALSE(
        source.indexReady());

    REQUIRE(
        source.indexedRecords() ==
        0);

    std::vector<
        llt::itch::ItchUdpPacket>
        packets;

    REQUIRE_FALSE(
        source.recover(
            1,
            2,
            packets));

    REQUIRE(
        packets.empty());
}

TEST_CASE(
    "File ITCH recovery source rejects zero checkpoint interval")
{
    TestFile testFile;

    llt::itch::FileItchRecoverySource
        source{
            testFile.path().string(),
            0};

    REQUIRE_FALSE(
        source.indexReady());

    REQUIRE(
        source.checkpointCount() ==
        0);

    std::vector<
        llt::itch::ItchUdpPacket>
        packets;

    REQUIRE_FALSE(
        source.recover(
            1,
            2,
            packets));
}

TEST_CASE(
    "File ITCH recovery source persists sparse index")
{
    TestFile testFile;

    const auto indexPath =
        std::filesystem::path{
            testFile.path().string() +
            ".idx"};

    std::error_code error;

    std::filesystem::remove(
        indexPath,
        error);

    REQUIRE_FALSE(
        std::filesystem::exists(
            indexPath));

    {
        llt::itch::FileItchRecoverySource
            source{
                testFile.path().string(),
                2};

        REQUIRE(
            source.indexReady());

        REQUIRE_FALSE(
            source.indexLoadedFromDisk());

        REQUIRE(
            source.indexedRecords() ==
            5);

        REQUIRE(
            source.checkpointCount() ==
            3);
    }

    //
    // First construction should have generated:
    //
    // <BinaryFILE>.idx
    //
    REQUIRE(
        std::filesystem::exists(
            indexPath));

    REQUIRE(
        std::filesystem::file_size(
            indexPath) >
        0);

    std::filesystem::remove(
        indexPath,
        error);
}

TEST_CASE(
    "File ITCH recovery source loads persisted sparse index")
{
    TestFile testFile;

    const auto indexPath =
        std::filesystem::path{
            testFile.path().string() +
            ".idx"};

    std::error_code error;

    std::filesystem::remove(
        indexPath,
        error);

    //
    // First source builds + persists the index.
    //
    {
        llt::itch::FileItchRecoverySource
            first{
                testFile.path().string(),
                2};

        REQUIRE(
            first.indexReady());

        REQUIRE_FALSE(
            first.indexLoadedFromDisk());

        REQUIRE(
            first.checkpointCount() ==
            3);
    }

    REQUIRE(
        std::filesystem::exists(
            indexPath));

    //
    // Second source should NOT rebuild.
    //
    // It should load the .idx file.
    //
    {
        llt::itch::FileItchRecoverySource
            second{
                testFile.path().string(),
                2};

        REQUIRE(
            second.indexReady());

        REQUIRE(
            second.indexLoadedFromDisk());

        REQUIRE(
            second.indexedRecords() ==
            5);

        REQUIRE(
            second.checkpointCount() ==
            3);

        //
        // Most importantly, prove recovery still
        // works using the deserialized checkpoints.
        //
        std::vector<
            llt::itch::ItchUdpPacket>
            packets;

        REQUIRE(
            second.recover(
                4,
                5,
                packets));

        REQUIRE(
            packets.size() ==
            1);

        REQUIRE(
            packets[0].sequence ==
            4);

        REQUIRE(
            packets[0].payload[11] ==
            static_cast<std::uint8_t>(
                'M'));
    }

    std::filesystem::remove(
        indexPath,
        error);
}

TEST_CASE(
    "File ITCH recovery source rebuilds corrupted persisted index")
{
    TestFile testFile;

    const auto indexPath =
        std::filesystem::path{
            testFile.path().string() +
            ".idx"};

    std::error_code error;

    std::filesystem::remove(
        indexPath,
        error);

    //
    // Create a valid index first.
    //
    {
        llt::itch::FileItchRecoverySource
            source{
                testFile.path().string(),
                2};

        REQUIRE(
            source.indexReady());

        REQUIRE_FALSE(
            source.indexLoadedFromDisk());
    }

    REQUIRE(
        std::filesystem::exists(
            indexPath));

    //
    // Destroy the index contents.
    //
    {
        std::ofstream corrupted(
            indexPath,
            std::ios::binary |
                std::ios::trunc);

        REQUIRE(
            corrupted.is_open());

        const char garbage[] = {
            'B',
            'A',
            'D',
            'I',
            'N',
            'D',
            'E',
            'X'};

        corrupted.write(
            garbage,
            sizeof(garbage));

        corrupted.close();
    }

    //
    // Loading must reject BADINDEX and rebuild
    // from the BinaryFILE.
    //
    {
        llt::itch::FileItchRecoverySource
            rebuilt{
                testFile.path().string(),
                2};

        REQUIRE(
            rebuilt.indexReady());

        //
        // This is the key assertion.
        //
        // false means:
        //
        // corrupted .idx rejected
        //         ↓
        // BinaryFILE rescanned
        //         ↓
        // new index generated
        //
        REQUIRE_FALSE(
            rebuilt.indexLoadedFromDisk());

        REQUIRE(
            rebuilt.indexedRecords() ==
            5);

        REQUIRE(
            rebuilt.checkpointCount() ==
            3);

        std::vector<
            llt::itch::ItchUdpPacket>
            packets;

        REQUIRE(
            rebuilt.recover(
                3,
                4,
                packets));

        REQUIRE(
            packets.size() ==
            1);

        REQUIRE(
            packets[0].sequence ==
            3);

        REQUIRE(
            packets[0].payload[11] ==
            static_cast<std::uint8_t>(
                'Q'));
    }

    //
    // And the rebuilt index itself should now
    // be loadable.
    //
    {
        llt::itch::FileItchRecoverySource
            loaded{
                testFile.path().string(),
                2};

        REQUIRE(
            loaded.indexReady());

        REQUIRE(
            loaded.indexLoadedFromDisk());
    }

    std::filesystem::remove(
        indexPath,
        error);
}

TEST_CASE(
    "File ITCH recovery source invalidates index when source file size changes")
{
    TestFile testFile;

    const auto indexPath =
        std::filesystem::path{
            testFile.path().string() +
            ".idx"};

    std::error_code error;

    std::filesystem::remove(
        indexPath,
        error);

    //
    // Original fixture:
    //
    // 5 records.
    //
    {
        llt::itch::FileItchRecoverySource
            original{
                testFile.path().string(),
                2};

        REQUIRE(
            original.indexReady());

        REQUIRE_FALSE(
            original.indexLoadedFromDisk());

        REQUIRE(
            original.indexedRecords() ==
            5);
    }

    REQUIRE(
        std::filesystem::exists(
            indexPath));

    //
    // Change the underlying BinaryFILE itself.
    //
    // This increases its byte size and adds
    // transport sequence 6.
    //
    {
        std::ofstream file(
            testFile.path(),
            std::ios::binary |
                std::ios::app);

        REQUIRE(
            file.is_open());

        writeRecord(
            file,
            makeSystemEvent(
                1006,
                'C'));

        file.close();
    }

    //
    // The persisted index says it belongs to the
    // OLD file size.
    //
    // Therefore it must be rejected and rebuilt.
    //
    {
        llt::itch::FileItchRecoverySource
            rebuilt{
                testFile.path().string(),
                2};

        REQUIRE(
            rebuilt.indexReady());

        REQUIRE_FALSE(
            rebuilt.indexLoadedFromDisk());

        REQUIRE(
            rebuilt.indexedRecords() ==
            6);

        //
        // Interval 2:
        //
        // checkpoints:
        // 1
        // 3
        // 5
        //
        REQUIRE(
            rebuilt.checkpointCount() ==
            3);

        //
        // Prove newly appended sequence 6 can
        // actually be recovered.
        //
        std::vector<
            llt::itch::ItchUdpPacket>
            packets;

        REQUIRE(
            rebuilt.recover(
                6,
                7,
                packets));

        REQUIRE(
            packets.size() ==
            1);

        REQUIRE(
            packets[0].sequence ==
            6);

        REQUIRE(
            packets[0].payload[11] ==
            static_cast<std::uint8_t>(
                'C'));
    }

    //
    // The newly rebuilt index should now match
    // the modified BinaryFILE and load normally.
    //
    {
        llt::itch::FileItchRecoverySource
            loaded{
                testFile.path().string(),
                2};

        REQUIRE(
            loaded.indexReady());

        REQUIRE(
            loaded.indexLoadedFromDisk());

        REQUIRE(
            loaded.indexedRecords() ==
            6);
    }

    std::filesystem::remove(
        indexPath,
        error);
}