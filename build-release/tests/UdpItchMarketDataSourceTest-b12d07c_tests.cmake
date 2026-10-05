add_test( [==[UDP ITCH market data source receives packet]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/UdpItchMarketDataSourceTest [==[UDP ITCH market data source receives packet]==]  )
set_tests_properties( [==[UDP ITCH market data source receives packet]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[UDP ITCH market data source rejects malformed datagram]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/UdpItchMarketDataSourceTest [==[UDP ITCH market data source rejects malformed datagram]==]  )
set_tests_properties( [==[UDP ITCH market data source rejects malformed datagram]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[UDP ITCH market data source returns false on timeout]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/UdpItchMarketDataSourceTest [==[UDP ITCH market data source returns false on timeout]==]  )
set_tests_properties( [==[UDP ITCH market data source returns false on timeout]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( UdpItchMarketDataSourceTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "UDP ITCH market data source receives packet",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/UdpItchMarketDataSourceTest.cpp",
      "line" : 77
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "UDP ITCH market data source rejects malformed datagram",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/UdpItchMarketDataSourceTest.cpp",
      "line" : 167
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "UDP ITCH market data source returns false on timeout",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/UdpItchMarketDataSourceTest.cpp",
      "line" : 145
    },
    "tags" : 
  }
]==] [==[UDP ITCH market data source receives packet]==] [==[UDP ITCH market data source rejects malformed datagram]==] [==[UDP ITCH market data source returns false on timeout]==])
