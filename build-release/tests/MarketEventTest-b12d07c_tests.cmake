add_test( [==[SPSC ring buffer transports MarketEvent]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/MarketEventTest [==[SPSC ring buffer transports MarketEvent]==]  )
set_tests_properties( [==[SPSC ring buffer transports MarketEvent]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[SPSC ring buffer transports Trade]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/MarketEventTest [==[SPSC ring buffer transports Trade]==]  )
set_tests_properties( [==[SPSC ring buffer transports Trade]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( MarketEventTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "SPSC ring buffer transports MarketEvent",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/ring_buffer/MarketEventRingBufferTest.cpp",
      "line" : 6
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "SPSC ring buffer transports Trade",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/ring_buffer/MarketEventRingBufferTest.cpp",
      "line" : 58
    },
    "tags" : 
  }
]==] [==[SPSC ring buffer transports MarketEvent]==] [==[SPSC ring buffer transports Trade]==])
