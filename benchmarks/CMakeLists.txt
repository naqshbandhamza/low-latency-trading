add_executable(spsc_app
    SpscRingBufferBenchmark.cpp
)


target_link_libraries(spsc_app
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