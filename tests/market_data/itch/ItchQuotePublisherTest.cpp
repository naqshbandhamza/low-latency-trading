#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <string_view>
#include <variant>

#include "market_data/itch/ItchQuotePublisher.h"
#include "market_data/normalized/InstrumentStore.h"

namespace
{

void addInstrument(
    llt::market_data::InstrumentStore &store,
    llt::market_data::InstrumentId id,
    std::string_view symbol)
{
    llt::market_data::Instrument instrument{};

    instrument.id = id;

    instrument.symbol.fill(' ');

    const auto length =
        std::min(
            symbol.size(),
            instrument.symbol.size());

    std::copy_n(
        symbol.begin(),
        length,
        instrument.symbol.begin());

    REQUIRE(
        store.add(instrument));
}


llt::market_data::Bbo makeTwoSidedBbo(
    llt::market_data::Price bidPrice = 1000000,
    llt::market_data::Quantity bidQuantity = 500,
    llt::market_data::Price askPrice = 1000100,
    llt::market_data::Quantity askQuantity = 300)
{
    llt::market_data::Bbo bbo{};

    bbo.hasBid = true;
    bbo.bidPrice = bidPrice;
    bbo.bidQuantity = bidQuantity;

    bbo.hasAsk = true;
    bbo.askPrice = askPrice;
    bbo.askQuantity = askQuantity;

    return bbo;
}

} // namespace


TEST_CASE(
    "ITCH quote publisher publishes normalized quote")
{
    llt::market_data::InstrumentStore instruments;

    addInstrument(
        instruments,
        42,
        "AAPL");

    llt::MarketEventQueue queue;

    llt::itch::ItchQuotePublisher publisher{
        instruments,
        queue};

    const auto bbo =
        makeTwoSidedBbo();

    publisher.onBboChange(
        42,
        123456,
        bbo);

    REQUIRE(queue.size() == 1);

    REQUIRE(
        publisher.publishedQuotes() ==
        1);

    REQUIRE(
        publisher.droppedQuotes() ==
        0);

    REQUIRE(
        publisher.unknownInstruments() ==
        0);

    auto event =
        queue.pop();

    REQUIRE(event.has_value());

    REQUIRE(
        std::holds_alternative<llt::Quote>(
            *event));

    const auto &quote =
        std::get<llt::Quote>(*event);

    REQUIRE(
        quote.instrument() ==
        llt::Instrument{"AAPL"});

    REQUIRE(
        quote.sequence() ==
        llt::SequenceNumber{0});

    REQUIRE(
        quote.timestamp() ==
        llt::Timestamp{123456});

    REQUIRE(
        quote.bid().price() ==
        llt::Price{1000000});

    REQUIRE(
        quote.bid().quantity() ==
        llt::Quantity{500});

    REQUIRE(
        quote.ask().price() ==
        llt::Price{1000100});

    REQUIRE(
        quote.ask().quantity() ==
        llt::Quantity{300});
}


TEST_CASE(
    "ITCH quote publisher rejects unknown instrument")
{
    llt::market_data::InstrumentStore instruments;

    llt::MarketEventQueue queue;

    llt::itch::ItchQuotePublisher publisher{
        instruments,
        queue};

    const auto bbo =
        makeTwoSidedBbo();

    publisher.onBboChange(
        42,
        123456,
        bbo);

    REQUIRE(queue.size() == 0);

    REQUIRE(
        publisher.publishedQuotes() ==
        0);

    REQUIRE(
        publisher.droppedQuotes() ==
        0);

    REQUIRE(
        publisher.unknownInstruments() ==
        1);
}


TEST_CASE(
    "ITCH quote publisher does not publish bid only BBO")
{
    llt::market_data::InstrumentStore instruments;

    addInstrument(
        instruments,
        42,
        "AAPL");

    llt::MarketEventQueue queue;

    llt::itch::ItchQuotePublisher publisher{
        instruments,
        queue};

    llt::market_data::Bbo bbo{};

    bbo.hasBid = true;
    bbo.bidPrice = 1000000;
    bbo.bidQuantity = 500;

    publisher.onBboChange(
        42,
        123456,
        bbo);

    REQUIRE(queue.size() == 0);

    REQUIRE(
        publisher.publishedQuotes() ==
        0);

    REQUIRE(
        publisher.droppedQuotes() ==
        0);

    REQUIRE(
        publisher.unknownInstruments() ==
        0);
}


