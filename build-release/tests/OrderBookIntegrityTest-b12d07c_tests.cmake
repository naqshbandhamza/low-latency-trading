add_test( [==[Complete lifecycle leaves no book integrity errors]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderBookIntegrityTest [==[Complete lifecycle leaves no book integrity errors]==]  )
set_tests_properties( [==[Complete lifecycle leaves no book integrity errors]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Valid cancellation produces no book integrity errors]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderBookIntegrityTest [==[Valid cancellation produces no book integrity errors]==]  )
set_tests_properties( [==[Valid cancellation produces no book integrity errors]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Valid deletion produces no book integrity errors]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderBookIntegrityTest [==[Valid deletion produces no book integrity errors]==]  )
set_tests_properties( [==[Valid deletion produces no book integrity errors]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Valid full execution produces no book integrity errors]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderBookIntegrityTest [==[Valid full execution produces no book integrity errors]==]  )
set_tests_properties( [==[Valid full execution produces no book integrity errors]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Valid order execution produces no book integrity errors]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderBookIntegrityTest [==[Valid order execution produces no book integrity errors]==]  )
set_tests_properties( [==[Valid order execution produces no book integrity errors]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( OrderBookIntegrityTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "Complete lifecycle leaves no book integrity errors",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/OrderBookIntegrityTest.cpp",
      "line" : 126
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Valid cancellation produces no book integrity errors",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/OrderBookIntegrityTest.cpp",
      "line" : 79
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Valid deletion produces no book integrity errors",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/OrderBookIntegrityTest.cpp",
      "line" : 102
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Valid full execution produces no book integrity errors",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/OrderBookIntegrityTest.cpp",
      "line" : 54
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Valid order execution produces no book integrity errors",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/OrderBookIntegrityTest.cpp",
      "line" : 31
    },
    "tags" : 
  }
]==] [==[Complete lifecycle leaves no book integrity errors]==] [==[Valid cancellation produces no book integrity errors]==] [==[Valid deletion produces no book integrity errors]==] [==[Valid full execution produces no book integrity errors]==] [==[Valid order execution produces no book integrity errors]==])
