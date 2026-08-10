cmake_minimum_required(VERSION 3.20)

project(
    LowLatencyTrading
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()

add_subdirectory(common)
add_subdirectory(feed_handler)
add_subdirectory(app)
add_subdirectory(tests)