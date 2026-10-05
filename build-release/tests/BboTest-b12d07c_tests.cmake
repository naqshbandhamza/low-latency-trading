add_test( [==[BBO detects ask appearance]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BboTest [==[BBO detects ask appearance]==]  )
set_tests_properties( [==[BBO detects ask appearance]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[BBO detects ask disappearance]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BboTest [==[BBO detects ask disappearance]==]  )
set_tests_properties( [==[BBO detects ask disappearance]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[BBO detects best ask price change]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BboTest [==[BBO detects best ask price change]==]  )
set_tests_properties( [==[BBO detects best ask price change]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[BBO detects best ask quantity change]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BboTest [==[BBO detects best ask quantity change]==]  )
set_tests_properties( [==[BBO detects best ask quantity change]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[BBO detects best bid price change]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BboTest [==[BBO detects best bid price change]==]  )
set_tests_properties( [==[BBO detects best bid price change]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[BBO detects best bid quantity change]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BboTest [==[BBO detects best bid quantity change]==]  )
set_tests_properties( [==[BBO detects best bid quantity change]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[BBO detects bid appearance]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BboTest [==[BBO detects bid appearance]==]  )
set_tests_properties( [==[BBO detects bid appearance]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[BBO detects bid disappearance]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BboTest [==[BBO detects bid disappearance]==]  )
set_tests_properties( [==[BBO detects bid disappearance]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[BBO ignores values for absent ask]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BboTest [==[BBO ignores values for absent ask]==]  )
set_tests_properties( [==[BBO ignores values for absent ask]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[BBO ignores values for absent bid]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BboTest [==[BBO ignores values for absent bid]==]  )
set_tests_properties( [==[BBO ignores values for absent bid]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Empty BBO values are equal]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BboTest [==[Empty BBO values are equal]==]  )
set_tests_properties( [==[Empty BBO values are equal]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Identical two sided BBO values are equal]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/BboTest [==[Identical two sided BBO values are equal]==]  )
set_tests_properties( [==[Identical two sided BBO values are equal]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( BboTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "BBO detects ask appearance",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BboTest.cpp",
      "line" : 84
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BBO detects ask disappearance",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BboTest.cpp",
      "line" : 99
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BBO detects best ask price change",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BboTest.cpp",
      "line" : 114
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BBO detects best ask quantity change",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BboTest.cpp",
      "line" : 130
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BBO detects best bid price change",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BboTest.cpp",
      "line" : 52
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BBO detects best bid quantity change",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BboTest.cpp",
      "line" : 68
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BBO detects bid appearance",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BboTest.cpp",
      "line" : 22
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BBO detects bid disappearance",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BboTest.cpp",
      "line" : 37
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BBO ignores values for absent ask",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BboTest.cpp",
      "line" : 163
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BBO ignores values for absent bid",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BboTest.cpp",
      "line" : 146
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Empty BBO values are equal",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BboTest.cpp",
      "line" : 10
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Identical two sided BBO values are equal",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BboTest.cpp",
      "line" : 180
    },
    "tags" : 
  }
]==] [==[BBO detects ask appearance]==] [==[BBO detects ask disappearance]==] [==[BBO detects best ask price change]==] [==[BBO detects best ask quantity change]==] [==[BBO detects best bid price change]==] [==[BBO detects best bid quantity change]==] [==[BBO detects bid appearance]==] [==[BBO detects bid disappearance]==] [==[BBO ignores values for absent ask]==] [==[BBO ignores values for absent bid]==] [==[Empty BBO values are equal]==] [==[Identical two sided BBO values are equal]==])
