#include "market_data/itch/ItchStreamReader.h"

#include <array>

namespace llt::itch
{

    ItchStreamReader::ItchStreamReader(
        std::istream &stream) noexcept
        : stream_(stream)
    {
    }

    std::uint16_t ItchStreamReader::readU16(
        const std::uint8_t *data) noexcept
    {
        return static_cast<std::uint16_t>(
            (
                static_cast<std::uint16_t>(
                    data[0])
                << 8) |
            static_cast<std::uint16_t>(
                data[1]));
    }

    ItchStreamReadResult
    ItchStreamReader::readNext()
    {
        std::array<std::uint8_t, 2>
            lengthBytes{};

        stream_.read(
            reinterpret_cast<char *>(
                lengthBytes.data()),
            static_cast<std::streamsize>(
                lengthBytes.size()));

        const auto lengthBytesRead =
            stream_.gcount();

        if (lengthBytesRead == 0)
        {
            if (stream_.bad())
            {
                return {
                    ItchStreamReadStatus::Error,
                    nullptr,
                    0};
            }

            return {
                ItchStreamReadStatus::Incomplete,
                nullptr,
                0};
        }

        if (
            lengthBytesRead !=
            static_cast<std::streamsize>(
                lengthBytes.size()))
        {
            return {
                ItchStreamReadStatus::Incomplete,
                nullptr,
                0};
        }

        const std::uint16_t payloadLength =
            readU16(
                lengthBytes.data());

        if (payloadLength == 0)
        {
            return {
                ItchStreamReadStatus::EndOfSession,
                nullptr,
                0};
        }

        stream_.read(
            reinterpret_cast<char *>(
                payloadBuffer_.data()),
            static_cast<std::streamsize>(
                payloadLength));

        if (
            stream_.gcount() !=
            static_cast<std::streamsize>(
                payloadLength))
        {
            if (stream_.bad())
            {
                return {
                    ItchStreamReadStatus::Error,
                    nullptr,
                    0};
            }

            return {
                ItchStreamReadStatus::Incomplete,
                nullptr,
                0};
        }

        // return {
        //     ItchStreamReadStatus::Message,
        //     std::span<const std::uint8_t>(
        //         payloadBuffer_.data(),
        //         payloadLength)};
        return {
            ItchStreamReadStatus::Message,
            payloadBuffer_.data(),
            payloadLength};
    }

} // namespace llt::itch