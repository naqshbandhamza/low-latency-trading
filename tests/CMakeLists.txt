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