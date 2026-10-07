#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace llt::moldudp64
{

//
// One ITCH message together with its MoldUDP64
// message sequence number.
//
// This is deliberately independent of the UDP
// datagram that originally carried the message.
//
struct SequencedItchMessage
{
    static constexpr std::size_t MaxPayloadSize =
        2048;

    std::uint64_t sequence{0};

    std::uint16_t payloadSize{0};

    std::array<
        std::uint8_t,
        MaxPayloadSize
    > payload{};
};

} // namespace llt::moldudp64