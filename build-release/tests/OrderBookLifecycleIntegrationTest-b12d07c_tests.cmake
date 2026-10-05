add_test( [==[Complex order lifecycle keeps order store and book synchronized]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderBookLifecycleIntegrationTest [==[Complex order lifecycle keeps order store and book synchronized]==]  )
set_tests_properties( [==[Complex order lifecycle keeps order store and book synchronized]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Deleting one order preserves other quantity at same price]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderBookLifecycleIntegrationTest [==[Deleting one order preserves other quantity at same price]==]  )
set_tests_properties( [==[Deleting one order preserves other quantity at same price]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Full order execution removes price level]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderBookLifecycleIntegrationTest [==[Full order execution removes price level]==]  )
set_tests_properties( [==[Full order execution removes price level]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Order cancel reduces book quantity]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderBookLifecycleIntegrationTest [==[Order cancel reduces book quantity]==]  )
set_tests_properties( [==[Order cancel reduces book quantity]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Order delete removes remaining quantity from book]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderBookLifecycleIntegrationTest [==[Order delete removes remaining quantity from book]==]  )
set_tests_properties( [==[Order delete removes remaining quantity from book]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Order execution reduces book quantity]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderBookLifecycleIntegrationTest [==[Order execution reduces book quantity]==]  )
set_tests_properties( [==[Order execution reduces book quantity]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Order lifecycle updates BBO]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderBookLifecycleIntegrationTest [==[Order lifecycle updates BBO]==]  )
set_tests_properties( [==[Order lifecycle updates BBO]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Order replace moves quantity between price levels]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderBookLifecycleIntegrationTest [==[Order replace moves quantity between price levels]==]  )
set_tests_properties( [==[Order replace moves quantity between price levels]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Priced execution reduces resting price level]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderBookLifecycleIntegrationTest [==[Priced execution reduces resting price level]==]  )
set_tests_properties( [==[Priced execution reduces resting price level]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( OrderBookLifecycleIntegrationTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "Complex order lifecycle keeps order store and book synchronized",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/OrderBookLifecycleIntegrationTest.cpp",
      "line" : 410
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Deleting one order preserves other quantity at same price",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/OrderBookLifecycleIntegrationTest.cpp",
      "line" : 241
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Full order execution removes price level",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/OrderBookLifecycleIntegrationTest.cpp",
      "line" : 81
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Order cancel reduces book quantity",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/OrderBookLifecycleIntegrationTest.cpp",
      "line" : 155
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Order delete removes remaining quantity from book",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/OrderBookLifecycleIntegrationTest.cpp",
      "line" : 188
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Order execution reduces book quantity",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/OrderBookLifecycleIntegrationTest.cpp",
      "line" : 43
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Order lifecycle updates BBO",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/OrderBookLifecycleIntegrationTest.cpp",
      "line" : 346
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Order replace moves quantity between price levels",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/OrderBookLifecycleIntegrationTest.cpp",
      "line" : 284
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Priced execution reduces resting price level",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/OrderBookLifecycleIntegrationTest.cpp",
      "line" : 117
    },
    "tags" : 
  }
]==] [==[Complex order lifecycle keeps order store and book synchronized]==] [==[Deleting one order preserves other quantity at same price]==] [==[Full order execution removes price level]==] [==[Order cancel reduces book quantity]==] [==[Order delete removes remaining quantity from book]==] [==[Order execution reduces book quantity]==] [==[Order lifecycle updates BBO]==] [==[Order replace moves quantity between price levels]==] [==[Priced execution reduces resting price level]==])
