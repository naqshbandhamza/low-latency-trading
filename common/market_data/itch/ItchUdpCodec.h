#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "market_data/itch/ItchUdpPacket.h"

namespace llt::itch
{

class ItchUdpCodec
{
public:
    static constexpr std::size_t HeaderSize =
        sizeof(std::uint64_t) +
        sizeof(std::uint16_t);

    static constexpr std::size_t MaxDatagramSize =
        HeaderSize +
        ItchUdpPacket::MaxPayloadSize;

    using Datagram =
        std::array<
            std::uint8_t,
            MaxDatagramSize>;

    [[nodiscard]]
    static bool encode(
        const ItchUdpPacket& packet,
        Datagram& output,
        std::size_t& outputSize
    ) noexcept;

    [[nodiscard]]
    static bool decode(
        const std::uint8_t* data,
        std::size_t size,
        ItchUdpPacket& packet
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

} // namespace llt::itch