#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace llt::itch
{

struct ItchUdpPacket
{
    //
    // Keep the datagram comfortably below the
    // practical UDP payload limit.
    //
    // Individual ITCH messages are much smaller,
    // but this gives the transport room without
    // coupling it to a specific ITCH message type.
    //
    static constexpr std::size_t MaxPayloadSize =
        2048;

    std::uint64_t sequence{0};

    std::uint16_t payloadSize{0};

    std::array<
        std::uint8_t,
        MaxPayloadSize
    > payload{};
};

} // namespace llt::itch