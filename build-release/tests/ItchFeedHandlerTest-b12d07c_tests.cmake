add_test( [==[ITCH feed handler fails on malformed ITCH payload]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchFeedHandlerTest [==[ITCH feed handler fails on malformed ITCH payload]==]  )
set_tests_properties( [==[ITCH feed handler fails on malformed ITCH payload]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH feed handler fails when recovered packet is malformed]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchFeedHandlerTest [==[ITCH feed handler fails when recovered packet is malformed]==]  )
set_tests_properties( [==[ITCH feed handler fails when recovered packet is malformed]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH feed handler fails when sequence recovery fails]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchFeedHandlerTest [==[ITCH feed handler fails when sequence recovery fails]==]  )
set_tests_properties( [==[ITCH feed handler fails when sequence recovery fails]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH feed handler ignores burst of stale packets]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchFeedHandlerTest [==[ITCH feed handler ignores burst of stale packets]==]  )
set_tests_properties( [==[ITCH feed handler ignores burst of stale packets]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH feed handler ignores duplicate packet]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchFeedHandlerTest [==[ITCH feed handler ignores duplicate packet]==]  )
set_tests_properties( [==[ITCH feed handler ignores duplicate packet]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH feed handler ignores late packet after recovery]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchFeedHandlerTest [==[ITCH feed handler ignores late packet after recovery]==]  )
set_tests_properties( [==[ITCH feed handler ignores late packet after recovery]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH feed handler ignores old packet]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchFeedHandlerTest [==[ITCH feed handler ignores old packet]==]  )
set_tests_properties( [==[ITCH feed handler ignores old packet]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH feed handler processes contiguous packets]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchFeedHandlerTest [==[ITCH feed handler processes contiguous packets]==]  )
set_tests_properties( [==[ITCH feed handler processes contiguous packets]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH feed handler processes first packet]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchFeedHandlerTest [==[ITCH feed handler processes first packet]==]  )
set_tests_properties( [==[ITCH feed handler processes first packet]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH feed handler recovers multiple missing packets]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchFeedHandlerTest [==[ITCH feed handler recovers multiple missing packets]==]  )
set_tests_properties( [==[ITCH feed handler recovers multiple missing packets]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH feed handler recovers single missing packet]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchFeedHandlerTest [==[ITCH feed handler recovers single missing packet]==]  )
set_tests_properties( [==[ITCH feed handler recovers single missing packet]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH feed handler supports arbitrary starting sequence]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchFeedHandlerTest [==[ITCH feed handler supports arbitrary starting sequence]==]  )
set_tests_properties( [==[ITCH feed handler supports arbitrary starting sequence]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( ItchFeedHandlerTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "ITCH feed handler fails on malformed ITCH payload",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchFeedHandlerTest.cpp",
      "line" : 508
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH feed handler fails when recovered packet is malformed",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchFeedHandlerTest.cpp",
      "line" : 548
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH feed handler fails when sequence recovery fails",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchFeedHandlerTest.cpp",
      "line" : 449
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH feed handler ignores burst of stale packets",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchFeedHandlerTest.cpp",
      "line" : 771
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH feed handler ignores duplicate packet",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchFeedHandlerTest.cpp",
      "line" : 261
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH feed handler ignores late packet after recovery",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchFeedHandlerTest.cpp",
      "line" : 669
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH feed handler ignores old packet",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchFeedHandlerTest.cpp",
      "line" : 598
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH feed handler processes contiguous packets",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchFeedHandlerTest.cpp",
      "line" : 169
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH feed handler processes first packet",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchFeedHandlerTest.cpp",
      "line" : 121
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH feed handler recovers multiple missing packets",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchFeedHandlerTest.cpp",
      "line" : 386
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH feed handler recovers single missing packet",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchFeedHandlerTest.cpp",
      "line" : 324
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH feed handler supports arbitrary starting sequence",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchFeedHandlerTest.cpp",
      "line" : 219
    },
    "tags" : 
  }
]==] [==[ITCH feed handler fails on malformed ITCH payload]==] [==[ITCH feed handler fails when recovered packet is malformed]==] [==[ITCH feed handler fails when sequence recovery fails]==] [==[ITCH feed handler ignores burst of stale packets]==] [==[ITCH feed handler ignores duplicate packet]==] [==[ITCH feed handler ignores late packet after recovery]==] [==[ITCH feed handler ignores old packet]==] [==[ITCH feed handler processes contiguous packets]==] [==[ITCH feed handler processes first packet]==] [==[ITCH feed handler recovers multiple missing packets]==] [==[ITCH feed handler recovers single missing packet]==] [==[ITCH feed handler supports arbitrary starting sequence]==])
