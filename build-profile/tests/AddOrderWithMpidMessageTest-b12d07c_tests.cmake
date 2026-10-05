add_test( [==[Add Order with MPID trims padded attribution]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/AddOrderWithMpidMessageTest [==[Add Order with MPID trims padded attribution]==]  )
set_tests_properties( [==[Add Order with MPID trims padded attribution]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder decodes Add Order with MPID]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/AddOrderWithMpidMessageTest [==[ItchDecoder decodes Add Order with MPID]==]  )
set_tests_properties( [==[ItchDecoder decodes Add Order with MPID]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects null Add Order with MPID data]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/AddOrderWithMpidMessageTest [==[ItchDecoder rejects null Add Order with MPID data]==]  )
set_tests_properties( [==[ItchDecoder rejects null Add Order with MPID data]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects oversized Add Order with MPID]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/AddOrderWithMpidMessageTest [==[ItchDecoder rejects oversized Add Order with MPID]==]  )
set_tests_properties( [==[ItchDecoder rejects oversized Add Order with MPID]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects truncated Add Order with MPID]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/AddOrderWithMpidMessageTest [==[ItchDecoder rejects truncated Add Order with MPID]==]  )
set_tests_properties( [==[ItchDecoder rejects truncated Add Order with MPID]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects wrong Add Order with MPID type]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/AddOrderWithMpidMessageTest [==[ItchDecoder rejects wrong Add Order with MPID type]==]  )
set_tests_properties( [==[ItchDecoder rejects wrong Add Order with MPID type]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDispatcher routes Add Order with MPID]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/AddOrderWithMpidMessageTest [==[ItchDispatcher routes Add Order with MPID]==]  )
set_tests_properties( [==[ItchDispatcher routes Add Order with MPID]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchReplay counts Add Order with MPID as supported]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/AddOrderWithMpidMessageTest [==[ItchReplay counts Add Order with MPID as supported]==]  )
set_tests_properties( [==[ItchReplay counts Add Order with MPID as supported]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
set( AddOrderWithMpidMessageTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "Add Order with MPID trims padded attribution",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/AddOrderWithMpidMessageTest.cpp",
      "line" : 195
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder decodes Add Order with MPID",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/AddOrderWithMpidMessageTest.cpp",
      "line" : 65
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects null Add Order with MPID data",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/AddOrderWithMpidMessageTest.cpp",
      "line" : 179
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects oversized Add Order with MPID",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/AddOrderWithMpidMessageTest.cpp",
      "line" : 140
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects truncated Add Order with MPID",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/AddOrderWithMpidMessageTest.cpp",
      "line" : 122
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects wrong Add Order with MPID type",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/AddOrderWithMpidMessageTest.cpp",
      "line" : 159
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDispatcher routes Add Order with MPID",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/AddOrderWithMpidMessageTest.cpp",
      "line" : 222
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchReplay counts Add Order with MPID as supported",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/AddOrderWithMpidMessageTest.cpp",
      "line" : 246
    },
    "tags" : 
  }
]==] [==[Add Order with MPID trims padded attribution]==] [==[ItchDecoder decodes Add Order with MPID]==] [==[ItchDecoder rejects null Add Order with MPID data]==] [==[ItchDecoder rejects oversized Add Order with MPID]==] [==[ItchDecoder rejects truncated Add Order with MPID]==] [==[ItchDecoder rejects wrong Add Order with MPID type]==] [==[ItchDispatcher routes Add Order with MPID]==] [==[ItchReplay counts Add Order with MPID as supported]==])
