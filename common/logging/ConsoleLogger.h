#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include "ILogger.h"


namespace llt
{

class ConsoleLogger : public ILogger
{

public:

    void log(
        LogLevel level,
        std::string_view message
    ) override;

    std::size_t warningCount{0};
    std::string lastWarning;

};

}