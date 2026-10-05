add_test( [==[PriceLevel adds order quantity]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/PriceLevelTest [==[PriceLevel adds order quantity]==]  )
set_tests_properties( [==[PriceLevel adds order quantity]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[PriceLevel aggregates multiple orders]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/PriceLevelTest [==[PriceLevel aggregates multiple orders]==]  )
set_tests_properties( [==[PriceLevel aggregates multiple orders]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[PriceLevel becomes empty after last order removed]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/PriceLevelTest [==[PriceLevel becomes empty after last order removed]==]  )
set_tests_properties( [==[PriceLevel becomes empty after last order removed]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[PriceLevel handles partial execution then order removal]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/PriceLevelTest [==[PriceLevel handles partial execution then order removal]==]  )
set_tests_properties( [==[PriceLevel handles partial execution then order removal]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[PriceLevel reduces quantity without removing order]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/PriceLevelTest [==[PriceLevel reduces quantity without removing order]==]  )
set_tests_properties( [==[PriceLevel reduces quantity without removing order]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[PriceLevel rejects reduction larger than quantity]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/PriceLevelTest [==[PriceLevel rejects reduction larger than quantity]==]  )
set_tests_properties( [==[PriceLevel rejects reduction larger than quantity]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[PriceLevel rejects removing order when empty]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/PriceLevelTest [==[PriceLevel rejects removing order when empty]==]  )
set_tests_properties( [==[PriceLevel rejects removing order when empty]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[PriceLevel removes remaining order quantity]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/PriceLevelTest [==[PriceLevel removes remaining order quantity]==]  )
set_tests_properties( [==[PriceLevel removes remaining order quantity]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[PriceLevel starts empty]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/PriceLevelTest [==[PriceLevel starts empty]==]  )
set_tests_properties( [==[PriceLevel starts empty]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
set( PriceLevelTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "PriceLevel adds order quantity",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/PriceLevelTest.cpp",
      "line" : 25
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "PriceLevel aggregates multiple orders",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/PriceLevelTest.cpp",
      "line" : 41
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "PriceLevel becomes empty after last order removed",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/PriceLevelTest.cpp",
      "line" : 115
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "PriceLevel handles partial execution then order removal",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/PriceLevelTest.cpp",
      "line" : 152
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "PriceLevel reduces quantity without removing order",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/PriceLevelTest.cpp",
      "line" : 58
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "PriceLevel rejects reduction larger than quantity",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/PriceLevelTest.cpp",
      "line" : 75
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "PriceLevel rejects removing order when empty",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/PriceLevelTest.cpp",
      "line" : 135
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "PriceLevel removes remaining order quantity",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/PriceLevelTest.cpp",
      "line" : 94
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "PriceLevel starts empty",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/PriceLevelTest.cpp",
      "line" : 10
    },
    "tags" : 
  }
]==] [==[PriceLevel adds order quantity]==] [==[PriceLevel aggregates multiple orders]==] [==[PriceLevel becomes empty after last order removed]==] [==[PriceLevel handles partial execution then order removal]==] [==[PriceLevel reduces quantity without removing order]==] [==[PriceLevel rejects reduction larger than quantity]==] [==[PriceLevel rejects removing order when empty]==] [==[PriceLevel removes remaining order quantity]==] [==[PriceLevel starts empty]==])
