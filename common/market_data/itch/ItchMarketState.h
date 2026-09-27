#pragma once

#include <iostream>
#include <cstdint>
#include "market_data/itch/ItchMessage.h"
#include "market_data/itch/ItchNormalizer.h"
#include "market_data/normalized/InstrumentStore.h"

namespace llt::itch
{

    class ItchMarketState
    {
    public:
        void onMessage(
            const ItchMessage &message) noexcept
        {
            std::visit(
                [this](const auto &msg)
                {
                    handle(msg);
                },
                message);
        }

        [[nodiscard]]
        market_data::InstrumentStore &
        instruments() noexcept
        {
            return instruments_;
        }

        [[nodiscard]]
        const market_data::InstrumentStore &
        instruments() const noexcept
        {
            return instruments_;
        }

        [[nodiscard]]
        std::uint64_t unknownTradingActionInstruments() const noexcept
        {
            return unknownTradingActionInstruments_;
        }

        [[nodiscard]]
        std::uint64_t unknownRegShoInstruments() const noexcept
        {
            return unknownRegShoInstruments_;
        }

    private:
        void handle(
            const StockDirectoryMessage &message) noexcept
        {
            const auto instrument =
                ItchNormalizer::normalize(message);

            instruments_.add(instrument);
        }

        void handle(
            const StockTradingActionMessage& message
        ) noexcept
        {
            auto* instrument =
                instruments_.find(
                    message.stockLocate
                );
        
            if (instrument == nullptr)
            {
                ++unknownTradingActionInstruments_;
                return;
            }
        
            instrument->tradingState =
                ItchNormalizer::normalizeTradingState(
                    message
                );
        }

        void handle(
            const RegShoRestrictionMessage& message
        ) noexcept
        {
            auto* instrument =
                instruments_.find(
                    message.stockLocate
                );
        
            if (instrument == nullptr)
            {
                ++unknownRegShoInstruments_;
                return;
            }
        
            instrument->regShoState =
                ItchNormalizer::normalizeRegShoState(
                    message
                );
        }

        // void handle(
        //     const StockDirectoryMessage& message
        // ) noexcept
        // {
        //     const auto instrument =
        //         ItchNormalizer::normalize(message);

        //     if (!instruments_.add(instrument))
        //     {
        //         const auto* existing =
        //             instruments_.find(instrument.id);

        //         std::cerr
        //             << "\n[DUPLICATE STOCK DIRECTORY]\n"
        //             << "Stock Locate : "
        //             << instrument.id
        //             << '\n'
        //             << "New Symbol   : "
        //             << instrument.symbolView()
        //             << '\n';

        //         if (existing != nullptr)
        //         {
        //             std::cerr
        //                 << "Old Symbol   : "
        //                 << existing->symbolView()
        //                 << '\n';
        //         }
        //     }
        // }

        // All other ITCH messages are intentionally
        // ignored at this stage.
        template <typename T>
        void handle(
            const T &) noexcept
        {
        }

        std::uint64_t unknownTradingActionInstruments_{0};
        std::uint64_t unknownRegShoInstruments_{0};

        market_data::InstrumentStore instruments_;
    };

} // namespace llt::itch