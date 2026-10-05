add_test( [==[UdpMarketDataSource handles bind failure]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/UdpMarketDataSourceTest [==[UdpMarketDataSource handles bind failure]==]  )
set_tests_properties( [==[UdpMarketDataSource handles bind failure]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[UdpMarketDataSource receive times out when no packet arrives]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/UdpMarketDataSourceTest [==[UdpMarketDataSource receive times out when no packet arrives]==]  )
set_tests_properties( [==[UdpMarketDataSource receive times out when no packet arrives]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[UdpMarketDataSource receives Quote packet]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/UdpMarketDataSourceTest [==[UdpMarketDataSource receives Quote packet]==]  )
set_tests_properties( [==[UdpMarketDataSource receives Quote packet]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[UdpMarketDataSource receives Trade packet]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/UdpMarketDataSourceTest [==[UdpMarketDataSource receives Trade packet]==]  )
set_tests_properties( [==[UdpMarketDataSource receives Trade packet]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[UdpMarketDataSource rejects oversized datagram]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/UdpMarketDataSourceTest [==[UdpMarketDataSource rejects oversized datagram]==]  )
set_tests_properties( [==[UdpMarketDataSource rejects oversized datagram]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[UdpMarketDataSource rejects short datagram]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/UdpMarketDataSourceTest [==[UdpMarketDataSource rejects short datagram]==]  )
set_tests_properties( [==[UdpMarketDataSource rejects short datagram]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[UdpMarketDataSource remains safe after construction failure]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/UdpMarketDataSourceTest [==[UdpMarketDataSource remains safe after construction failure]==]  )
set_tests_properties( [==[UdpMarketDataSource remains safe after construction failure]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( UdpMarketDataSourceTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "UdpMarketDataSource handles bind failure",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/UdpMarketDataSourceTest.cpp",
      "line" : 458
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "UdpMarketDataSource receive times out when no packet arrives",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/UdpMarketDataSourceTest.cpp",
      "line" : 297
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "UdpMarketDataSource receives Quote packet",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/UdpMarketDataSourceTest.cpp",
      "line" : 117
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "UdpMarketDataSource receives Trade packet",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/UdpMarketDataSourceTest.cpp",
      "line" : 212
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "UdpMarketDataSource rejects oversized datagram",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/UdpMarketDataSourceTest.cpp",
      "line" : 396
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "UdpMarketDataSource rejects short datagram",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/UdpMarketDataSourceTest.cpp",
      "line" : 337
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "UdpMarketDataSource remains safe after construction failure",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/UdpMarketDataSourceTest.cpp",
      "line" : 486
    },
    "tags" : 
  }
]==] [==[UdpMarketDataSource handles bind failure]==] [==[UdpMarketDataSource receive times out when no packet arrives]==] [==[UdpMarketDataSource receives Quote packet]==] [==[UdpMarketDataSource receives Trade packet]==] [==[UdpMarketDataSource rejects oversized datagram]==] [==[UdpMarketDataSource rejects short datagram]==] [==[UdpMarketDataSource remains safe after construction failure]==])
