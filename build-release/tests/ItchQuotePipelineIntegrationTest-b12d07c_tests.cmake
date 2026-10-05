add_test( [==[Full C execution publishes next BBO with correct instrument]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchQuotePipelineIntegrationTest [==[Full C execution publishes next BBO with correct instrument]==]  )
set_tests_properties( [==[Full C execution publishes next BBO with correct instrument]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Full E execution publishes next BBO with correct instrument]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchQuotePipelineIntegrationTest [==[Full E execution publishes next BBO with correct instrument]==]  )
set_tests_properties( [==[Full E execution publishes next BBO with correct instrument]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[Full X cancellation publishes next BBO with correct instrument]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchQuotePipelineIntegrationTest [==[Full X cancellation publishes next BBO with correct instrument]==]  )
set_tests_properties( [==[Full X cancellation publishes next BBO with correct instrument]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH pipeline assigns increasing normalized quote sequences]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchQuotePipelineIntegrationTest [==[ITCH pipeline assigns increasing normalized quote sequences]==]  )
set_tests_properties( [==[ITCH pipeline assigns increasing normalized quote sequences]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH pipeline ignores order behind current BBO]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchQuotePipelineIntegrationTest [==[ITCH pipeline ignores order behind current BBO]==]  )
set_tests_properties( [==[ITCH pipeline ignores order behind current BBO]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH pipeline publishes another quote when BBO changes]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchQuotePipelineIntegrationTest [==[ITCH pipeline publishes another quote when BBO changes]==]  )
set_tests_properties( [==[ITCH pipeline publishes another quote when BBO changes]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH pipeline publishes correct normalized quote data]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchQuotePipelineIntegrationTest [==[ITCH pipeline publishes correct normalized quote data]==]  )
set_tests_properties( [==[ITCH pipeline publishes correct normalized quote data]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH pipeline publishes quote when book becomes two sided]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchQuotePipelineIntegrationTest [==[ITCH pipeline publishes quote when book becomes two sided]==]  )
set_tests_properties( [==[ITCH pipeline publishes quote when book becomes two sided]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( ItchQuotePipelineIntegrationTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "Full C execution publishes next BBO with correct instrument",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchQuotePipelineIntegrationTest.cpp",
      "line" : 873
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Full E execution publishes next BBO with correct instrument",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchQuotePipelineIntegrationTest.cpp",
      "line" : 750
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Full X cancellation publishes next BBO with correct instrument",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchQuotePipelineIntegrationTest.cpp",
      "line" : 984
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH pipeline assigns increasing normalized quote sequences",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchQuotePipelineIntegrationTest.cpp",
      "line" : 610
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH pipeline ignores order behind current BBO",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchQuotePipelineIntegrationTest.cpp",
      "line" : 338
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH pipeline publishes another quote when BBO changes",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchQuotePipelineIntegrationTest.cpp",
      "line" : 435
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH pipeline publishes correct normalized quote data",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchQuotePipelineIntegrationTest.cpp",
      "line" : 512
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH pipeline publishes quote when book becomes two sided",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchQuotePipelineIntegrationTest.cpp",
      "line" : 242
    },
    "tags" : 
  }
]==] [==[Full C execution publishes next BBO with correct instrument]==] [==[Full E execution publishes next BBO with correct instrument]==] [==[Full X cancellation publishes next BBO with correct instrument]==] [==[ITCH pipeline assigns increasing normalized quote sequences]==] [==[ITCH pipeline ignores order behind current BBO]==] [==[ITCH pipeline publishes another quote when BBO changes]==] [==[ITCH pipeline publishes correct normalized quote data]==] [==[ITCH pipeline publishes quote when book becomes two sided]==])
