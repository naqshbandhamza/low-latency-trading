add_test( [==[MarketEvent can be visited]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/market_event_tests [==[MarketEvent can be visited]==]  )
set_tests_properties( [==[MarketEvent can be visited]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[MarketEvent can contain a Quote]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/market_event_tests [==[MarketEvent can contain a Quote]==]  )
set_tests_properties( [==[MarketEvent can contain a Quote]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[MarketEvent can contain a Trade]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/market_event_tests [==[MarketEvent can contain a Trade]==]  )
set_tests_properties( [==[MarketEvent can contain a Trade]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( market_event_tests_TESTS [==[
  {
    "class-name" : "",
    "name" : "MarketEvent can be visited",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/MarketEventTest.cpp",
      "line" : 47
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "MarketEvent can contain a Quote",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/MarketEventTest.cpp",
      "line" : 9
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "MarketEvent can contain a Trade",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/MarketEventTest.cpp",
      "line" : 30
    },
    "tags" : 
  }
]==] [==[MarketEvent can be visited]==] [==[MarketEvent can contain a Quote]==] [==[MarketEvent can contain a Trade]==])
