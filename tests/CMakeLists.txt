

find_package(Catch2 3 REQUIRED)


add_executable(
    logger_tests
    logging/logger_test.cpp
)


target_link_libraries(
    logger_tests
    PRIVATE
        common
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(logger_tests)


add_executable(
    strong_type_tests
    types/StrongTypeTest.cpp
)

target_link_libraries(
    strong_type_tests
    PRIVATE
        common
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(strong_type_tests)

add_executable(
    price_type_tests
    types/PriceTest.cpp
)

target_link_libraries(
    price_type_tests
    PRIVATE
        common
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(price_type_tests)

add_executable(
    level_tests
    market_data/LevelTest.cpp
)

target_link_libraries(
    level_tests
    PRIVATE
        common
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(level_tests)


add_executable(
    quote_tests
    market_data/QuoteTest.cpp
)

target_link_libraries(
    quote_tests
    PRIVATE
        common
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(quote_tests)

add_executable(
    trade_tests
    market_data/TradeTest.cpp
)

target_link_libraries(
    trade_tests
    PRIVATE
        common
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(trade_tests)

add_executable(
    market_event_tests
    market_data/MarketEventTest.cpp
)

target_link_libraries(
    market_event_tests
    PRIVATE
        common
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(market_event_tests)


add_executable(
    spsc_ringbuffer_tests
    ring_buffer/SpscRingBufferTest.cpp
)

target_link_libraries(
    spsc_ringbuffer_tests
    PRIVATE
        common
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(spsc_ringbuffer_tests)




add_executable(
    SpscRingBufferConcurrencyTest
    ring_buffer/SpscRingBufferConcurrencyTest.cpp
)

target_link_libraries(
    SpscRingBufferConcurrencyTest
    PRIVATE
        common
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(SpscRingBufferConcurrencyTest)
