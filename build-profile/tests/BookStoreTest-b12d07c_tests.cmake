add_test( [==[BookStore aggregates orders independently per instrument]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/BookStoreTest [==[BookStore aggregates orders independently per instrument]==]  )
set_tests_properties( [==[BookStore aggregates orders independently per instrument]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[BookStore creates order book]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/BookStoreTest [==[BookStore creates order book]==]  )
set_tests_properties( [==[BookStore creates order book]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[BookStore find does not create missing book]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/BookStoreTest [==[BookStore find does not create missing book]==]  )
set_tests_properties( [==[BookStore find does not create missing book]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[BookStore keeps instruments independent]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/BookStoreTest [==[BookStore keeps instruments independent]==]  )
set_tests_properties( [==[BookStore keeps instruments independent]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[BookStore maintains independent bid and ask BBO]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/BookStoreTest [==[BookStore maintains independent bid and ask BBO]==]  )
set_tests_properties( [==[BookStore maintains independent bid and ask BBO]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[BookStore returns existing order book]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/BookStoreTest [==[BookStore returns existing order book]==]  )
set_tests_properties( [==[BookStore returns existing order book]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[BookStore starts empty]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/BookStoreTest [==[BookStore starts empty]==]  )
set_tests_properties( [==[BookStore starts empty]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
set( BookStoreTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "BookStore aggregates orders independently per instrument",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BookStoreTest.cpp",
      "line" : 133
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BookStore creates order book",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BookStoreTest.cpp",
      "line" : 22
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BookStore find does not create missing book",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BookStoreTest.cpp",
      "line" : 118
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BookStore keeps instruments independent",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BookStoreTest.cpp",
      "line" : 76
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BookStore maintains independent bid and ask BBO",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BookStoreTest.cpp",
      "line" : 171
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BookStore returns existing order book",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BookStoreTest.cpp",
      "line" : 43
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "BookStore starts empty",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/book/BookStoreTest.cpp",
      "line" : 10
    },
    "tags" : 
  }
]==] [==[BookStore aggregates orders independently per instrument]==] [==[BookStore creates order book]==] [==[BookStore find does not create missing book]==] [==[BookStore keeps instruments independent]==] [==[BookStore maintains independent bid and ask BBO]==] [==[BookStore returns existing order book]==] [==[BookStore starts empty]==])
