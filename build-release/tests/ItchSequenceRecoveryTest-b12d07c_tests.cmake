add_test( [==[ITCH sequence recovery ignores non forward recovery request]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchSequenceRecoveryTest [==[ITCH sequence recovery ignores non forward recovery request]==]  )
set_tests_properties( [==[ITCH sequence recovery ignores non forward recovery request]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH sequence recovery recovers multiple missing packets]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchSequenceRecoveryTest [==[ITCH sequence recovery recovers multiple missing packets]==]  )
set_tests_properties( [==[ITCH sequence recovery recovers multiple missing packets]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH sequence recovery recovers single missing packet]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchSequenceRecoveryTest [==[ITCH sequence recovery recovers single missing packet]==]  )
set_tests_properties( [==[ITCH sequence recovery recovers single missing packet]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH sequence recovery rejects incomplete recovery]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchSequenceRecoveryTest [==[ITCH sequence recovery rejects incomplete recovery]==]  )
set_tests_properties( [==[ITCH sequence recovery rejects incomplete recovery]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH sequence recovery rejects out of order packets]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchSequenceRecoveryTest [==[ITCH sequence recovery rejects out of order packets]==]  )
set_tests_properties( [==[ITCH sequence recovery rejects out of order packets]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH sequence recovery rejects wrong recovered sequence]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchSequenceRecoveryTest [==[ITCH sequence recovery rejects wrong recovered sequence]==]  )
set_tests_properties( [==[ITCH sequence recovery rejects wrong recovered sequence]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[ITCH sequence recovery stops when source fails]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/ItchSequenceRecoveryTest [==[ITCH sequence recovery stops when source fails]==]  )
set_tests_properties( [==[ITCH sequence recovery stops when source fails]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( ItchSequenceRecoveryTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "ITCH sequence recovery ignores non forward recovery request",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchSequenceRecoveryTest.cpp",
      "line" : 270
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH sequence recovery recovers multiple missing packets",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchSequenceRecoveryTest.cpp",
      "line" : 82
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH sequence recovery recovers single missing packet",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchSequenceRecoveryTest.cpp",
      "line" : 36
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH sequence recovery rejects incomplete recovery",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchSequenceRecoveryTest.cpp",
      "line" : 161
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH sequence recovery rejects out of order packets",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchSequenceRecoveryTest.cpp",
      "line" : 199
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH sequence recovery rejects wrong recovered sequence",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchSequenceRecoveryTest.cpp",
      "line" : 233
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ITCH sequence recovery stops when source fails",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/ItchSequenceRecoveryTest.cpp",
      "line" : 128
    },
    "tags" : 
  }
]==] [==[ITCH sequence recovery ignores non forward recovery request]==] [==[ITCH sequence recovery recovers multiple missing packets]==] [==[ITCH sequence recovery recovers single missing packet]==] [==[ITCH sequence recovery rejects incomplete recovery]==] [==[ITCH sequence recovery rejects out of order packets]==] [==[ITCH sequence recovery rejects wrong recovered sequence]==] [==[ITCH sequence recovery stops when source fails]==])
