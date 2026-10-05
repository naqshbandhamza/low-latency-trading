add_test( [==[ItchDecoder decodes Broken Trade message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BrokenTradeMessageTest [==[ItchDecoder decodes Broken Trade message]==]  )
set_tests_properties( [==[ItchDecoder decodes Broken Trade message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder decodes maximum Broken Trade header fields]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BrokenTradeMessageTest [==[ItchDecoder decodes maximum Broken Trade header fields]==]  )
set_tests_properties( [==[ItchDecoder decodes maximum Broken Trade header fields]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder decodes maximum Broken Trade match number]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BrokenTradeMessageTest [==[ItchDecoder decodes maximum Broken Trade match number]==]  )
set_tests_properties( [==[ItchDecoder decodes maximum Broken Trade match number]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects null Broken Trade data]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BrokenTradeMessageTest [==[ItchDecoder rejects null Broken Trade data]==]  )
set_tests_properties( [==[ItchDecoder rejects null Broken Trade data]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects oversized Broken Trade message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BrokenTradeMessageTest [==[ItchDecoder rejects oversized Broken Trade message]==]  )
set_tests_properties( [==[ItchDecoder rejects oversized Broken Trade message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects truncated Broken Trade message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BrokenTradeMessageTest [==[ItchDecoder rejects truncated Broken Trade message]==]  )
set_tests_properties( [==[ItchDecoder rejects truncated Broken Trade message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects wrong Broken Trade message type]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BrokenTradeMessageTest [==[ItchDecoder rejects wrong Broken Trade message type]==]  )
set_tests_properties( [==[ItchDecoder rejects wrong Broken Trade message type]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDispatcher rejects malformed Broken Trade message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BrokenTradeMessageTest [==[ItchDispatcher rejects malformed Broken Trade message]==]  )
set_tests_properties( [==[ItchDispatcher rejects malformed Broken Trade message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDispatcher routes Broken Trade message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BrokenTradeMessageTest [==[ItchDispatcher routes Broken Trade message]==]  )
set_tests_properties( [==[ItchDispatcher routes Broken Trade message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchReplay counts Broken Trade as supported]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BrokenTradeMessageTest [==[ItchReplay counts Broken Trade as supported]==]  )
set_tests_properties( [==[ItchReplay counts Broken Trade as supported]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( BrokenTradeMessageTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder decodes Broken Trade message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/BrokenTradeMessageTest.cpp",
      "line" : 46
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder decodes maximum Broken Trade header fields",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/BrokenTradeMessageTest.cpp",
      "line" : 192
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder decodes maximum Broken Trade match number",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/BrokenTradeMessageTest.cpp",
      "line" : 161
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects null Broken Trade data",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/BrokenTradeMessageTest.cpp",
      "line" : 145
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects oversized Broken Trade message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/BrokenTradeMessageTest.cpp",
      "line" : 104
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects truncated Broken Trade message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/BrokenTradeMessageTest.cpp",
      "line" : 85
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects wrong Broken Trade message type",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/BrokenTradeMessageTest.cpp",
      "line" : 124
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDispatcher rejects malformed Broken Trade message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/BrokenTradeMessageTest.cpp",
      "line" : 275
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDispatcher routes Broken Trade message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/BrokenTradeMessageTest.cpp",
      "line" : 242
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchReplay counts Broken Trade as supported",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/BrokenTradeMessageTest.cpp",
      "line" : 294
    },
    "tags" : 
  }
]==] [==[ItchDecoder decodes Broken Trade message]==] [==[ItchDecoder decodes maximum Broken Trade header fields]==] [==[ItchDecoder decodes maximum Broken Trade match number]==] [==[ItchDecoder rejects null Broken Trade data]==] [==[ItchDecoder rejects oversized Broken Trade message]==] [==[ItchDecoder rejects truncated Broken Trade message]==] [==[ItchDecoder rejects wrong Broken Trade message type]==] [==[ItchDispatcher rejects malformed Broken Trade message]==] [==[ItchDispatcher routes Broken Trade message]==] [==[ItchReplay counts Broken Trade as supported]==])
