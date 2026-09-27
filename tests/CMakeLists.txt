

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


add_executable(
    AtomicTest
    ring_buffer/AtomicTest.cpp
)

target_link_libraries(
    AtomicTest
    PRIVATE
        common
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(AtomicTest)


add_executable(
    MarketEventTest
    ring_buffer/MarketEventRingBufferTest.cpp
)

target_link_libraries(
    MarketEventTest
    PRIVATE
        common
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(MarketEventTest)



add_executable(
    FeedHandlerMockTest
    feed_handler/FeedHandlerTest.cpp
)

target_link_libraries(
    FeedHandlerMockTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(FeedHandlerMockTest)





add_executable(
    UdpMarketDataSourceTest
    market_data/UdpMarketDataSourceTest.cpp
)

target_link_libraries(
    UdpMarketDataSourceTest
    PRIVATE
        common
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(UdpMarketDataSourceTest)





add_executable(
    UdpFeedHandlerTest
    feed_handler/UdpFeedHandlerTest.cpp
)

target_link_libraries(
    UdpFeedHandlerTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(UdpFeedHandlerTest)





add_executable(
    UdpCodecTest
    market_data/UdpMarketDataCodecTest.cpp
)

target_link_libraries(
    UdpCodecTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(UdpCodecTest)





add_executable(
    FeedHandlerUdpIntegrationTest
    feed_handler/FeedHandlerUdpIntegrationTest.cpp
)

target_link_libraries(
    FeedHandlerUdpIntegrationTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(FeedHandlerUdpIntegrationTest)


add_executable(
    SimpleStrategyTest
    strategy/SimpleStrategyTest.cpp
)

target_link_libraries(
    SimpleStrategyTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(SimpleStrategyTest)


add_executable(
    StrategyEngineTest
    strategy/StrategyEngineTest.cpp
)

target_link_libraries(
    StrategyEngineTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(StrategyEngineTest)




add_executable(
    ItchDecoderTest
    market_data/itch/ItchDecoderTest.cpp
)

target_link_libraries(
    ItchDecoderTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(ItchDecoderTest)



add_executable(
    ItchDispatcherTest
    market_data/itch/ItchDispatcherTest.cpp
)

target_link_libraries(
    ItchDispatcherTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(ItchDispatcherTest)



add_executable(
    ItchStreamReaderTest
    market_data/itch/ItchStreamReaderTest.cpp
)

target_link_libraries(
    ItchStreamReaderTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(ItchStreamReaderTest)




add_executable(
    ItchReplayTest
    market_data/itch/ItchReplayTest.cpp
)

target_link_libraries(
    ItchReplayTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(ItchReplayTest)




add_executable(
    AddOrderWithMpidMessageTest
    market_data/itch/AddOrderWithMpidMessageTest.cpp
)

target_link_libraries(
    AddOrderWithMpidMessageTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(AddOrderWithMpidMessageTest)




add_executable(
    TradeMessageTest
    market_data/itch/TradeMessageTest.cpp
)

target_link_libraries(
    TradeMessageTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(TradeMessageTest)


add_executable(
    CrossTradeMessageTest
    market_data/itch/CrossTradeMessageTest.cpp
)

target_link_libraries(
    CrossTradeMessageTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(CrossTradeMessageTest)


add_executable(
    BrokenTradeMessageTest
    market_data/itch/BrokenTradeMessageTest.cpp
)

target_link_libraries(
    BrokenTradeMessageTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(BrokenTradeMessageTest)


add_executable(
    StockTradingActionMessageTest
    market_data/itch/StockTradingActionMessageTest.cpp
)

target_link_libraries(
    StockTradingActionMessageTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(StockTradingActionMessageTest)


add_executable(
    MarketParticipantPositionMessageTest
    market_data/itch/MarketParticipantPositionMessageTest.cpp
)

target_link_libraries(
    MarketParticipantPositionMessageTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(MarketParticipantPositionMessageTest)




add_executable(
    RemainingItchMessagesTest
    market_data/itch/RemainingItchMessagesTest.cpp
)

target_link_libraries(
    RemainingItchMessagesTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(RemainingItchMessagesTest)




add_executable(
    InstrumentStoreTest
    market_data/normalized/InstrumentStoreTest.cpp
)

target_link_libraries(
    InstrumentStoreTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(InstrumentStoreTest)






add_executable(
    ItchNormalizerTest
    market_data/itch/ItchNormalizerTest.cpp
)

target_link_libraries(
    ItchNormalizerTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(ItchNormalizerTest)