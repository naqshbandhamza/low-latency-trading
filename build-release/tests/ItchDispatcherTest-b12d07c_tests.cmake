add_test( [==[ITCH dispatcher rejects known message type with incorrect length]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchDispatcherTest [==[ITCH dispatcher rejects known message type with incorrect length]==]  )
set_tests_properties( [==[ITCH dispatcher rejects known message type with incorrect length]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH dispatcher rejects null data]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchDispatcherTest [==[ITCH dispatcher rejects null data]==]  )
set_tests_properties( [==[ITCH dispatcher rejects null data]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH dispatcher rejects unsupported message type]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchDispatcherTest [==[ITCH dispatcher rejects unsupported message type]==]  )
set_tests_properties( [==[ITCH dispatcher rejects unsupported message type]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH dispatcher rejects zero length message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchDispatcherTest [==[ITCH dispatcher rejects zero length message]==]  )
set_tests_properties( [==[ITCH dispatcher rejects zero length message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH dispatcher routes Add Order message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchDispatcherTest [==[ITCH dispatcher routes Add Order message]==]  )
set_tests_properties( [==[ITCH dispatcher routes Add Order message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH dispatcher routes Order Cancel message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchDispatcherTest [==[ITCH dispatcher routes Order Cancel message]==]  )
set_tests_properties( [==[ITCH dispatcher routes Order Cancel message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH dispatcher routes Order Delete message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchDispatcherTest [==[ITCH dispatcher routes Order Delete message]==]  )
set_tests_properties( [==[ITCH dispatcher routes Order Delete message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH dispatcher routes Order Executed With Price message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchDispatcherTest [==[ITCH dispatcher routes Order Executed With Price message]==]  )
set_tests_properties( [==[ITCH dispatcher routes Order Executed With Price message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH dispatcher routes Order Executed message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchDispatcherTest [==[ITCH dispatcher routes Order Executed message]==]  )
set_tests_properties( [==[ITCH dispatcher routes Order Executed message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH dispatcher routes Order Replace message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchDispatcherTest [==[ITCH dispatcher routes Order Replace message]==]  )
set_tests_properties( [==[ITCH dispatcher routes Order Replace message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH dispatcher routes Stock Directory message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchDispatcherTest [==[ITCH dispatcher routes Stock Directory message]==]  )
set_tests_properties( [==[ITCH dispatcher routes Stock Directory message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH dispatcher routes System Event message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchDispatcherTest [==[ITCH dispatcher routes System Event message]==]  )
set_tests_properties( [==[ITCH dispatcher routes System Event message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( ItchDispatcherTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "ITCH dispatcher rejects known message type with incorrect length",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchDispatcherTest.cpp",
      "line" : 311
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH dispatcher rejects null data",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchDispatcherTest.cpp",
      "line" : 279
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH dispatcher rejects unsupported message type",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchDispatcherTest.cpp",
      "line" : 259
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH dispatcher rejects zero length message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchDispatcherTest.cpp",
      "line" : 294
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH dispatcher routes Add Order message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchDispatcherTest.cpp",
      "line" : 95
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH dispatcher routes Order Cancel message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchDispatcherTest.cpp",
      "line" : 187
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH dispatcher routes Order Delete message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchDispatcherTest.cpp",
      "line" : 211
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH dispatcher routes Order Executed With Price message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchDispatcherTest.cpp",
      "line" : 153
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH dispatcher routes Order Executed message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchDispatcherTest.cpp",
      "line" : 129
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH dispatcher routes Order Replace message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchDispatcherTest.cpp",
      "line" : 235
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH dispatcher routes Stock Directory message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchDispatcherTest.cpp",
      "line" : 53
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH dispatcher routes System Event message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchDispatcherTest.cpp",
      "line" : 21
    },
    "tags" : 
  }
]==] [==[ITCH dispatcher rejects known message type with incorrect length]==] [==[ITCH dispatcher rejects null data]==] [==[ITCH dispatcher rejects unsupported message type]==] [==[ITCH dispatcher rejects zero length message]==] [==[ITCH dispatcher routes Add Order message]==] [==[ITCH dispatcher routes Order Cancel message]==] [==[ITCH dispatcher routes Order Delete message]==] [==[ITCH dispatcher routes Order Executed With Price message]==] [==[ITCH dispatcher routes Order Executed message]==] [==[ITCH dispatcher routes Order Replace message]==] [==[ITCH dispatcher routes Stock Directory message]==] [==[ITCH dispatcher routes System Event message]==])
