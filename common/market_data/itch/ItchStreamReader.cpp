#include "market_data/itch/ItchStreamReader.h"
#include <utility>
#include <array>

namespace llt::itch
{

ItchStreamReader::ItchStreamReader(
    std::istream& stream
) noexcept
    : stream_(stream)
{
}

std::uint16_t ItchStreamReader::readU16(
    const std::uint8_t* data
) noexcept
{
    return
        static_cast<std::uint16_t>(
            (
                static_cast<std::uint16_t>(
                    data[0]
                ) << 8
            ) |
            static_cast<std::uint16_t>(
                data[1]
            )
        );
}

ItchStreamReadResult
ItchStreamReader::readNext()
{
    std::array<std::uint8_t, 2>
        lengthBytes{};

    stream_.read(
        reinterpret_cast<char*>(
            lengthBytes.data()
        ),
        lengthBytes.size()
    );

    const auto lengthBytesRead =
        stream_.gcount();

        if (lengthBytesRead == 0)
        {
            if (stream_.bad())
            {
                return {
                    ItchStreamReadStatus::Error,
                    {}
                };
            }
        
            return {
                ItchStreamReadStatus::Incomplete,
                {}
            };
        }

    if (
        lengthBytesRead !=
        static_cast<std::streamsize>(
            lengthBytes.size()
        )
    )
    {
        return {
            ItchStreamReadStatus::Incomplete,
            {}
        };
    }

    const std::uint16_t payloadLength =
        readU16(
            lengthBytes.data()
        );

    if (payloadLength == 0)
    {
        return {
            ItchStreamReadStatus::EndOfSession,
            {}
        };
    }

    std::vector<std::uint8_t>
        payload(payloadLength);

    stream_.read(
        reinterpret_cast<char*>(
            payload.data()
        ),
        static_cast<std::streamsize>(
            payload.size()
        )
    );

    if (
        stream_.gcount() !=
        static_cast<std::streamsize>(
            payload.size()
        )
    )
    {
        if (stream_.bad())
        {
            return {
                ItchStreamReadStatus::Error,
                {}
            };
        }
    
        return {
            ItchStreamReadStatus::Incomplete,
            {}
        };
    }

    return {
        ItchStreamReadStatus::Message,
        std::move(payload)
    };
}

} // namespace llt::itch