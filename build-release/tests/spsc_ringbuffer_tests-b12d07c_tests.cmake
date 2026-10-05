add_test( [==[Ring buffer cannot pop from empty buffer]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/spsc_ringbuffer_tests [==[Ring buffer cannot pop from empty buffer]==]  )
set_tests_properties( [==[Ring buffer cannot pop from empty buffer]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Ring buffer detects full state]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/spsc_ringbuffer_tests [==[Ring buffer detects full state]==]  )
set_tests_properties( [==[Ring buffer detects full state]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Ring buffer pushes and pops]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/spsc_ringbuffer_tests [==[Ring buffer pushes and pops]==]  )
set_tests_properties( [==[Ring buffer pushes and pops]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Ring buffer starts empty]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/spsc_ringbuffer_tests [==[Ring buffer starts empty]==]  )
set_tests_properties( [==[Ring buffer starts empty]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Ring buffer wraps around]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/spsc_ringbuffer_tests [==[Ring buffer wraps around]==]  )
set_tests_properties( [==[Ring buffer wraps around]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( spsc_ringbuffer_tests_TESTS [==[
  {
    "class-name" : "",
    "name" : "Ring buffer cannot pop from empty buffer",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/ring_buffer/SpscRingBufferTest.cpp",
      "line" : 50
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Ring buffer detects full state",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/ring_buffer/SpscRingBufferTest.cpp",
      "line" : 36
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Ring buffer pushes and pops",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/ring_buffer/SpscRingBufferTest.cpp",
      "line" : 14
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Ring buffer starts empty",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/ring_buffer/SpscRingBufferTest.cpp",
      "line" : 5
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Ring buffer wraps around",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/ring_buffer/SpscRingBufferTest.cpp",
      "line" : 59
    },
    "tags" : 
  }
]==] [==[Ring buffer cannot pop from empty buffer]==] [==[Ring buffer detects full state]==] [==[Ring buffer pushes and pops]==] [==[Ring buffer starts empty]==] [==[Ring buffer wraps around]==])
