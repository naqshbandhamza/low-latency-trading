#include <catch2/catch_test_macros.hpp>

#include "market_data/normalized/OrderStore.h"
#include "market_data/book/BookSide.h"

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

    BookSide bookSide{
        Side::Buy
    };

    Order order{
        .orderId = 100,
        .instrumentId = 42,
        .timestamp = 1000,
        .price = 1872500,
        .quantity = 500,
        .side = Side::Buy
    };

    auto level =
        bookSide.add(
            order.price,
            order.quantity);

    REQUIRE(
        store.add(
            order,
            level)
    );

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

    BookSide bookSide{
        Side::Buy
    };

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

    auto level =
        bookSide.add(
            first.price,
            first.quantity);

    REQUIRE(
        store.add(
            first,
            level)
    );

    // We deliberately reuse a valid handle here.
    //
    // This test is testing OrderStore's duplicate-ID
    // rejection, not BookSide mutation.
    REQUIRE_FALSE(
        store.add(
            duplicate,
            level)
    );

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

    BookSide bidSide{
        Side::Buy
    };

    BookSide askSide{
        Side::Sell
    };

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

    auto firstLevel =
        bidSide.add(
            first.price,
            first.quantity);

    auto secondLevel =
        askSide.add(
            second.price,
            second.quantity);

    REQUIRE(
        store.add(
            first,
            firstLevel)
    );

    REQUIRE(
        store.add(
            second,
            secondLevel)
    );

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

    BookSide bookSide{
        Side::Buy
    };

    Order order{
        .orderId = 100,
        .instrumentId = 42,
        .price = 1000,
        .quantity = 500,
        .side = Side::Buy
    };

    auto level =
        bookSide.add(
            order.price,
            order.quantity);

    REQUIRE(
        store.add(
            order,
            level)
    );

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

    BookSide bookSide{
        Side::Buy
    };

    Order order{
        .orderId = 100,
        .instrumentId = 42,
        .price = 1000,
        .quantity = 500,
        .side = Side::Buy
    };

    auto level =
        bookSide.add(
            order.price,
            order.quantity);

    REQUIRE(
        store.add(
            order,
            level)
    );

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


TEST_CASE(
    "OrderStore supports repeated insert remove churn")
{
    OrderStore store;
    BookSide side{Side::Buy};

    constexpr std::size_t Iterations =
        10000;

    for (std::size_t i = 0;
         i < Iterations;
         ++i)
    {
        const OrderId orderId =
            static_cast<OrderId>(
                100000 + i);

        Order order{
            .orderId = orderId,
            .instrumentId = 42,
            .timestamp =
                static_cast<Timestamp>(i),
            .price = 1000,
            .quantity = 100,
            .side = Side::Buy};

        auto level =
            side.add(
                order.price,
                order.quantity);

        REQUIRE(
            store.add(
                order,
                level));

        REQUIRE(
            store.contains(orderId));

        REQUIRE(
            store.remove(orderId));

        REQUIRE_FALSE(
            store.contains(orderId));

        // Keep BookSide state consistent as well.
        REQUIRE(
            side.removeOrder(
                level,
                order.quantity));
    }

    REQUIRE(
        store.size() == 0);

    REQUIRE(
        store.peakSize() == 1);
}

TEST_CASE(
    "OrderStore remains correct across repeated population churn")
{
    OrderStore store;
    BookSide side{Side::Buy};

    constexpr std::size_t BatchSize =
        10000;

    constexpr std::size_t Rounds =
        10;

    for (std::size_t round = 0;
         round < Rounds;
         ++round)
    {
        for (std::size_t i = 0;
             i < BatchSize;
             ++i)
        {
            const OrderId orderId =
                static_cast<OrderId>(
                    round * 100000 +
                    i + 1);

            Order order{
                .orderId = orderId,
                .instrumentId = 42,
                .timestamp =
                    static_cast<Timestamp>(i),
                .price =
                    static_cast<Price>(
                        1000 + (i % 100)),
                .quantity = 100,
                .side = Side::Buy};

            auto level =
                side.add(
                    order.price,
                    order.quantity);

            REQUIRE(
                store.add(
                    order,
                    level));
        }

        REQUIRE(
            store.size() ==
            BatchSize);

        for (std::size_t i = 0;
             i < BatchSize;
             ++i)
        {
            const OrderId orderId =
                static_cast<OrderId>(
                    round * 100000 +
                    i + 1);

            REQUIRE(
                store.find(orderId) !=
                nullptr);
        }

        // Remove OrderStore entries.
        //
        // We intentionally don't use the stored level handle
        // here because many orders share the same price level.
        // BookSide is only being used to provide valid handles
        // for StoredOrder construction.
        for (std::size_t i = 0;
             i < BatchSize;
             ++i)
        {
            const OrderId orderId =
                static_cast<OrderId>(
                    round * 100000 +
                    i + 1);

            REQUIRE(
                store.remove(orderId));
        }

        REQUIRE(
            store.size() == 0);
    }

    REQUIRE(
        store.peakSize() ==
        BatchSize);
}