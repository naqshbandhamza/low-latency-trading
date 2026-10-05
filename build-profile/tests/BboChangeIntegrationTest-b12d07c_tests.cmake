add_test( [==[Adding better bid emits BBO change]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/BboChangeIntegrationTest [==[Adding better bid emits BBO change]==]  )
set_tests_properties( [==[Adding better bid emits BBO change]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[Adding first bid emits BBO change]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/BboChangeIntegrationTest [==[Adding first bid emits BBO change]==]  )
set_tests_properties( [==[Adding first bid emits BBO change]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[Adding order behind best bid does not emit BBO change]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/BboChangeIntegrationTest [==[Adding order behind best bid does not emit BBO change]==]  )
set_tests_properties( [==[Adding order behind best bid does not emit BBO change]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[Adding quantity at current best bid emits BBO change]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/BboChangeIntegrationTest [==[Adding quantity at current best bid emits BBO change]==]  )
set_tests_properties( [==[Adding quantity at current best bid emits BBO change]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[BBO notifications remain independent between instruments]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/BboChangeIntegrationTest [==[BBO notifications remain independent between instruments]==]  )
set_tests_properties( [==[BBO notifications remain independent between instruments]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[Deleting best bid emits next best BBO]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/BboChangeIntegrationTest [==[Deleting best bid emits next best BBO]==]  )
set_tests_properties( [==[Deleting best bid emits next best BBO]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
set( BboChangeIntegrationTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "Adding better bid emits BBO change",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/BboChangeIntegrationTest.cpp",
      "line" : 240
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Adding first bid emits BBO change",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/BboChangeIntegrationTest.cpp",
      "line" : 69
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Adding order behind best bid does not emit BBO change",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/BboChangeIntegrationTest.cpp",
      "line" : 121
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Adding quantity at current best bid emits BBO change",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/BboChangeIntegrationTest.cpp",
      "line" : 177
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BBO notifications remain independent between instruments",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/BboChangeIntegrationTest.cpp",
      "line" : 367
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Deleting best bid emits next best BBO",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/BboChangeIntegrationTest.cpp",
      "line" : 298
    },
    "tags" : 
  }
]==] [==[Adding better bid emits BBO change]==] [==[Adding first bid emits BBO change]==] [==[Adding order behind best bid does not emit BBO change]==] [==[Adding quantity at current best bid emits BBO change]==] [==[BBO notifications remain independent between instruments]==] [==[Deleting best bid emits next best BBO]==])
