add_executable(trading_app
    main.cpp
)

add_executable(market_data_simulator
    market_data_simulator.cpp
)


target_link_libraries(trading_app
    PRIVATE
        common
        feed_handler
)

target_link_libraries(market_data_simulator
    PRIVATE
        common
        feed_handler
)


add_executable(itch_replay
    itch_replay.cpp
)

target_link_libraries(itch_replay
    PRIVATE
        common
        feed_handler
)