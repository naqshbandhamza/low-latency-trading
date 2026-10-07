#include <catch2/catch_test_macros.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "MoldItchFeedHandler.h"

#include "market_data/itch/ItchMarketState.h"

#include "market_data/moldudp64/FileMoldItchRecoverySource.h"
#include "market_data/moldudp64/IMoldMarketDataSource.h"
#include "market_data/moldudp64/MoldItchSequenceRecovery.h"
#include "market_data/moldudp64/MoldUdp64.h"
#include "market_data/moldudp64/MoldUdp64Codec.h"


namespace
{

using llt::moldudp64::ReceivedMoldDatagram;
using llt::moldudp64::Session;


// ---------------------------------------------------------
// Mock LIVE source only.
//
// Recovery itself is completely real:
//
// FileMoldItchRecoverySource
//          ↓
// MoldItchSequenceRecovery
//          ↓
// MoldItchFeedHandler
// ---------------------------------------------------------

class MockMoldMarketDataSource final
    : public llt::moldudp64::IMoldMarketDataSource
{
public:
    bool receive(
        ReceivedMoldDatagram& datagram
    ) noexcept override
    {
        if (nextDatagram_ >= datagrams_.size())
        {
            return false;
        }

        datagram =
            datagrams_[nextDatagram_];

        ++nextDatagram_;

        return true;
    }


    void addDatagram(
        const ReceivedMoldDatagram& datagram)
    {
        datagrams_.push_back(
            datagram);
    }


private:
    std::vector<ReceivedMoldDatagram>
        datagrams_;

    std::size_t nextDatagram_{0};
};


// ---------------------------------------------------------
// Valid 12-byte ITCH System Event message.
//
// Layout:
//
// [0]      Message Type = 'S'
// [1..2]   Stock Locate
// [3..4]   Tracking Number
// [5..10]  Timestamp (6-byte BE)
// [11]     Event Code
// ---------------------------------------------------------

std::array<std::uint8_t, 12>
makeSystemEventMessage(
    char eventCode,
    std::uint64_t timestamp)
{
    std::array<std::uint8_t, 12>
        message{};

    message[0] =
        static_cast<std::uint8_t>('S');

    // Stock Locate
    message[1] = 0;
    message[2] = 0;

    // Tracking Number
    message[3] = 0;
    message[4] = 0;

    // 6-byte big-endian timestamp
    message[5] =
        static_cast<std::uint8_t>(
            (timestamp >> 40) & 0xFF);

    message[6] =
        static_cast<std::uint8_t>(
            (timestamp >> 32) & 0xFF);

    message[7] =
        static_cast<std::uint8_t>(
            (timestamp >> 24) & 0xFF);

    message[8] =
        static_cast<std::uint8_t>(
            (timestamp >> 16) & 0xFF);

    message[9] =
        static_cast<std::uint8_t>(
            (timestamp >> 8) & 0xFF);

    message[10] =
        static_cast<std::uint8_t>(
            timestamp & 0xFF);

    message[11] =
        static_cast<std::uint8_t>(
            eventCode);

    return message;
}


// ---------------------------------------------------------
// Synthetic Mold session.
//
// Exactly 10 bytes.
// ---------------------------------------------------------

Session makeSession()
{
    Session session{};

    constexpr char value[] =
        "20190130A ";

    static_assert(
        sizeof(value) - 1 ==
        llt::moldudp64::SessionSize);

    std::memcpy(
        session.data(),
        value,
        llt::moldudp64::SessionSize);

    return session;
}


// ---------------------------------------------------------
// Build one real MoldUDP64 datagram containing N ITCH
// messages.
// ---------------------------------------------------------

ReceivedMoldDatagram makeDatagram(
    std::uint64_t firstSequence,
    const std::vector<
        std::array<std::uint8_t, 12>>& messages)
{
    ReceivedMoldDatagram datagram{};

    std::size_t encodedSize{0};

    std::uint16_t messageCount{0};

    const auto session =
        makeSession();

    REQUIRE(
        llt::moldudp64::MoldUdp64Codec::
            beginPacket(
                session,
                firstSequence,
                datagram.bytes,
                encodedSize));

    for (const auto& message : messages)
    {
        REQUIRE(
            llt::moldudp64::MoldUdp64Codec::
                appendMessage(
                    message.data(),
                    message.size(),
                    datagram.bytes,
                    encodedSize,
                    messageCount));
    }

    llt::moldudp64::MoldUdp64Codec::
        finalizePacket(
            datagram.bytes,
            messageCount);

    datagram.size =
        encodedSize;

    return datagram;
}


// ---------------------------------------------------------
// Write one BinaryFILE record.
//
// BinaryFILE format:
//
// 2-byte big-endian length
// followed by ITCH message bytes.
// ---------------------------------------------------------

void writeBinaryFileRecord(
    std::ofstream& file,
    const std::array<std::uint8_t, 12>& message)
{
    const auto length =
        static_cast<std::uint16_t>(
            message.size());

    const std::uint8_t lengthBytes[2] = {
        static_cast<std::uint8_t>(
            (length >> 8) & 0xFF),

        static_cast<std::uint8_t>(
            length & 0xFF)
    };

    file.write(
        reinterpret_cast<const char*>(
            lengthBytes),
        2);

    file.write(
        reinterpret_cast<const char*>(
            message.data()),
        static_cast<std::streamsize>(
            message.size()));
}


// ---------------------------------------------------------
// Create temporary BinaryFILE containing:
//
// sequence 1 → O
// sequence 2 → S
// sequence 3 → Q
// sequence 4 → M
// sequence 5 → E
//
// FileMoldItchRecoverySource assigns sequence numbers
// according to BinaryFILE record position.
// ---------------------------------------------------------

std::filesystem::path
createRecoveryFile()
{
    const auto path =
        std::filesystem::temp_directory_path() /
        "llt_mold_recovery_integration_test.bin";

    std::ofstream file(
        path,
        std::ios::binary |
        std::ios::trunc);

    REQUIRE(
        file.is_open());

    writeBinaryFileRecord(
        file,
        makeSystemEventMessage(
            'O',
            1001));

    writeBinaryFileRecord(
        file,
        makeSystemEventMessage(
            'S',
            1002));

    writeBinaryFileRecord(
        file,
        makeSystemEventMessage(
            'Q',
            1003));

    writeBinaryFileRecord(
        file,
        makeSystemEventMessage(
            'M',
            1004));

    writeBinaryFileRecord(
        file,
        makeSystemEventMessage(
            'E',
            1005));

    file.close();

    return path;
}


void removeRecoveryFiles(
    const std::filesystem::path& path)
{
    std::error_code error;

    std::filesystem::remove(
        path,
        error);

    //
    // Keep this cleanup because FileMoldItchRecoverySource
    // may later use the same persistent .idx design as the
    // existing FileItchRecoverySource.
    //
    auto indexPath =
        path;

    indexPath += ".idx";

    std::filesystem::remove(
        indexPath,
        error);
}

} // namespace


