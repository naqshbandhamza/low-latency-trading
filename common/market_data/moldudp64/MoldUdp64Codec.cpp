#include "market_data/moldudp64/MoldUdp64Codec.h"

#include <cstring>
#include <limits>

namespace llt::moldudp64
{

namespace
{

constexpr std::size_t
    SessionOffset = 0;

constexpr std::size_t
    SequenceOffset = 10;

constexpr std::size_t
    MessageCountOffset = 18;

constexpr std::size_t
    MessageLengthSize = 2;

} // namespace


void MoldUdp64Codec::writeU16(
    std::uint8_t* destination,
    std::uint16_t value
) noexcept
{
    destination[0] =
        static_cast<std::uint8_t>(
            value >> 8);

    destination[1] =
        static_cast<std::uint8_t>(
            value);
}


void MoldUdp64Codec::writeU64(
    std::uint8_t* destination,
    std::uint64_t value
) noexcept
{
    destination[0] =
        static_cast<std::uint8_t>(
            value >> 56);

    destination[1] =
        static_cast<std::uint8_t>(
            value >> 48);

    destination[2] =
        static_cast<std::uint8_t>(
            value >> 40);

    destination[3] =
        static_cast<std::uint8_t>(
            value >> 32);

    destination[4] =
        static_cast<std::uint8_t>(
            value >> 24);

    destination[5] =
        static_cast<std::uint8_t>(
            value >> 16);

    destination[6] =
        static_cast<std::uint8_t>(
            value >> 8);

    destination[7] =
        static_cast<std::uint8_t>(
            value);
}


std::uint16_t MoldUdp64Codec::readU16(
    const std::uint8_t* source
) noexcept
{
    return
        static_cast<std::uint16_t>(
            (
                static_cast<std::uint16_t>(
                    source[0])
                << 8
            ) |
            static_cast<std::uint16_t>(
                source[1]));
}


std::uint64_t MoldUdp64Codec::readU64(
    const std::uint8_t* source
) noexcept
{
    return
        (
            static_cast<std::uint64_t>(
                source[0])
            << 56
        ) |
        (
            static_cast<std::uint64_t>(
                source[1])
            << 48
        ) |
        (
            static_cast<std::uint64_t>(
                source[2])
            << 40
        ) |
        (
            static_cast<std::uint64_t>(
                source[3])
            << 32
        ) |
        (
            static_cast<std::uint64_t>(
                source[4])
            << 24
        ) |
        (
            static_cast<std::uint64_t>(
                source[5])
            << 16
        ) |
        (
            static_cast<std::uint64_t>(
                source[6])
            << 8
        ) |
        static_cast<std::uint64_t>(
            source[7]);
}


bool MoldUdp64Codec::beginPacket(
    const Session& session,
    std::uint64_t firstSequenceNumber,
    Datagram& output,
    std::size_t& encodedSize
) noexcept
{
    output.fill(0);

    std::memcpy(
        output.data() +
            SessionOffset,
        session.data(),
        session.size());

    writeU64(
        output.data() +
            SequenceOffset,
        firstSequenceNumber);

    writeU16(
        output.data() +
            MessageCountOffset,
        0);

    encodedSize =
        HeaderSize;

    return true;
}


bool MoldUdp64Codec::appendMessage(
    const std::uint8_t* message,
    std::size_t messageSize,
    Datagram& output,
    std::size_t& encodedSize,
    std::uint16_t& messageCount
) noexcept
{
    if (
        message == nullptr ||
        messageSize == 0)
    {
        return false;
    }

    if (
        messageSize >
        std::numeric_limits<
            std::uint16_t>::max())
    {
        return false;
    }

    if (
        messageCount ==
        std::numeric_limits<
            std::uint16_t>::max())
    {
        return false;
    }

    const std::size_t requiredSize =
        MessageLengthSize +
        messageSize;

    if (
        encodedSize >
        output.size() ||
        requiredSize >
            output.size() -
                encodedSize)
    {
        return false;
    }

    writeU16(
        output.data() +
            encodedSize,
        static_cast<std::uint16_t>(
            messageSize));

    encodedSize +=
        MessageLengthSize;

    std::memcpy(
        output.data() +
            encodedSize,
        message,
        messageSize);

    encodedSize +=
        messageSize;

    ++messageCount;

    return true;
}


void MoldUdp64Codec::finalizePacket(
    Datagram& output,
    std::uint16_t messageCount
) noexcept
{
    writeU16(
        output.data() +
            MessageCountOffset,
        messageCount);
}


bool MoldUdp64Codec::decodeHeader(
    const std::uint8_t* data,
    std::size_t size,
    Header& header
) noexcept
{
    if (
        data == nullptr ||
        size < HeaderSize)
    {
        return false;
    }

    std::memcpy(
        header.session.data(),
        data +
            SessionOffset,
        SessionSize);

    header.sequenceNumber =
        readU64(
            data +
                SequenceOffset);

    header.messageCount =
        readU16(
            data +
                MessageCountOffset);

    return true;
}


bool MoldUdp64Codec::nextMessage(
    const std::uint8_t* data,
    std::size_t datagramSize,
    std::size_t& offset,
    std::uint64_t sequenceNumber,
    MessageView& message
) noexcept
{
    if (
        data == nullptr ||
        offset > datagramSize)
    {
        return false;
    }

    if (
        datagramSize -
            offset <
        MessageLengthSize)
    {
        return false;
    }

    const auto messageSize =
        static_cast<std::size_t>(
            readU16(
                data +
                    offset));

    offset +=
        MessageLengthSize;

    if (
        messageSize >
        datagramSize -
            offset)
    {
        return false;
    }

    message.data =
        data +
        offset;

    message.size =
        messageSize;

    message.sequenceNumber =
        sequenceNumber;

    offset +=
        messageSize;

    return true;
}

} // namespace llt::moldudp64