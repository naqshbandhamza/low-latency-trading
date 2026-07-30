#include "logging/ConsoleLogger.h"
#include "FeedHandler.h"


int main()
{

    llt::ConsoleLogger logger;


    llt::FeedHandler feed(logger);


    feed.start();


    return 0;
}