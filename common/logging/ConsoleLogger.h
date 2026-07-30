#pragma once

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

};

}