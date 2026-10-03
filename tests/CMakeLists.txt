

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







add_executable(
    OrderStoreTest
    market_data/normalized/OrderStoreTest.cpp
)

target_link_libraries(
    OrderStoreTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(OrderStoreTest)





add_executable(
    PriceLevelTest
    market_data/book/PriceLevelTest.cpp
)

target_link_libraries(
    PriceLevelTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(PriceLevelTest)





add_executable(
    BookSideTest
    market_data/book/BookSideTest.cpp
)

target_link_libraries(
    BookSideTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(BookSideTest)






add_executable(
    OrderBookTest
    market_data/book/OrderBookTest.cpp
)

target_link_libraries(
    OrderBookTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(OrderBookTest)




add_executable(
    BookStoreTest
    market_data/book/BookStoreTest.cpp
)

target_link_libraries(
    BookStoreTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(BookStoreTest)





add_executable(
    OrderBookAddIntegrationTest
    market_data/itch/OrderBookAddIntegrationTest.cpp
)

target_link_libraries(
    OrderBookAddIntegrationTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(OrderBookAddIntegrationTest)





add_executable(
    OrderBookLifecycleIntegrationTest
    market_data/itch/OrderBookLifecycleIntegrationTest.cpp
)

target_link_libraries(
    OrderBookLifecycleIntegrationTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(OrderBookLifecycleIntegrationTest)







add_executable(
    OrderBookIntegrityTest
    market_data/itch/OrderBookIntegrityTest.cpp
)

target_link_libraries(
    OrderBookIntegrityTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(OrderBookIntegrityTest)





add_executable(
    BboTest
    market_data/book/BboTest.cpp
)

target_link_libraries(
    BboTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(BboTest)




add_executable(
    BboChangeIntegrationTest
    market_data/itch/BboChangeIntegrationTest.cpp
)

target_link_libraries(
    BboChangeIntegrationTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(BboChangeIntegrationTest)





add_executable(
    ItchQuotePublisherTest
    market_data/itch/ItchQuotePublisherTest.cpp
)

target_link_libraries(
    ItchQuotePublisherTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(ItchQuotePublisherTest)





add_executable(
    ItchQuotePipelineIntegrationTest
    market_data/itch/ItchQuotePipelineIntegrationTest.cpp
)

target_link_libraries(
    ItchQuotePipelineIntegrationTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(ItchQuotePipelineIntegrationTest)






add_executable(
    MarketEventConsumerTest
    market_data/MarketEventConsumerTest.cpp
)

target_link_libraries(
    MarketEventConsumerTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(MarketEventConsumerTest)





add_executable(
    ItchUdpCodecTest
    market_data/itch/ItchUdpCodecTest.cpp
)

target_link_libraries(
    ItchUdpCodecTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(ItchUdpCodecTest)




add_executable(
    UdpItchMarketDataSourceTest
    market_data/itch/UdpItchMarketDataSourceTest.cpp
)

target_link_libraries(
    UdpItchMarketDataSourceTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(UdpItchMarketDataSourceTest)









add_executable(
    ItchSequenceRecoveryTest
    market_data/itch/ItchSequenceRecoveryTest.cpp
)

target_link_libraries(
    ItchSequenceRecoveryTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(ItchSequenceRecoveryTest)





add_executable(
    ItchFeedHandlerTest
    market_data/itch/ItchFeedHandlerTest.cpp
)

target_link_libraries(
    ItchFeedHandlerTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(ItchFeedHandlerTest)




add_executable(
    ItchUdpFeedIntegrationTest
    market_data/itch/ItchUdpFeedIntegrationTest.cpp
)

target_link_libraries(
    ItchUdpFeedIntegrationTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(ItchUdpFeedIntegrationTest)





add_executable(
    ItchUdpQuotePipelineIntegrationTest
    market_data/itch/ItchUdpQuotePipelineIntegrationTest.cpp
)

target_link_libraries(
    ItchUdpQuotePipelineIntegrationTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(ItchUdpQuotePipelineIntegrationTest)








add_executable(
    ItchTradePublicationTest
    market_data/itch/ItchTradePublicationTest.cpp
)

target_link_libraries(
    ItchTradePublicationTest
    PRIVATE
        common
        feed_handler
        Catch2::Catch2WithMain
)

catch_discover_tests(
    ItchTradePublicationTest
)