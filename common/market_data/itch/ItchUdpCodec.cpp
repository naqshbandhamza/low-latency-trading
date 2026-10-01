#include "market_data/itch/ItchUdpCodec.h"

#include <algorithm>

namespace llt::itch
{

void ItchUdpCodec::writeU16(
    std::uint8_t* destination,
    std::uint16_t value
) noexcept
{
    destination[0] =
        static_cast<std::uint8_t>(
            (value >> 8) & 0xFF);

    destination[1] =
        static_cast<std::uint8_t>(
            value & 0xFF);
}


void ItchUdpCodec::writeU64(
    std::uint8_t* destination,
    std::uint64_t value
) noexcept
{
    for (
        std::size_t i = 0;
        i < sizeof(std::uint64_t);
        ++i)
    {
        const auto shift =
            static_cast<unsigned>(
                (sizeof(std::uint64_t) - 1 - i) *
                8);

        destination[i] =
            static_cast<std::uint8_t>(
                (value >> shift) &
                0xFF);
    }
}


std::uint16_t ItchUdpCodec::readU16(
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


std::uint64_t ItchUdpCodec::readU64(
    const std::uint8_t* source
) noexcept
{
    std::uint64_t value{0};

    for (
        std::size_t i = 0;
        i < sizeof(std::uint64_t);
        ++i)
    {
        value =
            (value << 8) |
            static_cast<std::uint64_t>(
                source[i]);
    }

    return value;
}


bool ItchUdpCodec::encode(
    const ItchUdpPacket& packet,
    Datagram& output,
    std::size_t& outputSize
) noexcept
{
    outputSize = 0;

    if (
        packet.payloadSize >
        ItchUdpPacket::MaxPayloadSize)
    {
        return false;
    }

    writeU64(
        output.data(),
        packet.sequence);

    writeU16(
        output.data() +
            sizeof(std::uint64_t),
        packet.payloadSize);

    if (packet.payloadSize > 0)
    {
        std::copy_n(
            packet.payload.data(),
            packet.payloadSize,
            output.data() +
                HeaderSize);
    }

    outputSize =
        HeaderSize +
        packet.payloadSize;

    return true;
}


bool ItchUdpCodec::decode(
    const std::uint8_t* data,
    std::size_t size,
    ItchUdpPacket& packet
) noexcept
{
    if (data == nullptr)
    {
        return false;
    }

    //
    // We cannot even inspect the payload length
    // unless the complete transport header exists.
    //
    if (size < HeaderSize)
    {
        return false;
    }

    const auto sequence =
        readU64(data);

    const auto payloadSize =
        readU16(
            data +
            sizeof(std::uint64_t));

    if (
        payloadSize >
        ItchUdpPacket::MaxPayloadSize)
    {
        return false;
    }

    const std::size_t expectedSize =
        HeaderSize +
        static_cast<std::size_t>(
            payloadSize);

    //
    // Require exact framing.
    //
    // Reject both:
    //
    // truncated datagrams
    // trailing unexpected bytes
    //
    if (size != expectedSize)
    {
        return false;
    }

    packet.sequence =
        sequence;

    packet.payloadSize =
        payloadSize;

    if (payloadSize > 0)
    {
        std::copy_n(
            data + HeaderSize,
            payloadSize,
            packet.payload.data());
    }

    return true;
}

} // namespace llt::itch