add_test( [==[OrderBook aggregates quantity at best bid]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/OrderBookTest [==[OrderBook aggregates quantity at best bid]==]  )
set_tests_properties( [==[OrderBook aggregates quantity at best bid]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[OrderBook becomes empty after both sides removed]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/OrderBookTest [==[OrderBook becomes empty after both sides removed]==]  )
set_tests_properties( [==[OrderBook becomes empty after both sides removed]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[OrderBook best ask changes when level disappears]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/OrderBookTest [==[OrderBook best ask changes when level disappears]==]  )
set_tests_properties( [==[OrderBook best ask changes when level disappears]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[OrderBook best bid changes when level disappears]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/OrderBookTest [==[OrderBook best bid changes when level disappears]==]  )
set_tests_properties( [==[OrderBook best bid changes when level disappears]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[OrderBook can have only ask side]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/OrderBookTest [==[OrderBook can have only ask side]==]  )
set_tests_properties( [==[OrderBook can have only ask side]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[OrderBook can have only bid side]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/OrderBookTest [==[OrderBook can have only bid side]==]  )
set_tests_properties( [==[OrderBook can have only bid side]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[OrderBook produces best bid and ask]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/OrderBookTest [==[OrderBook produces best bid and ask]==]  )
set_tests_properties( [==[OrderBook produces best bid and ask]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[OrderBook starts empty]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/OrderBookTest [==[OrderBook starts empty]==]  )
set_tests_properties( [==[OrderBook starts empty]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[OrderBook stores ask]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/OrderBookTest [==[OrderBook stores ask]==]  )
set_tests_properties( [==[OrderBook stores ask]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[OrderBook stores bid]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/OrderBookTest [==[OrderBook stores bid]==]  )
set_tests_properties( [==[OrderBook stores bid]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
set( OrderBookTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "OrderBook aggregates quantity at best bid",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/OrderBookTest.cpp",
      "line" : 114
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "OrderBook becomes empty after both sides removed",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/OrderBookTest.cpp",
      "line" : 255
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "OrderBook best ask changes when level disappears",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/OrderBookTest.cpp",
      "line" : 176
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "OrderBook best bid changes when level disappears",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/OrderBookTest.cpp",
      "line" : 139
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "OrderBook can have only ask side",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/OrderBookTest.cpp",
      "line" : 234
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "OrderBook can have only bid side",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/OrderBookTest.cpp",
      "line" : 213
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "OrderBook produces best bid and ask",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/OrderBookTest.cpp",
      "line" : 75
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "OrderBook starts empty",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/OrderBookTest.cpp",
      "line" : 10
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "OrderBook stores ask",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/OrderBookTest.cpp",
      "line" : 53
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "OrderBook stores bid",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/OrderBookTest.cpp",
      "line" : 29
    },
    "tags" : 
  }
]==] [==[OrderBook aggregates quantity at best bid]==] [==[OrderBook becomes empty after both sides removed]==] [==[OrderBook best ask changes when level disappears]==] [==[OrderBook best bid changes when level disappears]==] [==[OrderBook can have only ask side]==] [==[OrderBook can have only bid side]==] [==[OrderBook produces best bid and ask]==] [==[OrderBook starts empty]==] [==[OrderBook stores ask]==] [==[OrderBook stores bid]==])
