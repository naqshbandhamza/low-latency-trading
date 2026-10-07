#include <catch2/catch_test_macros.hpp>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

#include "MoldItchFeedHandler.h"

#include "market_data/itch/ItchMarketState.h"

#include "market_data/moldudp64/FileMoldItchRecoverySource.h"
#include "market_data/moldudp64/MoldItchSequenceRecovery.h"
#include "market_data/moldudp64/MoldUdp64.h"
#include "market_data/moldudp64/MoldUdp64Codec.h"
#include "market_data/moldudp64/UdpMoldMarketDataSource.h"


namespace
{

using llt::moldudp64::ReceivedMoldDatagram;
using llt::moldudp64::Session;


// ---------------------------------------------------------
// Valid 12-byte ITCH System Event message.
// ---------------------------------------------------------

std::array<std::uint8_t, 12>
makeSystemEventMessage(
    char eventCode,
    std::uint64_t timestamp)
{
    std::array<std::uint8_t, 12> message{};

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
// Synthetic 10-byte Mold session.
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
// Build one MoldUDP64 datagram.
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
// BinaryFILE writer.
//
// Record:
// 2-byte big-endian length
// + ITCH payload.
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
// Recovery BinaryFILE:
//
// sequence 1 -> O
// sequence 2 -> S
// sequence 3 -> Q
// sequence 4 -> M
// sequence 5 -> E
// ---------------------------------------------------------

std::filesystem::path createRecoveryFile()
{
    const auto path =
        std::filesystem::temp_directory_path() /
        "llt_mold_udp_pipeline_integration_test.bin";

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

    auto indexPath =
        path;

    indexPath += ".idx";

    std::filesystem::remove(
        indexPath,
        error);
}


// ---------------------------------------------------------
// Send one raw MoldUDP64 datagram to loopback.
// ---------------------------------------------------------

bool sendDatagram(
    int socketFd,
    const sockaddr_in& destination,
    const ReceivedMoldDatagram& datagram)
{
    const auto sent =
        ::sendto(
            socketFd,
            datagram.bytes.data(),
            datagram.size,
            0,
            reinterpret_cast<
                const sockaddr*>(
                &destination),
            sizeof(destination));

    return
        sent ==
        static_cast<ssize_t>(
            datagram.size);
}

} // namespace


TEST_CASE(
    "Real UDP Mold pipeline recovers missing ITCH messages from BinaryFILE")
{
    //
    // Keep this separate from the port used by the
    // existing UdpMoldMarketDataSourceTest.
    //
    constexpr std::uint16_t port =
        19003;

    const auto recoveryFile =
        createRecoveryFile();


    // =====================================================
    // Real recovery source
    // =====================================================

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


    // =====================================================
    // Real UDP receiver
    // =====================================================

    //
    // 1000 ms receive timeout prevents the test from
    // hanging forever if something goes wrong.
    //
    llt::moldudp64::UdpMoldMarketDataSource
        udpSource{
            port,
            1000};


    llt::itch::ItchMarketState
        marketState;


    llt::itch::MoldItchFeedHandler
        handler{
            udpSource,
            sequenceRecovery,
            marketState};


    // =====================================================
    // Prepare real UDP sender
    // =====================================================

    const int senderSocket =
        ::socket(
            AF_INET,
            SOCK_DGRAM,
            0);

    REQUIRE(
        senderSocket >= 0);


    sockaddr_in destination{};

    destination.sin_family =
        AF_INET;

    destination.sin_port =
        htons(port);

    destination.sin_addr.s_addr =
        htonl(INADDR_LOOPBACK);


    // =====================================================
    // Construct the two LIVE datagrams.
    //
    // Live:
    //
    // seq 1
    // seq 5
    //
    // Missing:
    //
    // 2, 3, 4
    // =====================================================

    const auto firstDatagram =
        makeDatagram(
            1,
            {
                makeSystemEventMessage(
                    'O',
                    1001)
            });


    const auto secondDatagram =
        makeDatagram(
            5,
            {
                makeSystemEventMessage(
                    'E',
                    1005)
            });


    // =====================================================
    // Run the actual handler on another thread.
    //
    // start(2) returns after two successfully processed
    // live Mold datagrams.
    // =====================================================

    std::thread handlerThread(
        [&handler]()
        {
            handler.start(2);
        });


    //
    // Give the receiver thread a small amount of time
    // to enter receive().
    //
    // The UdpMoldMarketDataSource socket itself is already
    // constructed and bound before this point, so this is
    // not required for socket binding correctness; it only
    // makes test execution easier to reason about.
    //
    std::this_thread::sleep_for(
        std::chrono::milliseconds(
            20));


    // =====================================================
    // Send LIVE sequence 1.
    // =====================================================

    REQUIRE(
        sendDatagram(
            senderSocket,
            destination,
            firstDatagram));


    // =====================================================
    // Send LIVE sequence 5.
    //
    // Handler should detect:
    //
    // expected = 2
    // received = 5
    //
    // gap = [2,5)
    // =====================================================

    REQUIRE(
        sendDatagram(
            senderSocket,
            destination,
            secondDatagram));


    handlerThread.join();


    ::close(
        senderSocket);


    // =====================================================
    // Lifecycle
    // =====================================================

    CHECK(
        handler.state() ==
        llt::FeedHandlerState::Stopped);


    // =====================================================
    // Real UDP live path
    //
    // seq 1
    // seq 5
    // =====================================================

    CHECK(
        handler.processedDatagrams() ==
        2);

    CHECK(
        handler.processedMessages() ==
        2);


    // =====================================================
    // Real BinaryFILE recovery
    //
    // seq 2
    // seq 3
    // seq 4
    // =====================================================

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


    // =====================================================
    // Logical stream processed:
    //
    // 1 live
    // 2 recovered
    // 3 recovered
    // 4 recovered
    // 5 live
    //
    // Therefore next expected message = 6.
    // =====================================================

    CHECK(
        handler.expectedSequence() ==
        6);


    // =====================================================
    // Verify actual file recovery was used.
    // =====================================================

    CHECK(
        fileRecoverySource.recoveryRequests() ==
        1);

    CHECK(
        fileRecoverySource.recoveredMessages() ==
        3);


    removeRecoveryFiles(
        recoveryFile);
}