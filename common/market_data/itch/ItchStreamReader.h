#pragma once

#include <cstddef>
#include <cstdint>
#include <istream>
#include <optional>
#include <vector>

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

    std::vector<std::uint8_t> payload{};
};

class ItchStreamReader
{
public:
    explicit ItchStreamReader(
        std::istream& stream
    ) noexcept;

    ItchStreamReadResult readNext();

private:
    std::istream& stream_;

    static std::uint16_t readU16(
        const std::uint8_t* data
    ) noexcept;
};

} // namespace llt::itch