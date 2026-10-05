add_test( [==[ItchDecoder decodes Trade message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/TradeMessageTest [==[ItchDecoder decodes Trade message]==]  )
set_tests_properties( [==[ItchDecoder decodes Trade message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder decodes maximum Trade numeric fields]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/TradeMessageTest [==[ItchDecoder decodes maximum Trade numeric fields]==]  )
set_tests_properties( [==[ItchDecoder decodes maximum Trade numeric fields]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects null Trade message data]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/TradeMessageTest [==[ItchDecoder rejects null Trade message data]==]  )
set_tests_properties( [==[ItchDecoder rejects null Trade message data]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects oversized Trade message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/TradeMessageTest [==[ItchDecoder rejects oversized Trade message]==]  )
set_tests_properties( [==[ItchDecoder rejects oversized Trade message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects truncated Trade message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/TradeMessageTest [==[ItchDecoder rejects truncated Trade message]==]  )
set_tests_properties( [==[ItchDecoder rejects truncated Trade message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects wrong Trade message type]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/TradeMessageTest [==[ItchDecoder rejects wrong Trade message type]==]  )
set_tests_properties( [==[ItchDecoder rejects wrong Trade message type]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDispatcher routes Trade message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/TradeMessageTest [==[ItchDispatcher routes Trade message]==]  )
set_tests_properties( [==[ItchDispatcher routes Trade message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchReplay counts Trade message as supported]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/TradeMessageTest [==[ItchReplay counts Trade message as supported]==]  )
set_tests_properties( [==[ItchReplay counts Trade message as supported]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[Trade message preserves eight character stock]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/TradeMessageTest [==[Trade message preserves eight character stock]==]  )
set_tests_properties( [==[Trade message preserves eight character stock]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
set( TradeMessageTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder decodes Trade message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/TradeMessageTest.cpp",
      "line" : 61
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder decodes maximum Trade numeric fields",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/TradeMessageTest.cpp",
      "line" : 218
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects null Trade message data",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/TradeMessageTest.cpp",
      "line" : 203
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects oversized Trade message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/TradeMessageTest.cpp",
      "line" : 165
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects truncated Trade message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/TradeMessageTest.cpp",
      "line" : 147
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects wrong Trade message type",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/TradeMessageTest.cpp",
      "line" : 183
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDispatcher routes Trade message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/TradeMessageTest.cpp",
      "line" : 272
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchReplay counts Trade message as supported",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/TradeMessageTest.cpp",
      "line" : 294
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Trade message preserves eight character stock",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/TradeMessageTest.cpp",
      "line" : 109
    },
    "tags" : 
  }
]==] [==[ItchDecoder decodes Trade message]==] [==[ItchDecoder decodes maximum Trade numeric fields]==] [==[ItchDecoder rejects null Trade message data]==] [==[ItchDecoder rejects oversized Trade message]==] [==[ItchDecoder rejects truncated Trade message]==] [==[ItchDecoder rejects wrong Trade message type]==] [==[ItchDispatcher routes Trade message]==] [==[ItchReplay counts Trade message as supported]==] [==[Trade message preserves eight character stock]==])
