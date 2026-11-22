cmake_minimum_required(VERSION 3.20)

project(
    LowLatencyTrading
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(LLT_ENABLE_LATENCY_TRACE "Enable nanosecond latency tracing" OFF)
if(LLT_ENABLE_LATENCY_TRACE)
    add_compile_definitions(LLT_ENABLE_LATENCY_TRACE)
endif()

include(CTest)
enable_testing()

add_subdirectory(common)
add_subdirectory(feed_handler)
add_subdirectory(app)
add_subdirectory(benchmarks)
add_subdirectory(tests)
