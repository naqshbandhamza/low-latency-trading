#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

#include "market_data/MarketEventQueue.h"
#include "market_data/itch/ItchMarketState.h"
#include "market_data/itch/ItchQuotePublisher.h"
#include "market_data/itch/ItchReplay.h"

namespace
{

    const char *statusToString(
        llt::itch::ItchReplayStatus status) noexcept
    {
        switch (status)
        {
        case llt::itch::ItchReplayStatus::Complete:
            return "COMPLETE";

        case llt::itch::ItchReplayStatus::IncompleteStream:
            return "INCOMPLETE";

        case llt::itch::ItchReplayStatus::StreamError:
            return "STREAM ERROR";
        }

        return "UNKNOWN";
    }

    void printStats(
        const llt::itch::ItchReplayResult &result)
    {
        const auto &stats =
            result.stats;

        std::cout
            << "\n"
            << "========================================\n"
            << "          ITCH REPLAY SUMMARY\n"
            << "========================================\n"
            << "Status                 : "
            << statusToString(result.status) << '\n'
            << "Session complete       : "
            << (stats.sessionComplete ? "YES" : "NO") << '\n'
            << '\n'
            << "Records read           : "
            << stats.recordsRead << '\n'
            << "Decoded messages       : "
            << stats.decodedMessages << '\n'
            << "Unsupported messages   : "
            << stats.unsupportedMessages << '\n'
            << "Malformed messages     : "
            << stats.malformedMessages << '\n'
            << '\n'
            << "S  System Event        : "
            << stats.systemEvents << '\n'
            << "R  Stock Directory     : "
            << stats.stockDirectories << '\n'
            << "A  Add Order           : "
            << stats.addOrders << '\n'
            << "F  Add Order with MPID : "
            << stats.addOrdersWithMpid << '\n'
            << "E  Order Executed      : "
            << stats.orderExecutions << '\n'
            << "C  Executed With Price : "
            << stats.orderExecutionsWithPrice << '\n'
            << "X  Order Cancel        : "
            << stats.orderCancels << '\n'
            << "D  Order Delete        : "
            << stats.orderDeletes << '\n'
            << "U  Order Replace       : "
            << stats.orderReplaces << '\n'
            << "P  Trade               : "
            << stats.trades << '\n'
            << "Q  Cross Trade         : "
            << stats.crossTrades << '\n'
            << "B  Broken Trade        : "
            << stats.brokenTrades << '\n'
            << "H  Stock Trading Action: "
            << stats.stockTradingActions << '\n'
            << "L  Market Participant  : "
            << stats.marketParticipantPositions << '\n'
            << "Y  Reg SHO Restriction : "
            << stats.regShoRestrictions << '\n'
            << "V  MWCB Decline Level  : "
            << stats.mwcbDeclineLevels << '\n'
            << "W  MWCB Status         : "
            << stats.mwcbStatuses << '\n'
            << "I  NOII                : "
            << stats.noiiMessages << '\n'
            << "J  LULD Auction Collar : "
            << stats.luldAuctionCollars << '\n'
            << "========================================\n";
    }

    void printUnsupportedTypes(
        const llt::itch::ItchReplayStats &stats)
    {
        std::cout
            << "\n"
            << "Unsupported message types:\n";

        bool found = false;

        for (
            std::size_t i = 0;
            i < stats.unsupportedByType.size();
            ++i)
        {
            const auto count =
                stats.unsupportedByType[i];

            if (count == 0)
            {
                continue;
            }

            found = true;

            const auto type =
                static_cast<unsigned char>(i);

            if (type >= 32 && type <= 126)
            {
                std::cout
                    << "  "
                    << static_cast<char>(type)
                    << " : "
                    << count
                    << '\n';
            }
            else
            {
                std::cout
                    << "  0x"
                    << std::hex
                    << static_cast<unsigned int>(type)
                    << std::dec
                    << " : "
                    << count
                    << '\n';
            }
        }

        if (!found)
        {
            std::cout
                << "  None\n";
        }
    }

} // namespace

