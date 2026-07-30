#include "FeedHandler.h"


namespace llt
{


FeedHandler::FeedHandler(ILogger& logger)
    :
      logger_(logger)
{

}


void FeedHandler::start()
{

    logger_.info(
        "Feed handler started"
    );


    logger_.debug(
        "Waiting for market packets"
    );

}


}