#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace llt::moldudp64
{

inline constexpr std::size_t
    SessionSize = 10;

inline constexpr std::size_t
    HeaderSize = 20;

//
// Keep the UDP payload comfortably below a normal
// Ethernet MTU.
//
// 1400 gives us room beneath the typical 1472-byte
// maximum UDP payload for a 1500-byte Ethernet MTU.
//
inline constexpr std::size_t
    MaxDatagramSize = 1400;

using Session =
    std::array<char, SessionSize>;

using Datagram =
    std::array<
        std::uint8_t,
        MaxDatagramSize>;


struct ReceivedMoldDatagram
{
    Datagram bytes{};
    std::size_t size{0};
};

struct Header
{
    Session session{};

    std::uint64_t
        sequenceNumber{0};

    std::uint16_t
        messageCount{0};
};

struct MessageView
{
    const std::uint8_t*
        data{nullptr};

    std::size_t
        size{0};

    std::uint64_t
        sequenceNumber{0};
};

} // namespace llt::moldudp64