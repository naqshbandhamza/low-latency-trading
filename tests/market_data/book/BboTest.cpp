#include <catch2/catch_test_macros.hpp>

#include "market_data/book/OrderBook.h"

using namespace llt::market_data;


TEST_CASE(
    "Empty BBO values are equal"
)
{
    const Bbo first{};
    const Bbo second{};

    REQUIRE(first == second);
    REQUIRE_FALSE(first != second);
}


TEST_CASE(
    "BBO detects bid appearance"
)
{
    const Bbo before{};

    Bbo after{};
    after.hasBid = true;
    after.bidPrice = 1000000;
    after.bidQuantity = 500;

    REQUIRE(before != after);
}


TEST_CASE(
    "BBO detects bid disappearance"
)
{
    Bbo before{};
    before.hasBid = true;
    before.bidPrice = 1000000;
    before.bidQuantity = 500;

    const Bbo after{};

    REQUIRE(before != after);
}


TEST_CASE(
    "BBO detects best bid price change"
)
{
    Bbo before{};
    before.hasBid = true;
    before.bidPrice = 1000000;
    before.bidQuantity = 500;

    Bbo after = before;
    after.bidPrice = 1000100;

    REQUIRE(before != after);
}


TEST_CASE(
    "BBO detects best bid quantity change"
)
{
    Bbo before{};
    before.hasBid = true;
    before.bidPrice = 1000000;
    before.bidQuantity = 500;

    Bbo after = before;
    after.bidQuantity = 800;

    REQUIRE(before != after);
}


TEST_CASE(
    "BBO detects ask appearance"
)
{
    const Bbo before{};

    Bbo after{};
    after.hasAsk = true;
    after.askPrice = 1000100;
    after.askQuantity = 300;

    REQUIRE(before != after);
}


TEST_CASE(
    "BBO detects ask disappearance"
)
{
    Bbo before{};
    before.hasAsk = true;
    before.askPrice = 1000100;
    before.askQuantity = 300;

    const Bbo after{};

    REQUIRE(before != after);
}


TEST_CASE(
    "BBO detects best ask price change"
)
{
    Bbo before{};
    before.hasAsk = true;
    before.askPrice = 1000100;
    before.askQuantity = 300;

    Bbo after = before;
    after.askPrice = 1000200;

    REQUIRE(before != after);
}


TEST_CASE(
    "BBO detects best ask quantity change"
)
{
    Bbo before{};
    before.hasAsk = true;
    before.askPrice = 1000100;
    before.askQuantity = 300;

    Bbo after = before;
    after.askQuantity = 700;

    REQUIRE(before != after);
}


TEST_CASE(
    "BBO ignores values for absent bid"
)
{
    Bbo first{};

    Bbo second{};
    second.bidPrice = 1000000;
    second.bidQuantity = 500;

    REQUIRE_FALSE(first.hasBid);
    REQUIRE_FALSE(second.hasBid);

    REQUIRE(first == second);
}


TEST_CASE(
    "BBO ignores values for absent ask"
)
{
    Bbo first{};

    Bbo second{};
    second.askPrice = 1000100;
    second.askQuantity = 500;

    REQUIRE_FALSE(first.hasAsk);
    REQUIRE_FALSE(second.hasAsk);

    REQUIRE(first == second);
}


TEST_CASE(
    "Identical two sided BBO values are equal"
)
{
    Bbo first{};

    first.hasBid = true;
    first.bidPrice = 1000000;
    first.bidQuantity = 500;

    first.hasAsk = true;
    first.askPrice = 1000100;
    first.askQuantity = 300;

    const Bbo second =
        first;

    REQUIRE(first == second);
    REQUIRE_FALSE(first != second);
}