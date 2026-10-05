add_test( [==[Duplicate ITCH Add Order does not duplicate book quantity]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderBookAddIntegrationTest [==[Duplicate ITCH Add Order does not duplicate book quantity]==]  )
set_tests_properties( [==[Duplicate ITCH Add Order does not duplicate book quantity]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH Add Order creates book price level]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderBookAddIntegrationTest [==[ITCH Add Order creates book price level]==]  )
set_tests_properties( [==[ITCH Add Order creates book price level]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH Add Order routes sell order to ask side]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderBookAddIntegrationTest [==[ITCH Add Order routes sell order to ask side]==]  )
set_tests_properties( [==[ITCH Add Order routes sell order to ask side]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH Add Order with MPID updates order book]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderBookAddIntegrationTest [==[ITCH Add Order with MPID updates order book]==]  )
set_tests_properties( [==[ITCH Add Order with MPID updates order book]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH Add Orders aggregate at same price]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderBookAddIntegrationTest [==[ITCH Add Orders aggregate at same price]==]  )
set_tests_properties( [==[ITCH Add Orders aggregate at same price]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH Add Orders create correct BBO]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderBookAddIntegrationTest [==[ITCH Add Orders create correct BBO]==]  )
set_tests_properties( [==[ITCH Add Orders create correct BBO]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH Add Orders maintain independent instrument books]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderBookAddIntegrationTest [==[ITCH Add Orders maintain independent instrument books]==]  )
set_tests_properties( [==[ITCH Add Orders maintain independent instrument books]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( OrderBookAddIntegrationTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "Duplicate ITCH Add Order does not duplicate book quantity",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/OrderBookAddIntegrationTest.cpp",
      "line" : 243
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH Add Order creates book price level",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/OrderBookAddIntegrationTest.cpp",
      "line" : 38
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH Add Order routes sell order to ask side",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/OrderBookAddIntegrationTest.cpp",
      "line" : 73
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH Add Order with MPID updates order book",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/OrderBookAddIntegrationTest.cpp",
      "line" : 287
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH Add Orders aggregate at same price",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/OrderBookAddIntegrationTest.cpp",
      "line" : 110
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH Add Orders create correct BBO",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/OrderBookAddIntegrationTest.cpp",
      "line" : 144
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH Add Orders maintain independent instrument books",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/OrderBookAddIntegrationTest.cpp",
      "line" : 192
    },
    "tags" : 
  }
]==] [==[Duplicate ITCH Add Order does not duplicate book quantity]==] [==[ITCH Add Order creates book price level]==] [==[ITCH Add Order routes sell order to ask side]==] [==[ITCH Add Order with MPID updates order book]==] [==[ITCH Add Orders aggregate at same price]==] [==[ITCH Add Orders create correct BBO]==] [==[ITCH Add Orders maintain independent instrument books]==])
