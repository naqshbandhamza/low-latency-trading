add_test( [==[ITCH replay can populate market state]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchReplayTest [==[ITCH replay can populate market state]==]  )
set_tests_properties( [==[ITCH replay can populate market state]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH replay counts every supported message type]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchReplayTest [==[ITCH replay counts every supported message type]==]  )
set_tests_properties( [==[ITCH replay counts every supported message type]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH replay counts malformed supported message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchReplayTest [==[ITCH replay counts malformed supported message]==]  )
set_tests_properties( [==[ITCH replay counts malformed supported message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH replay delivers decoded messages to handler]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchReplayTest [==[ITCH replay delivers decoded messages to handler]==]  )
set_tests_properties( [==[ITCH replay delivers decoded messages to handler]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH replay does not deliver unsupported messages to handler]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchReplayTest [==[ITCH replay does not deliver unsupported messages to handler]==]  )
set_tests_properties( [==[ITCH replay does not deliver unsupported messages to handler]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH replay processes complete BinaryFILE session]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchReplayTest [==[ITCH replay processes complete BinaryFILE session]==]  )
set_tests_properties( [==[ITCH replay processes complete BinaryFILE session]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH replay reports incomplete BinaryFILE]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchReplayTest [==[ITCH replay reports incomplete BinaryFILE]==]  )
set_tests_properties( [==[ITCH replay reports incomplete BinaryFILE]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH replay skips unsupported message types]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchReplayTest [==[ITCH replay skips unsupported message types]==]  )
set_tests_properties( [==[ITCH replay skips unsupported message types]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchReplay counts unsupported messages by type]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/ItchReplayTest [==[ItchReplay counts unsupported messages by type]==]  )
set_tests_properties( [==[ItchReplay counts unsupported messages by type]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
set( ItchReplayTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "ITCH replay can populate market state",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchReplayTest.cpp",
      "line" : 429
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH replay counts every supported message type",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchReplayTest.cpp",
      "line" : 255
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH replay counts malformed supported message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchReplayTest.cpp",
      "line" : 174
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH replay delivers decoded messages to handler",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchReplayTest.cpp",
      "line" : 390
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH replay does not deliver unsupported messages to handler",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchReplayTest.cpp",
      "line" : 478
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH replay processes complete BinaryFILE session",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchReplayTest.cpp",
      "line" : 82
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH replay reports incomplete BinaryFILE",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchReplayTest.cpp",
      "line" : 216
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH replay skips unsupported message types",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchReplayTest.cpp",
      "line" : 141
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchReplay counts unsupported messages by type",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchReplayTest.cpp",
      "line" : 337
    },
    "tags" : 
  }
]==] [==[ITCH replay can populate market state]==] [==[ITCH replay counts every supported message type]==] [==[ITCH replay counts malformed supported message]==] [==[ITCH replay delivers decoded messages to handler]==] [==[ITCH replay does not deliver unsupported messages to handler]==] [==[ITCH replay processes complete BinaryFILE session]==] [==[ITCH replay reports incomplete BinaryFILE]==] [==[ITCH replay skips unsupported message types]==] [==[ItchReplay counts unsupported messages by type]==])
