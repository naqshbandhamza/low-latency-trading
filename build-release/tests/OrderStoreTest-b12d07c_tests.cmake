add_test( [==[OrderStore adds order]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderStoreTest [==[OrderStore adds order]==]  )
set_tests_properties( [==[OrderStore adds order]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[OrderStore mutable lookup can update quantity]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderStoreTest [==[OrderStore mutable lookup can update quantity]==]  )
set_tests_properties( [==[OrderStore mutable lookup can update quantity]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[OrderStore rejects duplicate order id]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderStoreTest [==[OrderStore rejects duplicate order id]==]  )
set_tests_properties( [==[OrderStore rejects duplicate order id]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[OrderStore remains correct across repeated population churn]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderStoreTest [==[OrderStore remains correct across repeated population churn]==]  )
set_tests_properties( [==[OrderStore remains correct across repeated population churn]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[OrderStore removes order]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderStoreTest [==[OrderStore removes order]==]  )
set_tests_properties( [==[OrderStore removes order]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[OrderStore removing unknown order returns false]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderStoreTest [==[OrderStore removing unknown order returns false]==]  )
set_tests_properties( [==[OrderStore removing unknown order returns false]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[OrderStore starts empty]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderStoreTest [==[OrderStore starts empty]==]  )
set_tests_properties( [==[OrderStore starts empty]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[OrderStore stores independent orders]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderStoreTest [==[OrderStore stores independent orders]==]  )
set_tests_properties( [==[OrderStore stores independent orders]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[OrderStore supports repeated insert remove churn]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/OrderStoreTest [==[OrderStore supports repeated insert remove churn]==]  )
set_tests_properties( [==[OrderStore supports repeated insert remove churn]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( OrderStoreTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "OrderStore adds order",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/normalized/OrderStoreTest.cpp",
      "line" : 23
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "OrderStore mutable lookup can update quantity",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/normalized/OrderStoreTest.cpp",
      "line" : 193
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "OrderStore rejects duplicate order id",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/normalized/OrderStoreTest.cpp",
      "line" : 68
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "OrderStore remains correct across repeated population churn",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/normalized/OrderStoreTest.cpp",
      "line" : 344
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "OrderStore removes order",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/normalized/OrderStoreTest.cpp",
      "line" : 235
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "OrderStore removing unknown order returns false",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/normalized/OrderStoreTest.cpp",
      "line" : 272
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "OrderStore starts empty",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/normalized/OrderStoreTest.cpp",
      "line" : 11
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "OrderStore stores independent orders",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/normalized/OrderStoreTest.cpp",
      "line" : 129
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "OrderStore supports repeated insert remove churn",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/normalized/OrderStoreTest.cpp",
      "line" : 285
    },
    "tags" : 
  }
]==] [==[OrderStore adds order]==] [==[OrderStore mutable lookup can update quantity]==] [==[OrderStore rejects duplicate order id]==] [==[OrderStore remains correct across repeated population churn]==] [==[OrderStore removes order]==] [==[OrderStore removing unknown order returns false]==] [==[OrderStore starts empty]==] [==[OrderStore stores independent orders]==] [==[OrderStore supports repeated insert remove churn]==])
