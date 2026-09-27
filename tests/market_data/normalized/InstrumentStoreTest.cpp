#include <catch2/catch_test_macros.hpp>

#include "market_data/normalized/InstrumentStore.h"

using namespace llt::market_data;


TEST_CASE(
    "InstrumentStore starts empty"
)
{
    InstrumentStore store;

    REQUIRE(store.size() == 0);

    REQUIRE_FALSE(
        store.contains(42)
    );

    REQUIRE(
        store.find(42) == nullptr
    );
}


TEST_CASE(
    "InstrumentStore adds instrument"
)
{
    InstrumentStore store;

    Instrument instrument;

    instrument.id = 42;

    instrument.symbol = {
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' '
    };

    REQUIRE(
        store.add(instrument)
    );

    REQUIRE(store.size() == 1);
    REQUIRE(store.contains(42));

    const auto* result =
        store.find(42);

    REQUIRE(result != nullptr);

    REQUIRE(
        result->id == 42
    );

    REQUIRE(
        result->symbolView() == "AAPL"
    );
}


TEST_CASE(
    "InstrumentStore rejects duplicate instrument"
)
{
    InstrumentStore store;

    Instrument instrument;

    instrument.id = 42;

    instrument.symbol = {
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' '
    };

    REQUIRE(
        store.add(instrument)
    );

    REQUIRE_FALSE(
        store.add(instrument)
    );

    REQUIRE(
        store.size() == 1
    );
}


TEST_CASE(
    "InstrumentStore stores instruments independently"
)
{
    InstrumentStore store;

    Instrument aapl;

    aapl.id = 10;

    aapl.symbol = {
        'A', 'A', 'P', 'L',
        ' ', ' ', ' ', ' '
    };

    Instrument msft;

    msft.id = 20;

    msft.symbol = {
        'M', 'S', 'F', 'T',
        ' ', ' ', ' ', ' '
    };

    REQUIRE(store.add(aapl));
    REQUIRE(store.add(msft));

    REQUIRE(store.size() == 2);

    REQUIRE(
        store.find(10)->symbolView() ==
        "AAPL"
    );

    REQUIRE(
        store.find(20)->symbolView() ==
        "MSFT"
    );
}


TEST_CASE(
    "InstrumentStore supports maximum instrument id"
)
{
    InstrumentStore store;

    Instrument instrument;

    instrument.id =
        UINT16_MAX;

    instrument.symbol = {
        'M', 'A', 'X', ' ',
        ' ', ' ', ' ', ' '
    };

    REQUIRE(
        store.add(instrument)
    );

    REQUIRE(
        store.contains(UINT16_MAX)
    );

    REQUIRE(
        store.find(UINT16_MAX) !=
        nullptr
    );
}


TEST_CASE(
    "InstrumentStore mutable lookup updates state"
)
{
    InstrumentStore store;

    Instrument instrument;

    instrument.id = 42;

    REQUIRE(
        store.add(instrument)
    );

    auto* result =
        store.find(42);

    REQUIRE(result != nullptr);

    result->tradingState =
        TradingState::Trading;

    REQUIRE(
        store.find(42)->tradingState ==
        TradingState::Trading
    );
}


TEST_CASE(
    "InstrumentStore keeps missing ids absent"
)
{
    InstrumentStore store;

    Instrument instrument;
    instrument.id = 100;

    REQUIRE(
        store.add(instrument)
    );

    REQUIRE(store.contains(100));

    REQUIRE_FALSE(
        store.contains(101)
    );

    REQUIRE(
        store.find(101) == nullptr
    );
}