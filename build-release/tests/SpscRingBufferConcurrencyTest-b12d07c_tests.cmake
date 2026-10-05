add_test( [==[SPSC ring buffer works between producer and consumer]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/SpscRingBufferConcurrencyTest [==[SPSC ring buffer works between producer and consumer]==]  )
set_tests_properties( [==[SPSC ring buffer works between producer and consumer]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( SpscRingBufferConcurrencyTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "SPSC ring buffer works between producer and consumer",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/ring_buffer/SpscRingBufferConcurrencyTest.cpp",
      "line" : 8
    },
    "tags" : 
  }
]==] [==[SPSC ring buffer works between producer and consumer]==])
