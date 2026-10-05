add_test( [==[UDP market data flows through FeedHandler into SPSC queue]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/UdpFeedHandlerTest [==[UDP market data flows through FeedHandler into SPSC queue]==]  )
set_tests_properties( [==[UDP market data flows through FeedHandler into SPSC queue]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( UdpFeedHandlerTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "UDP market data flows through FeedHandler into SPSC queue",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/feed_handler/UdpFeedHandlerTest.cpp",
      "line" : 79
    },
    "tags" : 
  }
]==] [==[UDP market data flows through FeedHandler into SPSC queue]==])
