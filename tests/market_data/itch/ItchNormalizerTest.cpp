#include <catch2/catch_test_macros.hpp>

#include "market_data/itch/ItchNormalizer.h"

#include "market_data/itch/ItchMarketState.h"

using namespace llt;


TEST_CASE(
    "ITCH normalizer converts Stock Directory to Instrument"
)
{
    itch::StockDirectoryMessage message;

    message.stockLocate = 42;

    message.stock = {
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' '
    };

    const auto instrument =
        itch::ItchNormalizer::normalize(message);

    REQUIRE(instrument.id == 42);

    REQUIRE(
        instrument.symbolView() ==
        "AAPL"
    );

    REQUIRE(
        instrument.tradingState ==
        market_data::TradingState::Unknown
    );
}


TEST_CASE(
    "ITCH normalizer preserves eight character symbol"
)
{
    itch::StockDirectoryMessage message;

    message.stockLocate = 123;

    message.stock = {
        'A', 'B', 'C', 'D',
        'E', 'F', 'G', 'H'
    };

    const auto instrument =
        itch::ItchNormalizer::normalize(message);

    REQUIRE(
        instrument.symbolView() ==
        "ABCDEFGH"
    );
}


TEST_CASE(
    "ITCH normalizer preserves maximum instrument id"
)
{
    itch::StockDirectoryMessage message;

    message.stockLocate =
        UINT16_MAX;

    message.stock = {
        'M', 'A', 'X', ' ',
        ' ', ' ', ' ', ' '
    };

    const auto instrument =
        itch::ItchNormalizer::normalize(message);

    REQUIRE(
        instrument.id ==
        UINT16_MAX
    );
}




TEST_CASE(
    "ITCH market state creates instrument from Stock Directory"
)
{
    itch::ItchMarketState state;

    itch::StockDirectoryMessage message;

    message.stockLocate = 42;

    message.stock = {
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' '
    };

    itch::ItchMessage event{
        message
    };

    state.onMessage(event);

    REQUIRE(
        state.instruments().size() == 1
    );

    const auto* instrument =
        state.instruments().find(42);

    REQUIRE(instrument != nullptr);

    REQUIRE(
        instrument->symbolView() ==
        "AAPL"
    );
}


TEST_CASE(
    "ITCH market state stores multiple instruments"
)
{
    itch::ItchMarketState state;

    itch::StockDirectoryMessage aapl;
    aapl.stockLocate = 10;

    aapl.stock = {
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' '
    };

    itch::StockDirectoryMessage msft;
    msft.stockLocate = 20;

    msft.stock = {
        'M', 'S', 'F', 'T',
        ' ', ' ', ' ', ' '
    };

    state.onMessage(
        itch::ItchMessage{aapl}
    );

    state.onMessage(
        itch::ItchMessage{msft}
    );

    REQUIRE(
        state.instruments().size() == 2
    );

    REQUIRE(
        state.instruments()
            .find(10)
            ->symbolView() ==
        "AAPL"
    );

    REQUIRE(
        state.instruments()
            .find(20)
            ->symbolView() ==
        "MSFT"
    );
}


TEST_CASE(
    "ITCH market state ignores non reference messages"
)
{
    itch::ItchMarketState state;

    itch::AddOrderMessage message{};

    state.onMessage(
        itch::ItchMessage{message}
    );

    REQUIRE(
        state.instruments().size() == 0
    );
}


TEST_CASE(
    "ITCH market state handles duplicate Stock Directory idempotently"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::StockDirectoryMessage message{};

    message.stockLocate = 1335;

    message.stock = {
        'C', 'F', 'G', '-', 'D',
        ' ', ' ', ' '
    };

    state.onMessage(
        llt::itch::ItchMessage{message}
    );

    state.onMessage(
        llt::itch::ItchMessage{message}
    );

    REQUIRE(
        state.instruments().size() == 1
    );

    const auto* instrument =
        state.instruments().find(1335);

    REQUIRE(instrument != nullptr);

    REQUIRE(
        instrument->symbolView() ==
        "CFG-D"
    );
}


TEST_CASE(
    "ITCH normalizer converts trading action states"
)
{
    llt::itch::StockTradingActionMessage message{};

    message.tradingState = 'H';

    REQUIRE(
        llt::itch::ItchNormalizer::
            normalizeTradingState(message) ==
        llt::market_data::TradingState::Halted
    );

    message.tradingState = 'P';

    REQUIRE(
        llt::itch::ItchNormalizer::
            normalizeTradingState(message) ==
        llt::market_data::TradingState::Paused
    );

    message.tradingState = 'Q';

    REQUIRE(
        llt::itch::ItchNormalizer::
            normalizeTradingState(message) ==
        llt::market_data::TradingState::QuotationOnly
    );

    message.tradingState = 'T';

    REQUIRE(
        llt::itch::ItchNormalizer::
            normalizeTradingState(message) ==
        llt::market_data::TradingState::Trading
    );
}


TEST_CASE(
    "ITCH normalizer maps unknown trading state to Unknown"
)
{
    llt::itch::StockTradingActionMessage message{};

    message.tradingState = '?';

    REQUIRE(
        llt::itch::ItchNormalizer::
            normalizeTradingState(message) ==
        llt::market_data::TradingState::Unknown
    );
}

