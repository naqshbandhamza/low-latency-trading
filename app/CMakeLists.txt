# add_executable(trading_app
#     main.cpp
# )

# add_executable(market_data_simulator
#     market_data_simulator.cpp
# )


# target_link_libraries(trading_app
#     PRIVATE
#         common
#         feed_handler
# )

# target_link_libraries(market_data_simulator
#     PRIVATE
#         common
#         feed_handler
# )


add_executable(itch_replay
    itch_replay.cpp
)

target_link_libraries(itch_replay
    PRIVATE
        common
        feed_handler
)




# add_executable(
#     itch_synthetic_sender
#     itch_synthetic_sender.cpp
# )

# target_link_libraries(
#     itch_synthetic_sender
#     PRIVATE
#     common
#     feed_handler
# )


add_executable(itch_live
    itch_live.cpp
)

target_link_libraries(itch_live
    PRIVATE
        common
        feed_handler
)




# add_executable(
#     itch_udp_replay
#     itch_udp_replay.cpp
# )

# target_link_libraries(
#     itch_udp_replay
#     PRIVATE
#     common
#     feed_handler
# )



add_executable(
    itch_udp_replay_gap
    itch_udp_replay_gap.cpp
)

target_link_libraries(
    itch_udp_replay_gap
    PRIVATE
    common
    feed_handler
)




add_executable(
    itch_recovery_benchmark
    itch_recovery_benchmark.cpp
)

target_link_libraries(
    itch_recovery_benchmark
    PRIVATE
        common
        feed_handler
)




# add_executable(
#     itch_udp_load_gen
#     itch_udp_load_gen.cpp
# )


# target_link_libraries(
#     itch_udp_load_gen
#     PRIVATE
#         common
#         feed_handler
# )



add_executable(
    itch_mold_udp_load_gen
    itch_mold_udp_load_gen.cpp
)

target_link_libraries(
    itch_mold_udp_load_gen
    PRIVATE
        common
        feed_handler
)




add_executable(
    itch_mold_live
    itch_mold_live.cpp
)

target_link_libraries(
    itch_mold_live
    PRIVATE
        common
        feed_handler
)