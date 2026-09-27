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
)

target_include_directories(common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(common
    PUBLIC
        cxx_std_17
)