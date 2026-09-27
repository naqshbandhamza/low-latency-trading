#pragma once

#include <cstddef>
#include <cstdint>
#include <istream>
#include <array>

#include <functional>

#include "market_data/itch/ItchMessage.h"

namespace llt::itch
{

    struct ItchReplayStats
    {
        std::uint64_t recordsRead{0};

        std::uint64_t decodedMessages{0};
        std::uint64_t unsupportedMessages{0};
        std::uint64_t malformedMessages{0};

        std::uint64_t systemEvents{0};
        std::uint64_t stockDirectories{0};
        std::uint64_t addOrders{0};
        std::uint64_t addOrdersWithMpid{0};
        std::uint64_t orderExecutions{0};
        std::uint64_t orderExecutionsWithPrice{0};
        std::uint64_t orderCancels{0};
        std::uint64_t orderDeletes{0};
        std::uint64_t orderReplaces{0};
        std::uint64_t trades{0};
        std::uint64_t crossTrades{0};
        std::uint64_t brokenTrades{0};

        std::uint64_t regShoRestrictions{0};
        std::uint64_t mwcbDeclineLevels{0};
        std::uint64_t mwcbStatuses{0};
        std::uint64_t noiiMessages{0};

        std::uint64_t luldAuctionCollars{0};

        std::array<std::uint64_t, 256> unsupportedByType{};

        std::uint64_t stockTradingActions{0};

        std::uint64_t marketParticipantPositions{0};

        bool sessionComplete{false};
    };

    enum class ItchReplayStatus
    {
        Complete,
        IncompleteStream,
        StreamError
    };

    struct ItchReplayResult
    {
        ItchReplayStatus status{
            ItchReplayStatus::StreamError};

        ItchReplayStats stats{};
    };

    class ItchReplay
    {
        using MessageHandler =
            std::function<void(const ItchMessage &)>;

    public:
        static ItchReplayResult run(
            std::istream &input,
            const MessageHandler &handler = {});
    };

} // namespace llt::itch