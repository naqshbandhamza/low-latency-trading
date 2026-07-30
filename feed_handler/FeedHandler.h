#pragma once

#include "logging/ILogger.h"


namespace llt
{

class FeedHandler
{

public:

    explicit FeedHandler(ILogger& logger);


    void start();


private:

    ILogger& logger_;

};

}