TEST_CASE(
    "Mold feed handler performs real file recovery across a live sequence gap")
{
    const auto recoveryFile =
        createRecoveryFile();

    //
    // Small checkpoint interval deliberately exercises
    // indexed recovery rather than only sequence 1.
    //
    llt::moldudp64::FileMoldItchRecoverySource
        fileRecoverySource{
            recoveryFile.string(),
            2};

    REQUIRE(
        fileRecoverySource.indexReady());

    REQUIRE(
        fileRecoverySource.indexedMessages() ==
        5);

    REQUIRE(
        fileRecoverySource.checkpointCount() ==
        3);


    llt::moldudp64::MoldItchSequenceRecovery
        sequenceRecovery{
            fileRecoverySource};


    MockMoldMarketDataSource
        liveSource;


    llt::itch::ItchMarketState
        marketState;


    // -----------------------------------------------------
    // LIVE DATAGRAM #1
    //
    // sequence 1
    //
    // After this:
    //
    // expectedSequence = 2
    // -----------------------------------------------------

    liveSource.addDatagram(
        makeDatagram(
            1,
            {
                makeSystemEventMessage(
                    'O',
                    1001)
            }));


    // -----------------------------------------------------
    // LIVE DATAGRAM #2
    //
    // sequence 5
    //
    // Handler expects sequence 2.
    //
    // Therefore gap is:
    //
    // [2, 5)
    //
    // Recovery must load:
    //
    // sequence 2
    // sequence 3
    // sequence 4
    //
    // from the BinaryFILE.
    // -----------------------------------------------------

    liveSource.addDatagram(
        makeDatagram(
            5,
            {
                makeSystemEventMessage(
                    'E',
                    1005)
            }));


    llt::itch::MoldItchFeedHandler
        handler{
            liveSource,
            sequenceRecovery,
            marketState};


    handler.start(2);


    // -----------------------------------------------------
    // Lifecycle
    // -----------------------------------------------------

    CHECK(
        handler.state() ==
        llt::FeedHandlerState::Stopped);


    // -----------------------------------------------------
    // Live path
    //
    // sequence 1
    // sequence 5
    // -----------------------------------------------------

    CHECK(
        handler.processedDatagrams() ==
        2);

    CHECK(
        handler.processedMessages() ==
        2);


    // -----------------------------------------------------
    // Recovery path
    //
    // sequence 2
    // sequence 3
    // sequence 4
    // -----------------------------------------------------

    CHECK(
        handler.recoveredMessages() ==
        3);


    CHECK(
        handler.gapsDetected() ==
        1);


    CHECK(
        handler.ignoredMessages() ==
        0);


    CHECK(
        handler.malformedDatagrams() ==
        0);


    // -----------------------------------------------------
    // Entire logical sequence has now been processed:
    //
    // 1
    // 2 recovered
    // 3 recovered
    // 4 recovered
    // 5
    //
    // Therefore:
    //
    // expectedSequence = 6
    // -----------------------------------------------------

    CHECK(
        handler.expectedSequence() ==
        6);


    // -----------------------------------------------------
    // Verify the real file recovery source was actually
    // exercised.
    // -----------------------------------------------------

    CHECK(
        fileRecoverySource.recoveryRequests() ==
        1);

    CHECK(
        fileRecoverySource.recoveredMessages() ==
        3);


    removeRecoveryFiles(
        recoveryFile);
}