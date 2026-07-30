#pragma once

#include <string_view>

#include "LogLevel.h"


namespace llt
{

class ILogger
{

public:

    virtual ~ILogger() = default;


    virtual void log(
        LogLevel level,
        std::string_view message
    ) = 0;


    void info(std::string_view message)
    {
        log(LogLevel::Info, message);
    }


    void debug(std::string_view message)
    {
        log(LogLevel::Debug, message);
    }


    void warning(std::string_view message)
    {
        log(LogLevel::Warning, message);
    }


    void error(std::string_view message)
    {
        log(LogLevel::Error, message);
    }

};

}