TEST_CASE(
    "ITCH quote publisher does not publish ask only BBO")
{
    llt::market_data::InstrumentStore instruments;

    addInstrument(
        instruments,
        42,
        "AAPL");

    llt::MarketEventQueue queue;

    llt::itch::ItchQuotePublisher publisher{
        instruments,
        queue};

    llt::market_data::Bbo bbo{};

    bbo.hasAsk = true;
    bbo.askPrice = 1000100;
    bbo.askQuantity = 300;

    publisher.onBboChange(
        42,
        123456,
        bbo);

    REQUIRE(queue.size() == 0);

    REQUIRE(
        publisher.publishedQuotes() ==
        0);

    REQUIRE(
        publisher.droppedQuotes() ==
        0);

    REQUIRE(
        publisher.unknownInstruments() ==
        0);
}


TEST_CASE(
    "ITCH quote publisher assigns increasing sequence numbers")
{
    llt::market_data::InstrumentStore instruments;

    addInstrument(
        instruments,
        42,
        "AAPL");

    llt::MarketEventQueue queue;

    llt::itch::ItchQuotePublisher publisher{
        instruments,
        queue};

    const auto first =
        makeTwoSidedBbo();

    auto second =
        first;

    second.bidQuantity =
        600;

    publisher.onBboChange(
        42,
        1000,
        first);

    publisher.onBboChange(
        42,
        2000,
        second);

    REQUIRE(queue.size() == 2);

    REQUIRE(
        publisher.publishedQuotes() ==
        2);

    REQUIRE(
        publisher.droppedQuotes() ==
        0);

    auto firstEvent =
        queue.pop();

    auto secondEvent =
        queue.pop();

    REQUIRE(firstEvent.has_value());
    REQUIRE(secondEvent.has_value());

    REQUIRE(
        std::holds_alternative<llt::Quote>(
            *firstEvent));

    REQUIRE(
        std::holds_alternative<llt::Quote>(
            *secondEvent));

    const auto &firstQuote =
        std::get<llt::Quote>(
            *firstEvent);

    const auto &secondQuote =
        std::get<llt::Quote>(
            *secondEvent);

    REQUIRE(
        firstQuote.sequence() ==
        llt::SequenceNumber{0});

    REQUIRE(
        secondQuote.sequence() ==
        llt::SequenceNumber{1});

    REQUIRE(
        firstQuote.timestamp() ==
        llt::Timestamp{1000});

    REQUIRE(
        secondQuote.timestamp() ==
        llt::Timestamp{2000});
}



TEST_CASE(
    "ITCH quote publisher records unknown instrument diagnostics")
{
    llt::market_data::InstrumentStore instruments;
    llt::MarketEventQueue queue;

    llt::itch::ItchQuotePublisher publisher{
        instruments,
        queue};

    const auto bbo =
        makeTwoSidedBbo();

    publisher.onBboChange(
        42,
        1000,
        bbo);

    publisher.onBboChange(
        42,
        2000,
        bbo);

    publisher.onBboChange(
        77,
        3000,
        bbo);

    REQUIRE(
        publisher.unknownInstruments() ==
        3);

    REQUIRE(
        publisher.unknownInstrumentSampleCount() ==
        2);

    const auto &samples =
        publisher.unknownInstrumentSamples();

    REQUIRE(samples[0].instrumentId == 42);
    REQUIRE(samples[0].timestamp == 1000);
    REQUIRE(samples[0].occurrences == 2);

    REQUIRE(samples[1].instrumentId == 77);
    REQUIRE(samples[1].timestamp == 3000);
    REQUIRE(samples[1].occurrences == 1);

    REQUIRE(queue.size() == 0);
}