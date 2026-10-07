#pragma once

#include <cstddef>
#include <cstdint>

#include "market_data/moldudp64/MoldUdp64.h"

namespace llt::moldudp64
{

class MoldUdp64Codec
{
public:

    //
    // Begin constructing a new downstream packet.
    //
    // Writes:
    //
    //   session
    //   first sequence number
    //   message count = 0
    //
    [[nodiscard]]
    static bool beginPacket(
        const Session& session,
        std::uint64_t firstSequenceNumber,
        Datagram& output,
        std::size_t& encodedSize
    ) noexcept;


    //
    // Append one message block:
    //
    //   2-byte length
    //   message bytes
    //
    [[nodiscard]]
    static bool appendMessage(
        const std::uint8_t* message,
        std::size_t messageSize,
        Datagram& output,
        std::size_t& encodedSize,
        std::uint16_t& messageCount
    ) noexcept;


    //
    // Write the final message count into the
    // MoldUDP64 header.
    //
    static void finalizePacket(
        Datagram& output,
        std::uint16_t messageCount
    ) noexcept;


    //
    // Decode only the 20-byte downstream header.
    //
    [[nodiscard]]
    static bool decodeHeader(
        const std::uint8_t* data,
        std::size_t size,
        Header& header
    ) noexcept;


    //
    // Iterate one message from an already decoded
    // MoldUDP64 datagram.
    //
    // offset should initially be HeaderSize.
    //
    [[nodiscard]]
    static bool nextMessage(
        const std::uint8_t* data,
        std::size_t datagramSize,
        std::size_t& offset,
        std::uint64_t sequenceNumber,
        MessageView& message
    ) noexcept;

private:

    static void writeU16(
        std::uint8_t* destination,
        std::uint16_t value
    ) noexcept;

    static void writeU64(
        std::uint8_t* destination,
        std::uint64_t value
    ) noexcept;

    [[nodiscard]]
    static std::uint16_t readU16(
        const std::uint8_t* source
    ) noexcept;

    [[nodiscard]]
    static std::uint64_t readU64(
        const std::uint8_t* source
    ) noexcept;
};

} // namespace llt::moldudp64