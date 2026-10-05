add_test( [==[UDP FeedHandler establishes sequence from non-zero first packet]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/FeedHandlerUdpIntegrationTest [==[UDP FeedHandler establishes sequence from non-zero first packet]==]  )
set_tests_properties( [==[UDP FeedHandler establishes sequence from non-zero first packet]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[UDP FeedHandler handles normal contiguous sequence]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/FeedHandlerUdpIntegrationTest [==[UDP FeedHandler handles normal contiguous sequence]==]  )
set_tests_properties( [==[UDP FeedHandler handles normal contiguous sequence]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[UDP FeedHandler ignores duplicate old packet]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/FeedHandlerUdpIntegrationTest [==[UDP FeedHandler ignores duplicate old packet]==]  )
set_tests_properties( [==[UDP FeedHandler ignores duplicate old packet]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[UDP FeedHandler recovers missing packet sequence]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/FeedHandlerUdpIntegrationTest [==[UDP FeedHandler recovers missing packet sequence]==]  )
set_tests_properties( [==[UDP FeedHandler recovers missing packet sequence]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[UDP FeedHandler recovers multiple missing packets]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/FeedHandlerUdpIntegrationTest [==[UDP FeedHandler recovers multiple missing packets]==]  )
set_tests_properties( [==[UDP FeedHandler recovers multiple missing packets]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[UDP FeedHandler stops safely when recovery fails]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/FeedHandlerUdpIntegrationTest [==[UDP FeedHandler stops safely when recovery fails]==]  )
set_tests_properties( [==[UDP FeedHandler stops safely when recovery fails]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
set( FeedHandlerUdpIntegrationTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "UDP FeedHandler establishes sequence from non-zero first packet",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/feed_handler/FeedHandlerUdpIntegrationTest.cpp",
      "line" : 483
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "UDP FeedHandler handles normal contiguous sequence",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/feed_handler/FeedHandlerUdpIntegrationTest.cpp",
      "line" : 236
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "UDP FeedHandler ignores duplicate old packet",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/feed_handler/FeedHandlerUdpIntegrationTest.cpp",
      "line" : 356
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "UDP FeedHandler recovers missing packet sequence",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/feed_handler/FeedHandlerUdpIntegrationTest.cpp",
      "line" : 182
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "UDP FeedHandler recovers multiple missing packets",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/feed_handler/FeedHandlerUdpIntegrationTest.cpp",
      "line" : 291
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "UDP FeedHandler stops safely when recovery fails",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/feed_handler/FeedHandlerUdpIntegrationTest.cpp",
      "line" : 415
    },
    "tags" : 
  }
]==] [==[UDP FeedHandler establishes sequence from non-zero first packet]==] [==[UDP FeedHandler handles normal contiguous sequence]==] [==[UDP FeedHandler ignores duplicate old packet]==] [==[UDP FeedHandler recovers missing packet sequence]==] [==[UDP FeedHandler recovers multiple missing packets]==] [==[UDP FeedHandler stops safely when recovery fails]==])
