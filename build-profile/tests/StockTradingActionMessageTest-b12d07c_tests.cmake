add_test( [==[ItchDecoder decodes Stock Trading Action message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/StockTradingActionMessageTest [==[ItchDecoder decodes Stock Trading Action message]==]  )
set_tests_properties( [==[ItchDecoder decodes Stock Trading Action message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder decodes maximum Stock Trading Action header fields]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/StockTradingActionMessageTest [==[ItchDecoder decodes maximum Stock Trading Action header fields]==]  )
set_tests_properties( [==[ItchDecoder decodes maximum Stock Trading Action header fields]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects null Stock Trading Action data]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/StockTradingActionMessageTest [==[ItchDecoder rejects null Stock Trading Action data]==]  )
set_tests_properties( [==[ItchDecoder rejects null Stock Trading Action data]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects oversized Stock Trading Action]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/StockTradingActionMessageTest [==[ItchDecoder rejects oversized Stock Trading Action]==]  )
set_tests_properties( [==[ItchDecoder rejects oversized Stock Trading Action]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects truncated Stock Trading Action]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/StockTradingActionMessageTest [==[ItchDecoder rejects truncated Stock Trading Action]==]  )
set_tests_properties( [==[ItchDecoder rejects truncated Stock Trading Action]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects wrong Stock Trading Action type]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/StockTradingActionMessageTest [==[ItchDecoder rejects wrong Stock Trading Action type]==]  )
set_tests_properties( [==[ItchDecoder rejects wrong Stock Trading Action type]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDispatcher rejects malformed Stock Trading Action]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/StockTradingActionMessageTest [==[ItchDispatcher rejects malformed Stock Trading Action]==]  )
set_tests_properties( [==[ItchDispatcher rejects malformed Stock Trading Action]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDispatcher routes Stock Trading Action]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/StockTradingActionMessageTest [==[ItchDispatcher routes Stock Trading Action]==]  )
set_tests_properties( [==[ItchDispatcher routes Stock Trading Action]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchReplay counts Stock Trading Action as supported]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/StockTradingActionMessageTest [==[ItchReplay counts Stock Trading Action as supported]==]  )
set_tests_properties( [==[ItchReplay counts Stock Trading Action as supported]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[Stock Trading Action preserves eight character stock]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/StockTradingActionMessageTest [==[Stock Trading Action preserves eight character stock]==]  )
set_tests_properties( [==[Stock Trading Action preserves eight character stock]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[Stock Trading Action preserves reason code]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/StockTradingActionMessageTest [==[Stock Trading Action preserves reason code]==]  )
set_tests_properties( [==[Stock Trading Action preserves reason code]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[Stock Trading Action preserves trading state]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/StockTradingActionMessageTest [==[Stock Trading Action preserves trading state]==]  )
set_tests_properties( [==[Stock Trading Action preserves trading state]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[Stock Trading Action trims padded reason]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/StockTradingActionMessageTest [==[Stock Trading Action trims padded reason]==]  )
set_tests_properties( [==[Stock Trading Action trims padded reason]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
set( StockTradingActionMessageTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder decodes Stock Trading Action message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/StockTradingActionMessageTest.cpp",
      "line" : 55
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder decodes maximum Stock Trading Action header fields",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/StockTradingActionMessageTest.cpp",
      "line" : 283
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects null Stock Trading Action data",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/StockTradingActionMessageTest.cpp",
      "line" : 267
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects oversized Stock Trading Action",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/StockTradingActionMessageTest.cpp",
      "line" : 226
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects truncated Stock Trading Action",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/StockTradingActionMessageTest.cpp",
      "line" : 207
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects wrong Stock Trading Action type",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/StockTradingActionMessageTest.cpp",
      "line" : 246
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDispatcher rejects malformed Stock Trading Action",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/StockTradingActionMessageTest.cpp",
      "line" : 361
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDispatcher routes Stock Trading Action",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/StockTradingActionMessageTest.cpp",
      "line" : 330
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchReplay counts Stock Trading Action as supported",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/StockTradingActionMessageTest.cpp",
      "line" : 378
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Stock Trading Action preserves eight character stock",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/StockTradingActionMessageTest.cpp",
      "line" : 168
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Stock Trading Action preserves reason code",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/StockTradingActionMessageTest.cpp",
      "line" : 110
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Stock Trading Action preserves trading state",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/StockTradingActionMessageTest.cpp",
      "line" : 87
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Stock Trading Action trims padded reason",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/StockTradingActionMessageTest.cpp",
      "line" : 140
    },
    "tags" : 
  }
]==] [==[ItchDecoder decodes Stock Trading Action message]==] [==[ItchDecoder decodes maximum Stock Trading Action header fields]==] [==[ItchDecoder rejects null Stock Trading Action data]==] [==[ItchDecoder rejects oversized Stock Trading Action]==] [==[ItchDecoder rejects truncated Stock Trading Action]==] [==[ItchDecoder rejects wrong Stock Trading Action type]==] [==[ItchDispatcher rejects malformed Stock Trading Action]==] [==[ItchDispatcher routes Stock Trading Action]==] [==[ItchReplay counts Stock Trading Action as supported]==] [==[Stock Trading Action preserves eight character stock]==] [==[Stock Trading Action preserves reason code]==] [==[Stock Trading Action preserves trading state]==] [==[Stock Trading Action trims padded reason]==])
