add_test( [==[UDP codec encodes and decodes quote packet]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/UdpCodecTest [==[UDP codec encodes and decodes quote packet]==]  )
set_tests_properties( [==[UDP codec encodes and decodes quote packet]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[UDP codec encodes and decodes trade packet]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/UdpCodecTest [==[UDP codec encodes and decodes trade packet]==]  )
set_tests_properties( [==[UDP codec encodes and decodes trade packet]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[UDP codec preserves maximum integer values]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/UdpCodecTest [==[UDP codec preserves maximum integer values]==]  )
set_tests_properties( [==[UDP codec preserves maximum integer values]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[UDP codec preserves minimum int64 price]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/UdpCodecTest [==[UDP codec preserves minimum int64 price]==]  )
set_tests_properties( [==[UDP codec preserves minimum int64 price]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[UDP codec preserves negative prices]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/UdpCodecTest [==[UDP codec preserves negative prices]==]  )
set_tests_properties( [==[UDP codec preserves negative prices]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[UDP codec rejects invalid packet side]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/UdpCodecTest [==[UDP codec rejects invalid packet side]==]  )
set_tests_properties( [==[UDP codec rejects invalid packet side]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[UDP codec rejects invalid packet type]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/UdpCodecTest [==[UDP codec rejects invalid packet type]==]  )
set_tests_properties( [==[UDP codec rejects invalid packet type]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[UDP codec rejects truncated packet]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/UdpCodecTest [==[UDP codec rejects truncated packet]==]  )
set_tests_properties( [==[UDP codec rejects truncated packet]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( UdpCodecTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "UDP codec encodes and decodes quote packet",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/UdpMarketDataCodecTest.cpp",
      "line" : 64
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "UDP codec encodes and decodes trade packet",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/UdpMarketDataCodecTest.cpp",
      "line" : 97
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "UDP codec preserves maximum integer values",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/UdpMarketDataCodecTest.cpp",
      "line" : 160
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "UDP codec preserves minimum int64 price",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/UdpMarketDataCodecTest.cpp",
      "line" : 215
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "UDP codec preserves negative prices",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/UdpMarketDataCodecTest.cpp",
      "line" : 130
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "UDP codec rejects invalid packet side",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/UdpMarketDataCodecTest.cpp",
      "line" : 296
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "UDP codec rejects invalid packet type",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/UdpMarketDataCodecTest.cpp",
      "line" : 272
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "UDP codec rejects truncated packet",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/UdpMarketDataCodecTest.cpp",
      "line" : 250
    },
    "tags" : 
  }
]==] [==[UDP codec encodes and decodes quote packet]==] [==[UDP codec encodes and decodes trade packet]==] [==[UDP codec preserves maximum integer values]==] [==[UDP codec preserves minimum int64 price]==] [==[UDP codec preserves negative prices]==] [==[UDP codec rejects invalid packet side]==] [==[UDP codec rejects invalid packet type]==] [==[UDP codec rejects truncated packet]==])
