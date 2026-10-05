add_test( [==[ITCH C execution publishes trade using explicit execution price]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchTradePublicationTest [==[ITCH C execution publishes trade using explicit execution price]==]  )
set_tests_properties( [==[ITCH C execution publishes trade using explicit execution price]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH E execution publishes trade using resting order data]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchTradePublicationTest [==[ITCH E execution publishes trade using resting order data]==]  )
set_tests_properties( [==[ITCH E execution publishes trade using resting order data]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH P message publishes trade without modifying order state]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchTradePublicationTest [==[ITCH P message publishes trade without modifying order state]==]  )
set_tests_properties( [==[ITCH P message publishes trade without modifying order state]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH full E execution publishes trade and removes order]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchTradePublicationTest [==[ITCH full E execution publishes trade and removes order]==]  )
set_tests_properties( [==[ITCH full E execution publishes trade and removes order]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchMarketState and ItchTradePublisher publish E end to end]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchTradePublicationTest [==[ItchMarketState and ItchTradePublisher publish E end to end]==]  )
set_tests_properties( [==[ItchMarketState and ItchTradePublisher publish E end to end]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchTradePublisher publishes normalized Trade into MarketEventQueue]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchTradePublicationTest [==[ItchTradePublisher publishes normalized Trade into MarketEventQueue]==]  )
set_tests_properties( [==[ItchTradePublisher publishes normalized Trade into MarketEventQueue]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchTradePublisher rejects unknown instrument]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchTradePublicationTest [==[ItchTradePublisher rejects unknown instrument]==]  )
set_tests_properties( [==[ItchTradePublisher rejects unknown instrument]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[Over-executed ITCH C order does not publish trade]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchTradePublicationTest [==[Over-executed ITCH C order does not publish trade]==]  )
set_tests_properties( [==[Over-executed ITCH C order does not publish trade]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[Over-executed ITCH E order does not publish trade]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchTradePublicationTest [==[Over-executed ITCH E order does not publish trade]==]  )
set_tests_properties( [==[Over-executed ITCH E order does not publish trade]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[Unknown ITCH C order does not publish trade]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchTradePublicationTest [==[Unknown ITCH C order does not publish trade]==]  )
set_tests_properties( [==[Unknown ITCH C order does not publish trade]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[Unknown ITCH E order does not publish trade]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchTradePublicationTest [==[Unknown ITCH E order does not publish trade]==]  )
set_tests_properties( [==[Unknown ITCH E order does not publish trade]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
set( ItchTradePublicationTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "ITCH C execution publishes trade using explicit execution price",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchTradePublicationTest.cpp",
      "line" : 347
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH E execution publishes trade using resting order data",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchTradePublicationTest.cpp",
      "line" : 219
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH P message publishes trade without modifying order state",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchTradePublicationTest.cpp",
      "line" : 432
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH full E execution publishes trade and removes order",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchTradePublicationTest.cpp",
      "line" : 295
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchMarketState and ItchTradePublisher publish E end to end",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchTradePublicationTest.cpp",
      "line" : 765
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchTradePublisher publishes normalized Trade into MarketEventQueue",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchTradePublicationTest.cpp",
      "line" : 647
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchTradePublisher rejects unknown instrument",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchTradePublicationTest.cpp",
      "line" : 730
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Over-executed ITCH C order does not publish trade",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchTradePublicationTest.cpp",
      "line" : 601
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Over-executed ITCH E order does not publish trade",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchTradePublicationTest.cpp",
      "line" : 553
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Unknown ITCH C order does not publish trade",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchTradePublicationTest.cpp",
      "line" : 524
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Unknown ITCH E order does not publish trade",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchTradePublicationTest.cpp",
      "line" : 497
    },
    "tags" : 
  }
]==] [==[ITCH C execution publishes trade using explicit execution price]==] [==[ITCH E execution publishes trade using resting order data]==] [==[ITCH P message publishes trade without modifying order state]==] [==[ITCH full E execution publishes trade and removes order]==] [==[ItchMarketState and ItchTradePublisher publish E end to end]==] [==[ItchTradePublisher publishes normalized Trade into MarketEventQueue]==] [==[ItchTradePublisher rejects unknown instrument]==] [==[Over-executed ITCH C order does not publish trade]==] [==[Over-executed ITCH E order does not publish trade]==] [==[Unknown ITCH C order does not publish trade]==] [==[Unknown ITCH E order does not publish trade]==])
