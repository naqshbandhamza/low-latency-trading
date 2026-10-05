add_test( [==[ITCH quote publisher assigns increasing sequence numbers]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchQuotePublisherTest [==[ITCH quote publisher assigns increasing sequence numbers]==]  )
set_tests_properties( [==[ITCH quote publisher assigns increasing sequence numbers]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH quote publisher does not publish ask only BBO]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchQuotePublisherTest [==[ITCH quote publisher does not publish ask only BBO]==]  )
set_tests_properties( [==[ITCH quote publisher does not publish ask only BBO]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH quote publisher does not publish bid only BBO]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchQuotePublisherTest [==[ITCH quote publisher does not publish bid only BBO]==]  )
set_tests_properties( [==[ITCH quote publisher does not publish bid only BBO]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH quote publisher publishes normalized quote]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchQuotePublisherTest [==[ITCH quote publisher publishes normalized quote]==]  )
set_tests_properties( [==[ITCH quote publisher publishes normalized quote]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH quote publisher records unknown instrument diagnostics]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchQuotePublisherTest [==[ITCH quote publisher records unknown instrument diagnostics]==]  )
set_tests_properties( [==[ITCH quote publisher records unknown instrument diagnostics]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH quote publisher rejects unknown instrument]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchQuotePublisherTest [==[ITCH quote publisher rejects unknown instrument]==]  )
set_tests_properties( [==[ITCH quote publisher rejects unknown instrument]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( ItchQuotePublisherTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "ITCH quote publisher assigns increasing sequence numbers",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchQuotePublisherTest.cpp",
      "line" : 263
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH quote publisher does not publish ask only BBO",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchQuotePublisherTest.cpp",
      "line" : 220
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH quote publisher does not publish bid only BBO",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchQuotePublisherTest.cpp",
      "line" : 177
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH quote publisher publishes normalized quote",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchQuotePublisherTest.cpp",
      "line" : 62
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH quote publisher records unknown instrument diagnostics",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchQuotePublisherTest.cpp",
      "line" : 352
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH quote publisher rejects unknown instrument",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchQuotePublisherTest.cpp",
      "line" : 142
    },
    "tags" : 
  }
]==] [==[ITCH quote publisher assigns increasing sequence numbers]==] [==[ITCH quote publisher does not publish ask only BBO]==] [==[ITCH quote publisher does not publish bid only BBO]==] [==[ITCH quote publisher publishes normalized quote]==] [==[ITCH quote publisher records unknown instrument diagnostics]==] [==[ITCH quote publisher rejects unknown instrument]==])
