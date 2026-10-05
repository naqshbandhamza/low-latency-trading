add_test( [==[ITCH UDP codec handles maximum payload]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchUdpCodecTest [==[ITCH UDP codec handles maximum payload]==]  )
set_tests_properties( [==[ITCH UDP codec handles maximum payload]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH UDP codec preserves maximum sequence]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchUdpCodecTest [==[ITCH UDP codec preserves maximum sequence]==]  )
set_tests_properties( [==[ITCH UDP codec preserves maximum sequence]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH UDP codec rejects null input]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchUdpCodecTest [==[ITCH UDP codec rejects null input]==]  )
set_tests_properties( [==[ITCH UDP codec rejects null input]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH UDP codec rejects trailing bytes]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchUdpCodecTest [==[ITCH UDP codec rejects trailing bytes]==]  )
set_tests_properties( [==[ITCH UDP codec rejects trailing bytes]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH UDP codec rejects truncated header]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchUdpCodecTest [==[ITCH UDP codec rejects truncated header]==]  )
set_tests_properties( [==[ITCH UDP codec rejects truncated header]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH UDP codec rejects truncated payload]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchUdpCodecTest [==[ITCH UDP codec rejects truncated payload]==]  )
set_tests_properties( [==[ITCH UDP codec rejects truncated payload]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH UDP codec round trips packet]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchUdpCodecTest [==[ITCH UDP codec round trips packet]==]  )
set_tests_properties( [==[ITCH UDP codec round trips packet]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH UDP codec supports empty payload]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchUdpCodecTest [==[ITCH UDP codec supports empty payload]==]  )
set_tests_properties( [==[ITCH UDP codec supports empty payload]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
set( ItchUdpCodecTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "ITCH UDP codec handles maximum payload",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchUdpCodecTest.cpp",
      "line" : 275
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH UDP codec preserves maximum sequence",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchUdpCodecTest.cpp",
      "line" : 81
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH UDP codec rejects null input",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchUdpCodecTest.cpp",
      "line" : 262
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH UDP codec rejects trailing bytes",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchUdpCodecTest.cpp",
      "line" : 221
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH UDP codec rejects truncated header",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchUdpCodecTest.cpp",
      "line" : 167
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH UDP codec rejects truncated payload",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchUdpCodecTest.cpp",
      "line" : 183
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH UDP codec round trips packet",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchUdpCodecTest.cpp",
      "line" : 12
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH UDP codec supports empty payload",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchUdpCodecTest.cpp",
      "line" : 128
    },
    "tags" : 
  }
]==] [==[ITCH UDP codec handles maximum payload]==] [==[ITCH UDP codec preserves maximum sequence]==] [==[ITCH UDP codec rejects null input]==] [==[ITCH UDP codec rejects trailing bytes]==] [==[ITCH UDP codec rejects truncated header]==] [==[ITCH UDP codec rejects truncated payload]==] [==[ITCH UDP codec round trips packet]==] [==[ITCH UDP codec supports empty payload]==])
