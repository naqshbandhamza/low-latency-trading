add_library(common STATIC
    logging/ConsoleLogger.cpp
    market_data/UdpMarketDataSource.cpp
    market_data/SequenceRecovery.cpp
    market_data/UdpMarketDataCodec.cpp
    market_data/itch/ItchDecoder.cpp
    market_data/itch/ItchDispatcher.cpp
    market_data/itch/ItchStreamReader.cpp
    market_data/itch/ItchReplay.cpp
    strategy/SimpleStrategy.cpp
    strategy/StrategyEngine.cpp
    market_data/itch/ItchQuotePublisher.cpp
    market_data/itch/ItchUdpCodec.cpp
    market_data/itch/UdpItchMarketDataSource.cpp
    market_data/itch/ItchSequenceRecovery.cpp
    market_data/itch/ItchTradePublisher.cpp
    market_data/itch/FileItchRecoverySource.cpp
    market_data/moldudp64/MoldUdp64Codec.cpp
    market_data/moldudp64/UdpMoldMarketDataSource.cpp
    market_data/moldudp64/FileMoldItchRecoverySource.cpp
    market_data/moldudp64/MoldItchSequenceRecovery.cpp
)

target_include_directories(common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(common
    PUBLIC
        cxx_std_17
)