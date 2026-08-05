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


add_executable(
    strong_type_tests
    types/StrongTypeTest.cpp
)

target_link_libraries(
    strong_type_tests
    PRIVATE
        common
        Catch2::Catch2WithMain
)


add_executable(
    price_type_tests
    types/PriceTest.cpp
)

target_link_libraries(
    price_type_tests
    PRIVATE
        common
        Catch2::Catch2WithMain
)