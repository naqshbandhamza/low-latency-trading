#include <arpa/inet.h>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

namespace
{

constexpr std::uint16_t Port = 19001;

// Roughly representative of a small market-data packet.
// We can test larger sizes later.
constexpr std::size_t PayloadSize = 64;

constexpr std::uint64_t PacketCount =
    10'000'000;

} // namespace

int main()
{
    // ---------------------------------------------------------
    // Receiver socket
    // ---------------------------------------------------------

    const int receiverSocket =
        ::socket(AF_INET, SOCK_DGRAM, 0);

    if (receiverSocket < 0)
    {
        std::perror("receiver socket");
        return 1;
    }

    int receiveBufferSize =
        16 * 1024 * 1024;

    ::setsockopt(
        receiverSocket,
        SOL_SOCKET,
        SO_RCVBUF,
        &receiveBufferSize,
        sizeof(receiveBufferSize));

    sockaddr_in receiverAddress{};

    receiverAddress.sin_family =
        AF_INET;

    receiverAddress.sin_port =
        htons(Port);

    receiverAddress.sin_addr.s_addr =
        htonl(INADDR_LOOPBACK);

    if (
        ::bind(
            receiverSocket,
            reinterpret_cast<sockaddr*>(
                &receiverAddress),
            sizeof(receiverAddress)) < 0)
    {
        std::perror("bind");
        ::close(receiverSocket);
        return 1;
    }

    // ---------------------------------------------------------
    // Sender socket
    // ---------------------------------------------------------

    const int senderSocket =
        ::socket(AF_INET, SOCK_DGRAM, 0);

    if (senderSocket < 0)
    {
        std::perror("sender socket");
        ::close(receiverSocket);
        return 1;
    }

    int sendBufferSize =
        16 * 1024 * 1024;

    ::setsockopt(
        senderSocket,
        SOL_SOCKET,
        SO_SNDBUF,
        &sendBufferSize,
        sizeof(sendBufferSize));

    sockaddr_in destination{};

    destination.sin_family =
        AF_INET;

    destination.sin_port =
        htons(Port);

    destination.sin_addr.s_addr =
        htonl(INADDR_LOOPBACK);

    // ---------------------------------------------------------
    // Prebuilt payload
    // ---------------------------------------------------------

    alignas(64)
    std::uint8_t payload[PayloadSize]{};

    // Receiver buffer.
    alignas(64)
    std::uint8_t receiveBuffer[PayloadSize]{};

    // Put a recognizable value into the packet so this isn't
    // conceptually an empty payload.
    payload[0] = 0xAB;
    payload[1] = 0xCD;

    std::atomic<bool> receiverReady{false};

    std::atomic<std::uint64_t>
        receivedPackets{0};

    std::atomic<bool>
        senderFinished{false};

    // ---------------------------------------------------------
    // Receiver
    // ---------------------------------------------------------

    std::thread receiver(
        [&]()
        {
            receiverReady.store(
                true,
                std::memory_order_release);

            while (true)
            {
                const auto received =
                    ::recvfrom(
                        receiverSocket,
                        receiveBuffer,
                        sizeof(receiveBuffer),
                        MSG_DONTWAIT,
                        nullptr,
                        nullptr);

                if (received > 0)
                {
                    receivedPackets.fetch_add(
                        1,
                        std::memory_order_relaxed);

                    continue;
                }

                if (
                    senderFinished.load(
                        std::memory_order_acquire))
                {
                    // Give the socket a little time to drain
                    // after the sender has finished.
                    bool drained = true;

                    for (int i = 0; i < 1000; ++i)
                    {
                        const auto finalReceived =
                            ::recvfrom(
                                receiverSocket,
                                receiveBuffer,
                                sizeof(receiveBuffer),
                                MSG_DONTWAIT,
                                nullptr,
                                nullptr);

                        if (finalReceived > 0)
                        {
                            receivedPackets.fetch_add(
                                1,
                                std::memory_order_relaxed);

                            drained = false;
                        }
                        else
                        {
                            break;
                        }
                    }

                    if (drained)
                    {
                        break;
                    }
                }

                std::this_thread::yield();
            }
        });

    while (
        !receiverReady.load(
            std::memory_order_acquire))
    {
        std::this_thread::yield();
    }

    // ---------------------------------------------------------
    // Benchmark
    // ---------------------------------------------------------

    std::cout
        << "========================================\n"
        << "       UDP THROUGHPUT BENCHMARK\n"
        << "========================================\n"
        << "Packets      : "
        << PacketCount
        << '\n'
        << "Payload size : "
        << PayloadSize
        << " bytes\n"
        << "Transport    : UDP loopback\n"
        << "Send API     : sendto() per packet\n"
        << "Receive API  : recvfrom() per packet\n"
        << "========================================\n";

    const auto start =
        std::chrono::steady_clock::now();

    std::uint64_t sentPackets{0};
    std::uint64_t sendErrors{0};

    for (
        std::uint64_t i = 0;
        i < PacketCount;
        ++i)
    {
        const auto sent =
            ::sendto(
                senderSocket,
                payload,
                sizeof(payload),
                0,
                reinterpret_cast<const sockaddr*>(
                    &destination),
                sizeof(destination));

        if (
            sent ==
            static_cast<ssize_t>(
                sizeof(payload)))
        {
            ++sentPackets;
        }
        else
        {
            ++sendErrors;
        }
    }

    const auto senderEnd =
        std::chrono::steady_clock::now();

    senderFinished.store(
        true,
        std::memory_order_release);

    receiver.join();

    const auto end =
        std::chrono::steady_clock::now();

    // ---------------------------------------------------------
    // Results
    // ---------------------------------------------------------

    const auto senderSeconds =
        std::chrono::duration<double>(
            senderEnd - start)
            .count();

    const auto totalSeconds =
        std::chrono::duration<double>(
            end - start)
            .count();

    const auto received =
        receivedPackets.load(
            std::memory_order_relaxed);

    const double sendRate =
        senderSeconds > 0.0
            ? static_cast<double>(
                  sentPackets) /
                  senderSeconds
            : 0.0;

    const double receiveRate =
        totalSeconds > 0.0
            ? static_cast<double>(
                  received) /
                  totalSeconds
            : 0.0;

    const std::uint64_t lostPackets =
        sentPackets >= received
            ? sentPackets - received
            : 0;

    const double lossPercentage =
        sentPackets > 0
            ? (
                  static_cast<double>(
                      lostPackets) /
                  static_cast<double>(
                      sentPackets)) *
                  100.0
            : 0.0;

    std::cout
        << "\n========================================\n"
        << "              RESULTS\n"
        << "========================================\n"
        << "Packets attempted : "
        << PacketCount
        << '\n'
        << "Packets sent      : "
        << sentPackets
        << '\n'
        << "Send errors       : "
        << sendErrors
        << '\n'
        << "Packets received  : "
        << received
        << '\n'
        << "Packets lost      : "
        << lostPackets
        << '\n'
        << "Loss              : "
        << lossPercentage
        << " %\n"
        << '\n'
        << "Sender elapsed    : "
        << senderSeconds
        << " sec\n"
        << "Total elapsed     : "
        << totalSeconds
        << " sec\n"
        << '\n'
        << "Send rate         : "
        << static_cast<std::uint64_t>(
               sendRate)
        << " packets/sec\n"
        << "Receive rate      : "
        << static_cast<std::uint64_t>(
               receiveRate)
        << " packets/sec\n"
        << "========================================\n";

    ::close(senderSocket);
    ::close(receiverSocket);

    return 0;
}