add_test( [==[BookSide aggregates orders at same price]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BookSideTest [==[BookSide aggregates orders at same price]==]  )
set_tests_properties( [==[BookSide aggregates orders at same price]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[BookSide creates independent price levels]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BookSideTest [==[BookSide creates independent price levels]==]  )
set_tests_properties( [==[BookSide creates independent price levels]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[BookSide creates price level]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BookSideTest [==[BookSide creates price level]==]  )
set_tests_properties( [==[BookSide creates price level]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[BookSide keeps price level while other orders remain]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BookSideTest [==[BookSide keeps price level while other orders remain]==]  )
set_tests_properties( [==[BookSide keeps price level while other orders remain]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[BookSide reduces price level quantity]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BookSideTest [==[BookSide reduces price level quantity]==]  )
set_tests_properties( [==[BookSide reduces price level quantity]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[BookSide rejects reduction for unknown price]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BookSideTest [==[BookSide rejects reduction for unknown price]==]  )
set_tests_properties( [==[BookSide rejects reduction for unknown price]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[BookSide removes empty price level]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BookSideTest [==[BookSide removes empty price level]==]  )
set_tests_properties( [==[BookSide removes empty price level]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[BookSide starts empty]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BookSideTest [==[BookSide starts empty]==]  )
set_tests_properties( [==[BookSide starts empty]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Buy BookSide best price changes after best level removed]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BookSideTest [==[Buy BookSide best price changes after best level removed]==]  )
set_tests_properties( [==[Buy BookSide best price changes after best level removed]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Buy BookSide chooses highest price as best]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BookSideTest [==[Buy BookSide chooses highest price as best]==]  )
set_tests_properties( [==[Buy BookSide chooses highest price as best]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Sell BookSide best price changes after best level removed]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BookSideTest [==[Sell BookSide best price changes after best level removed]==]  )
set_tests_properties( [==[Sell BookSide best price changes after best level removed]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Sell BookSide chooses lowest price as best]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BookSideTest [==[Sell BookSide chooses lowest price as best]==]  )
set_tests_properties( [==[Sell BookSide chooses lowest price as best]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( BookSideTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "BookSide aggregates orders at same price",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BookSideTest.cpp",
      "line" : 43
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BookSide creates independent price levels",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BookSideTest.cpp",
      "line" : 63
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BookSide creates price level",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BookSideTest.cpp",
      "line" : 22
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BookSide keeps price level while other orders remain",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BookSideTest.cpp",
      "line" : 191
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BookSide reduces price level quantity",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BookSideTest.cpp",
      "line" : 124
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BookSide rejects reduction for unknown price",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BookSideTest.cpp",
      "line" : 148
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BookSide removes empty price level",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BookSideTest.cpp",
      "line" : 170
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BookSide starts empty",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BookSideTest.cpp",
      "line" : 10
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Buy BookSide best price changes after best level removed",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BookSideTest.cpp",
      "line" : 217
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Buy BookSide chooses highest price as best",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BookSideTest.cpp",
      "line" : 86
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Sell BookSide best price changes after best level removed",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BookSideTest.cpp",
      "line" : 246
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Sell BookSide chooses lowest price as best",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BookSideTest.cpp",
      "line" : 105
    },
    "tags" : 
  }
]==] [==[BookSide aggregates orders at same price]==] [==[BookSide creates independent price levels]==] [==[BookSide creates price level]==] [==[BookSide keeps price level while other orders remain]==] [==[BookSide reduces price level quantity]==] [==[BookSide rejects reduction for unknown price]==] [==[BookSide removes empty price level]==] [==[BookSide starts empty]==] [==[Buy BookSide best price changes after best level removed]==] [==[Buy BookSide chooses highest price as best]==] [==[Sell BookSide best price changes after best level removed]==] [==[Sell BookSide chooses lowest price as best]==])
