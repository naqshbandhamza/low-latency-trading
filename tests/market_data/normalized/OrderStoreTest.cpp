#include <catch2/catch_test_macros.hpp>

#include "market_data/normalized/OrderStore.h"

using namespace llt::market_data;


TEST_CASE(
    "OrderStore starts empty"
)
{
    OrderStore store;

    REQUIRE(store.size() == 0);
    REQUIRE_FALSE(store.contains(100));
    REQUIRE(store.find(100) == nullptr);
}


TEST_CASE(
    "OrderStore adds order"
)
{
    OrderStore store;

    Order order{
        .orderId = 100,
        .instrumentId = 42,
        .timestamp = 1000,
        .price = 1872500,
        .quantity = 500,
        .side = Side::Buy
    };

    REQUIRE(store.add(order));
    REQUIRE(store.size() == 1);
    REQUIRE(store.contains(100));

    const auto* result =
        store.find(100);

    REQUIRE(result != nullptr);
    REQUIRE(result->orderId == 100);
    REQUIRE(result->instrumentId == 42);
    REQUIRE(result->price == 1872500);
    REQUIRE(result->quantity == 500);
    REQUIRE(result->side == Side::Buy);
}


TEST_CASE(
    "OrderStore rejects duplicate order id"
)
{
    OrderStore store;

    Order first{
        .orderId = 100,
        .instrumentId = 42,
        .price = 1000,
        .quantity = 500,
        .side = Side::Buy
    };

    Order duplicate{
        .orderId = 100,
        .instrumentId = 50,
        .price = 2000,
        .quantity = 100,
        .side = Side::Sell
    };

    REQUIRE(store.add(first));
    REQUIRE_FALSE(store.add(duplicate));

    REQUIRE(store.size() == 1);

    const auto* result =
        store.find(100);

    REQUIRE(result != nullptr);

    // Original order must remain untouched.
    REQUIRE(result->instrumentId == 42);
    REQUIRE(result->price == 1000);
    REQUIRE(result->quantity == 500);
}


TEST_CASE(
    "OrderStore stores independent orders"
)
{
    OrderStore store;

    Order first{
        .orderId = 100,
        .instrumentId = 42,
        .price = 1000,
        .quantity = 500,
        .side = Side::Buy
    };

    Order second{
        .orderId = 200,
        .instrumentId = 42,
        .price = 1100,
        .quantity = 300,
        .side = Side::Sell
    };

    REQUIRE(store.add(first));
    REQUIRE(store.add(second));

    REQUIRE(store.size() == 2);

    REQUIRE(
        store.find(100)->price == 1000
    );

    REQUIRE(
        store.find(200)->price == 1100
    );
}


TEST_CASE(
    "OrderStore mutable lookup can update quantity"
)
{
    OrderStore store;

    Order order{
        .orderId = 100,
        .instrumentId = 42,
        .price = 1000,
        .quantity = 500,
        .side = Side::Buy
    };

    REQUIRE(store.add(order));

    auto* stored =
        store.find(100);

    REQUIRE(stored != nullptr);

    stored->quantity -= 200;

    REQUIRE(
        store.find(100)->quantity == 300
    );
}


TEST_CASE(
    "OrderStore removes order"
)
{
    OrderStore store;

    Order order{
        .orderId = 100,
        .instrumentId = 42,
        .price = 1000,
        .quantity = 500,
        .side = Side::Buy
    };

    REQUIRE(store.add(order));

    REQUIRE(store.remove(100));

    REQUIRE(store.size() == 0);
    REQUIRE_FALSE(store.contains(100));
    REQUIRE(store.find(100) == nullptr);
}


TEST_CASE(
    "OrderStore removing unknown order returns false"
)
{
    OrderStore store;

    REQUIRE_FALSE(
        store.remove(999)
    );

    REQUIRE(store.size() == 0);
}