int main(
    int argc,
    char *argv[])
{
    if (argc != 2)
    {
        std::cerr
            << "Usage: "
            << argv[0]
            << " <ITCH BinaryFILE>\n";

        return 2;
    }

    const std::string filePath =
        argv[1];

    std::ifstream file(
        filePath,
        std::ios::binary);

    if (!file.is_open())
    {
        std::cerr
            << "Failed to open ITCH file: "
            << filePath
            << '\n';

        return 2;
    }

    std::cout
        << "ITCH replay starting\n"
        << "File: "
        << filePath
        << '\n';

    //
    // Normalized market-event SPSC.
    //
    llt::MarketEventQueue marketEventQueue;

    //
    // ITCH order-book / market-state reconstruction.
    //
    llt::itch::ItchMarketState marketState;

    //
    // Converts BBO changes into the existing normalized
    // Quote -> MarketEvent -> SPSC path.
    //
    llt::itch::ItchQuotePublisher publisher{
        marketState.instruments(),
        marketEventQueue};

    marketState.setBboChangeHandler(
        [&publisher](
            llt::market_data::InstrumentId instrumentId,
            llt::market_data::Timestamp timestamp,
            const llt::market_data::Bbo &bbo)
        {
            publisher.onBboChange(
                instrumentId,
                timestamp,
                bbo);
        });

    std::uint64_t consumedMarketEvents{0};

    const auto result =
        llt::itch::ItchReplay::run(
            file,
            [&marketState,
             &marketEventQueue,
             &consumedMarketEvents](
                const llt::itch::ItchMessage &message)
            {
                //
                // Decode-derived message enters the
                // reconstructed market state.
                //
                marketState.onMessage(
                    message);

                //
                // TEMPORARY replay consumer.
                //
                // The SPSC has capacity 4096. During a
                // 368M-message historical replay we cannot
                // leave published Quotes sitting in it.
                //
                // For now we drain immediately so that we
                // can validate:
                //
                // ITCH -> Book -> BBO -> Quote -> SPSC -> pop
                //
                // Later this becomes the dedicated strategy
                // / consumer thread.
                //
                while (true)
                {
                    auto event =
                        marketEventQueue.pop();

                    if (!event.has_value())
                    {
                        break;
                    }

                    ++consumedMarketEvents;
                }
            });

    printStats(
        result);

    printUnsupportedTypes(
        result.stats);

    std::cout
        << "\n"
        << "Market state\n"
        << "----------------------------------------\n"
        << "Instruments                 : "
        << marketState.instruments().size()
        << '\n'
        << "Active orders               : "
        << marketState.orders().size()
        << '\n'
        << '\n'
        << "Unknown H instruments       : "
        << marketState.unknownTradingActionInstruments()
        << '\n'
        << "Unknown Y instruments       : "
        << marketState.unknownRegShoInstruments()
        << '\n'
        << '\n'
        << "Duplicate A/F order IDs     : "
        << marketState.duplicateAddOrders()
        << '\n'
        << "Unknown E orders            : "
        << marketState.unknownOrderExecutions()
        << '\n'
        << "Over-executed E orders      : "
        << marketState.overExecutedOrders()
        << '\n'
        << "Unknown C orders            : "
        << marketState.unknownOrderExecutionsWithPrice()
        << '\n'
        << "Over-executed C orders      : "
        << marketState.overExecutedOrdersWithPrice()
        << '\n'
        << "Unknown X orders            : "
        << marketState.unknownOrderCancels()
        << '\n'
        << "Over-cancelled X orders     : "
        << marketState.overCancelledOrders()
        << '\n'
        << "Unknown D orders            : "
        << marketState.unknownOrderDeletes()
        << '\n'
        << "Unknown U orders            : "
        << marketState.unknownOrderReplaces()
        << '\n'
        << "Duplicate U new IDs         : "
        << marketState.duplicateReplacementOrderIds()
        << '\n'
        << '\n'
        << "Missing books               : "
        << marketState.missingBooks()
        << '\n'
        << "Failed book reductions      : "
        << marketState.failedBookReductions()
        << '\n'
        << "Failed book removals        : "
        << marketState.failedBookRemovals()
        << '\n'
        << "Books                       : "
        << marketState.books().size()
        << '\n';

    std::cout
        << "\n"
        << "Normalized publication\n"
        << "----------------------------------------\n"
        << "Published quotes            : "
        << publisher.publishedQuotes()
        << '\n'
        << "Dropped quotes              : "
        << publisher.droppedQuotes()
        << '\n'
        << "Unknown instruments         : "
        << publisher.unknownInstruments()
        << '\n'
        << "Consumed market events      : "
        << consumedMarketEvents
        << '\n'
        << "Events remaining in SPSC    : "
        << marketEventQueue.size()
        << '\n';

    std::cout
        << "\n"
        << "Unknown instrument samples\n"
        << "----------------------------------------\n";

    const auto sampleCount =
        publisher.unknownInstrumentSampleCount();

    if (sampleCount == 0)
    {
        std::cout
            << "None\n";
    }
    else
    {
        const auto &samples =
            publisher.unknownInstrumentSamples();

        for (
            std::size_t i = 0;
            i < sampleCount;
            ++i)
        {
            const auto &sample =
                samples[i];

            std::cout
                << "Instrument ID "
                << sample.instrumentId
                << " | first timestamp "
                << sample.timestamp
                << " | occurrences "
                << sample.occurrences
                << '\n';
        }
    }

    switch (result.status)
    {
    case llt::itch::ItchReplayStatus::Complete:
        return 0;

    case llt::itch::ItchReplayStatus::IncompleteStream:
        std::cerr
            << "Replay failed: "
            << "BinaryFILE ended without a "
            << "complete session terminator.\n";

        return 1;

    case llt::itch::ItchReplayStatus::StreamError:
        std::cerr
            << "Replay failed: "
            << "I/O error while reading file.\n";

        return 1;
    }

    return 1;
}