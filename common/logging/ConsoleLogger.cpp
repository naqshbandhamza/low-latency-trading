#include "ConsoleLogger.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>


namespace llt
{


void ConsoleLogger::log(
    LogLevel level,
    std::string_view message
)
{

    auto now =
        std::chrono::system_clock::now();


    auto time =
        std::chrono::system_clock::to_time_t(now);


    std::cout
        << "["
        << std::ctime(&time)
        << "] ";


    switch(level)
    {

        case LogLevel::Debug:
            std::cout << "[DEBUG] ";
            break;


        case LogLevel::Info:
            std::cout << "[INFO] ";
            break;


        case LogLevel::Warning:
            std::cout << "[WARNING] ";
            break;


        case LogLevel::Error:
            std::cout << "[ERROR] ";
            break;

    }


    std::cout
        << "[Thread "
        << std::this_thread::get_id()
        << "] "
        << message
        << '\n';

}


}