TEST_CASE(
    "ITCH market state updates instrument trading state"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::StockDirectoryMessage directory{};

    directory.stockLocate = 42;

    directory.stock = {
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' '
    };

    state.onMessage(
        llt::itch::ItchMessage{
            directory
        }
    );

    const auto* before =
        state.instruments().find(42);

    REQUIRE(before != nullptr);

    REQUIRE(
        before->tradingState ==
        llt::market_data::TradingState::Unknown
    );

    llt::itch::StockTradingActionMessage action{};

    action.stockLocate = 42;
    action.tradingState = 'H';

    state.onMessage(
        llt::itch::ItchMessage{
            action
        }
    );

    const auto* after =
        state.instruments().find(42);

    REQUIRE(after != nullptr);

    REQUIRE(
        after->tradingState ==
        llt::market_data::TradingState::Halted
    );
}

TEST_CASE(
    "ITCH market state applies trading state transitions"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::StockDirectoryMessage directory{};

    directory.stockLocate = 42;

    directory.stock = {
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' '
    };

    state.onMessage(
        llt::itch::ItchMessage{
            directory
        }
    );

    llt::itch::StockTradingActionMessage action{};

    action.stockLocate = 42;

    action.tradingState = 'H';

    state.onMessage(
        llt::itch::ItchMessage{
            action
        }
    );

    REQUIRE(
        state.instruments()
            .find(42)
            ->tradingState ==
        llt::market_data::TradingState::Halted
    );

    action.tradingState = 'T';

    state.onMessage(
        llt::itch::ItchMessage{
            action
        }
    );

    REQUIRE(
        state.instruments()
            .find(42)
            ->tradingState ==
        llt::market_data::TradingState::Trading
    );
}

TEST_CASE(
    "ITCH market state ignores trading action for unknown instrument"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::StockTradingActionMessage action{};

    action.stockLocate = 999;
    action.tradingState = 'H';

    REQUIRE(
        state.instruments().size() == 0
    );

    state.onMessage(
        llt::itch::ItchMessage{
            action
        }
    );

    REQUIRE(
        state.instruments().size() == 0
    );

    REQUIRE(
        state.instruments().find(999) ==
        nullptr
    );

    REQUIRE(
        state.unknownTradingActionInstruments() == 1
    );
}


TEST_CASE(
    "ITCH normalizer converts Reg SHO restriction states"
)
{
    llt::itch::RegShoRestrictionMessage message{};

    message.regShoAction = '0';

    REQUIRE(
        llt::itch::ItchNormalizer::
            normalizeRegShoState(message) ==
        llt::market_data::RegShoState::NoRestriction
    );

    message.regShoAction = '1';

    REQUIRE(
        llt::itch::ItchNormalizer::
            normalizeRegShoState(message) ==
        llt::market_data::RegShoState::RestrictionInEffect
    );

    message.regShoAction = '2';

    REQUIRE(
        llt::itch::ItchNormalizer::
            normalizeRegShoState(message) ==
        llt::market_data::RegShoState::RestrictionRemains
    );
}


TEST_CASE(
    "ITCH normalizer maps unknown Reg SHO state to Unknown"
)
{
    llt::itch::RegShoRestrictionMessage message{};

    message.regShoAction = '?';

    REQUIRE(
        llt::itch::ItchNormalizer::
            normalizeRegShoState(message) ==
        llt::market_data::RegShoState::Unknown
    );
}


TEST_CASE(
    "ITCH market state updates instrument Reg SHO state"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::StockDirectoryMessage directory{};

    directory.stockLocate = 42;

    directory.stock = {
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' '
    };

    state.onMessage(
        llt::itch::ItchMessage{directory}
    );

    llt::itch::RegShoRestrictionMessage restriction{};

    restriction.stockLocate = 42;
    restriction.regShoAction = '1';

    state.onMessage(
        llt::itch::ItchMessage{restriction}
    );

    const auto* instrument =
        state.instruments().find(42);

    REQUIRE(instrument != nullptr);

    REQUIRE(
        instrument->regShoState ==
        llt::market_data::RegShoState::RestrictionInEffect
    );
}


TEST_CASE(
    "ITCH market state applies Reg SHO state transitions"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::StockDirectoryMessage directory{};

    directory.stockLocate = 42;

    directory.stock = {
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' '
    };

    state.onMessage(
        llt::itch::ItchMessage{directory}
    );

    llt::itch::RegShoRestrictionMessage restriction{};

    restriction.stockLocate = 42;
    restriction.regShoAction = '1';

    state.onMessage(
        llt::itch::ItchMessage{restriction}
    );

    REQUIRE(
        state.instruments()
            .find(42)
            ->regShoState ==
        llt::market_data::RegShoState::RestrictionInEffect
    );

    restriction.regShoAction = '2';

    state.onMessage(
        llt::itch::ItchMessage{restriction}
    );

    REQUIRE(
        state.instruments()
            .find(42)
            ->regShoState ==
        llt::market_data::RegShoState::RestrictionRemains
    );
}


TEST_CASE(
    "ITCH market state ignores Reg SHO update for unknown instrument"
)
{
    llt::itch::ItchMarketState state;

    llt::itch::RegShoRestrictionMessage restriction{};

    restriction.stockLocate = 999;
    restriction.regShoAction = '1';

    state.onMessage(
        llt::itch::ItchMessage{restriction}
    );

    REQUIRE(
        state.instruments().size() == 0
    );

    REQUIRE(
        state.instruments().find(999) ==
        nullptr
    );

    REQUIRE(
        state.unknownRegShoInstruments() == 1
    );
}

