#include <catch2/catch_test_macros.hpp>

#include "logging/ConsoleLogger.h"


TEST_CASE(
    "Console logger can be created",
    "[logger]"
)
{

    llt::ConsoleLogger logger;


    REQUIRE(
        true
    );

}