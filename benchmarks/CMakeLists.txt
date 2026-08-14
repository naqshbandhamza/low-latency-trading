add_executable(spsc_app
    SpscRingBufferBenchmark.cpp
)


target_link_libraries(spsc_app
    PRIVATE
        common
)