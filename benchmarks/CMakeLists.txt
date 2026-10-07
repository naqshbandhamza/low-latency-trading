add_executable(spsc_app
    SpscRingBufferBenchmark.cpp
)


target_link_libraries(spsc_app
    PRIVATE
        common
)


add_executable(spsc_appb
    SpscRingBufferBench.cpp
)


target_link_libraries(spsc_appb
    PRIVATE
        common
)

add_executable(spsc_appc
    SpscRingBufferBenchC.cpp
)


target_link_libraries(spsc_appc
    PRIVATE
        common
)



add_executable(seq_cst_benchmark
    SeqCstBenchmark.cpp
)

target_link_libraries(seq_cst_benchmark
    PRIVATE
        common
)





add_executable(
    itch_pipeline_benchmark
    ItchPipelineBenchmark.cpp
)

target_link_libraries(
    itch_pipeline_benchmark
    PRIVATE
        common
        feed_handler
)


add_executable(
    itch_core_pipeline_benchmark
    ItchCorePipelineBenchmark.cpp
)

target_link_libraries(
    itch_core_pipeline_benchmark
    PRIVATE
        common
        feed_handler
)




add_executable(
    itch_decode_benchmark
    ItchDecodeBenchmark.cpp
)

target_link_libraries(
    itch_decode_benchmark
    PRIVATE
        common
        feed_handler
)






add_executable(
    udp_throughput_bench
    UdpThroughputBench.cpp
)


target_link_libraries(
    udp_throughput_bench
    PRIVATE
        common
        feed_handler
)

