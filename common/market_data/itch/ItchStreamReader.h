#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <istream>

namespace llt::itch
{

enum class ItchStreamReadStatus
{
    Message,
    EndOfSession,
    Incomplete,
    Error
};

struct ItchStreamReadResult
{
    ItchStreamReadStatus status{
        ItchStreamReadStatus::Error
    };

    const std::uint8_t* data{nullptr};
    std::size_t size{0};

    [[nodiscard]]
    bool empty() const noexcept
    {
        return size == 0;
    }
};

class ItchStreamReader
{
public:
    explicit ItchStreamReader(
        std::istream& stream
    ) noexcept;

    ItchStreamReadResult readNext();

private:
    static constexpr std::size_t MaxPayloadLength =
        65535;

    std::istream& stream_;

    std::array<
        std::uint8_t,
        MaxPayloadLength
    > payloadBuffer_{};

    static std::uint16_t readU16(
        const std::uint8_t* data
    ) noexcept;
};

} // namespace llt::itch