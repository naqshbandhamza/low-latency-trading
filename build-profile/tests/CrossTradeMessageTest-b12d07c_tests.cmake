add_test( [==[Cross Trade preserves eight character stock]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/CrossTradeMessageTest [==[Cross Trade preserves eight character stock]==]  )
set_tests_properties( [==[Cross Trade preserves eight character stock]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder decodes Cross Trade message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/CrossTradeMessageTest [==[ItchDecoder decodes Cross Trade message]==]  )
set_tests_properties( [==[ItchDecoder decodes Cross Trade message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder decodes maximum Cross Trade shares]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/CrossTradeMessageTest [==[ItchDecoder decodes maximum Cross Trade shares]==]  )
set_tests_properties( [==[ItchDecoder decodes maximum Cross Trade shares]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder preserves Cross Trade type]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/CrossTradeMessageTest [==[ItchDecoder preserves Cross Trade type]==]  )
set_tests_properties( [==[ItchDecoder preserves Cross Trade type]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects null Cross Trade data]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/CrossTradeMessageTest [==[ItchDecoder rejects null Cross Trade data]==]  )
set_tests_properties( [==[ItchDecoder rejects null Cross Trade data]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects oversized Cross Trade message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/CrossTradeMessageTest [==[ItchDecoder rejects oversized Cross Trade message]==]  )
set_tests_properties( [==[ItchDecoder rejects oversized Cross Trade message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects truncated Cross Trade message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/CrossTradeMessageTest [==[ItchDecoder rejects truncated Cross Trade message]==]  )
set_tests_properties( [==[ItchDecoder rejects truncated Cross Trade message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects wrong Cross Trade message type]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/CrossTradeMessageTest [==[ItchDecoder rejects wrong Cross Trade message type]==]  )
set_tests_properties( [==[ItchDecoder rejects wrong Cross Trade message type]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDispatcher routes Cross Trade message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/CrossTradeMessageTest [==[ItchDispatcher routes Cross Trade message]==]  )
set_tests_properties( [==[ItchDispatcher routes Cross Trade message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchReplay counts Cross Trade as supported]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/CrossTradeMessageTest [==[ItchReplay counts Cross Trade as supported]==]  )
set_tests_properties( [==[ItchReplay counts Cross Trade as supported]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
set( CrossTradeMessageTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "Cross Trade preserves eight character stock",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/CrossTradeMessageTest.cpp",
      "line" : 101
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder decodes Cross Trade message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/CrossTradeMessageTest.cpp",
      "line" : 58
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder decodes maximum Cross Trade shares",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/CrossTradeMessageTest.cpp",
      "line" : 210
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder preserves Cross Trade type",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/CrossTradeMessageTest.cpp",
      "line" : 240
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects null Cross Trade data",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/CrossTradeMessageTest.cpp",
      "line" : 195
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects oversized Cross Trade message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/CrossTradeMessageTest.cpp",
      "line" : 157
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects truncated Cross Trade message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/CrossTradeMessageTest.cpp",
      "line" : 139
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects wrong Cross Trade message type",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/CrossTradeMessageTest.cpp",
      "line" : 175
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDispatcher routes Cross Trade message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/CrossTradeMessageTest.cpp",
      "line" : 262
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchReplay counts Cross Trade as supported",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/CrossTradeMessageTest.cpp",
      "line" : 284
    },
    "tags" : 
  }
]==] [==[Cross Trade preserves eight character stock]==] [==[ItchDecoder decodes Cross Trade message]==] [==[ItchDecoder decodes maximum Cross Trade shares]==] [==[ItchDecoder preserves Cross Trade type]==] [==[ItchDecoder rejects null Cross Trade data]==] [==[ItchDecoder rejects oversized Cross Trade message]==] [==[ItchDecoder rejects truncated Cross Trade message]==] [==[ItchDecoder rejects wrong Cross Trade message type]==] [==[ItchDispatcher routes Cross Trade message]==] [==[ItchReplay counts Cross Trade as supported]==])
