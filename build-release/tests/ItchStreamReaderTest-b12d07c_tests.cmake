add_test( [==[ITCH stream reader reads BinaryFILE message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchStreamReaderTest [==[ITCH stream reader reads BinaryFILE message]==]  )
set_tests_properties( [==[ITCH stream reader reads BinaryFILE message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH stream reader reads message then end of session]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchStreamReaderTest [==[ITCH stream reader reads message then end of session]==]  )
set_tests_properties( [==[ITCH stream reader reads message then end of session]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH stream reader reads multiple BinaryFILE messages]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchStreamReaderTest [==[ITCH stream reader reads multiple BinaryFILE messages]==]  )
set_tests_properties( [==[ITCH stream reader reads multiple BinaryFILE messages]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH stream reader recognizes BinaryFILE end of session]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchStreamReaderTest [==[ITCH stream reader recognizes BinaryFILE end of session]==]  )
set_tests_properties( [==[ITCH stream reader recognizes BinaryFILE end of session]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH stream reader rejects truncated BinaryFILE length prefix]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchStreamReaderTest [==[ITCH stream reader rejects truncated BinaryFILE length prefix]==]  )
set_tests_properties( [==[ITCH stream reader rejects truncated BinaryFILE length prefix]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH stream reader rejects truncated BinaryFILE payload]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchStreamReaderTest [==[ITCH stream reader rejects truncated BinaryFILE payload]==]  )
set_tests_properties( [==[ITCH stream reader rejects truncated BinaryFILE payload]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH stream reader reports empty BinaryFILE as incomplete]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchStreamReaderTest [==[ITCH stream reader reports empty BinaryFILE as incomplete]==]  )
set_tests_properties( [==[ITCH stream reader reports empty BinaryFILE as incomplete]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( ItchStreamReaderTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "ITCH stream reader reads BinaryFILE message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchStreamReaderTest.cpp",
      "line" : 11
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH stream reader reads message then end of session",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchStreamReaderTest.cpp",
      "line" : 263
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH stream reader reads multiple BinaryFILE messages",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchStreamReaderTest.cpp",
      "line" : 62
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH stream reader recognizes BinaryFILE end of session",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchStreamReaderTest.cpp",
      "line" : 135
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH stream reader rejects truncated BinaryFILE length prefix",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchStreamReaderTest.cpp",
      "line" : 194
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH stream reader rejects truncated BinaryFILE payload",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchStreamReaderTest.cpp",
      "line" : 224
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH stream reader reports empty BinaryFILE as incomplete",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchStreamReaderTest.cpp",
      "line" : 166
    },
    "tags" : 
  }
]==] [==[ITCH stream reader reads BinaryFILE message]==] [==[ITCH stream reader reads message then end of session]==] [==[ITCH stream reader reads multiple BinaryFILE messages]==] [==[ITCH stream reader recognizes BinaryFILE end of session]==] [==[ITCH stream reader rejects truncated BinaryFILE length prefix]==] [==[ITCH stream reader rejects truncated BinaryFILE payload]==] [==[ITCH stream reader reports empty BinaryFILE as incomplete]==])
