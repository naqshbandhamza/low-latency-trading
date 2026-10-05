add_test( [==[SimpleStrategy creates buy intent for tight quote]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/SimpleStrategyTest [==[SimpleStrategy creates buy intent for tight quote]==]  )
set_tests_properties( [==[SimpleStrategy creates buy intent for tight quote]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[SimpleStrategy ignores trade event]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/SimpleStrategyTest [==[SimpleStrategy ignores trade event]==]  )
set_tests_properties( [==[SimpleStrategy ignores trade event]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[SimpleStrategy ignores wide quote]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/SimpleStrategyTest [==[SimpleStrategy ignores wide quote]==]  )
set_tests_properties( [==[SimpleStrategy ignores wide quote]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
set( SimpleStrategyTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "SimpleStrategy creates buy intent for tight quote",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/strategy/SimpleStrategyTest.cpp",
      "line" : 10
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "SimpleStrategy ignores trade event",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/strategy/SimpleStrategyTest.cpp",
      "line" : 99
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "SimpleStrategy ignores wide quote",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/strategy/SimpleStrategyTest.cpp",
      "line" : 65
    },
    "tags" : 
  }
]==] [==[SimpleStrategy creates buy intent for tight quote]==] [==[SimpleStrategy ignores trade event]==] [==[SimpleStrategy ignores wide quote]==])
