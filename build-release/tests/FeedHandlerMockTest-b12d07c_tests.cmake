add_test( [==[FeedHandler converts Quote messages]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/FeedHandlerMockTest [==[FeedHandler converts Quote messages]==]  )
set_tests_properties( [==[FeedHandler converts Quote messages]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[FeedHandler converts Trade messages]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/FeedHandlerMockTest [==[FeedHandler converts Trade messages]==]  )
set_tests_properties( [==[FeedHandler converts Trade messages]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[FeedHandler publishes Quote and Trade market events]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/FeedHandlerMockTest [==[FeedHandler publishes Quote and Trade market events]==]  )
set_tests_properties( [==[FeedHandler publishes Quote and Trade market events]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[FeedHandler recovers missing sequence]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/FeedHandlerMockTest [==[FeedHandler recovers missing sequence]==]  )
set_tests_properties( [==[FeedHandler recovers missing sequence]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[FeedHandler stops cleanly while market event queue is full]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/FeedHandlerMockTest [==[FeedHandler stops cleanly while market event queue is full]==]  )
set_tests_properties( [==[FeedHandler stops cleanly while market event queue is full]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[FeedHandler stops when sequence recovery fails]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/FeedHandlerMockTest [==[FeedHandler stops when sequence recovery fails]==]  )
set_tests_properties( [==[FeedHandler stops when sequence recovery fails]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[SequenceRecovery handles maximum sequence]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/FeedHandlerMockTest [==[SequenceRecovery handles maximum sequence]==]  )
set_tests_properties( [==[SequenceRecovery handles maximum sequence]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[SequenceRecovery logs sequence gap]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/FeedHandlerMockTest [==[SequenceRecovery logs sequence gap]==]  )
set_tests_properties( [==[SequenceRecovery logs sequence gap]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[SequenceRecovery rejects backward sequence]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/FeedHandlerMockTest [==[SequenceRecovery rejects backward sequence]==]  )
set_tests_properties( [==[SequenceRecovery rejects backward sequence]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[SequenceRecovery rejects incomplete recovered sequence]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/FeedHandlerMockTest [==[SequenceRecovery rejects incomplete recovered sequence]==]  )
set_tests_properties( [==[SequenceRecovery rejects incomplete recovered sequence]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[SequenceRecovery rejects out-of-order recovered sequence]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/FeedHandlerMockTest [==[SequenceRecovery rejects out-of-order recovered sequence]==]  )
set_tests_properties( [==[SequenceRecovery rejects out-of-order recovered sequence]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[SequenceRecovery rejects zero received sequence]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/FeedHandlerMockTest [==[SequenceRecovery rejects zero received sequence]==]  )
set_tests_properties( [==[SequenceRecovery rejects zero received sequence]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
add_test( [==[SequenceRecovery validates recovered sequence range]==] /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests/FeedHandlerMockTest [==[SequenceRecovery validates recovered sequence range]==]  )
set_tests_properties( [==[SequenceRecovery validates recovered sequence range]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-release/tests SKIP_RETURN_CODE 4)
set( FeedHandlerMockTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "FeedHandler converts Quote messages",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/feed_handler/FeedHandlerTest.cpp",
      "line" : 142
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "FeedHandler converts Trade messages",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/feed_handler/FeedHandlerTest.cpp",
      "line" : 211
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "FeedHandler publishes Quote and Trade market events",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/feed_handler/FeedHandlerTest.cpp",
      "line" : 84
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "FeedHandler recovers missing sequence",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/feed_handler/FeedHandlerTest.cpp",
      "line" : 281
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "FeedHandler stops cleanly while market event queue is full",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/feed_handler/FeedHandlerTest.cpp",
      "line" : 733
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "FeedHandler stops when sequence recovery fails",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/feed_handler/FeedHandlerTest.cpp",
      "line" : 433
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "SequenceRecovery handles maximum sequence",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/feed_handler/FeedHandlerTest.cpp",
      "line" : 676
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "SequenceRecovery logs sequence gap",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/feed_handler/FeedHandlerTest.cpp",
      "line" : 377
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "SequenceRecovery rejects backward sequence",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/feed_handler/FeedHandlerTest.cpp",
      "line" : 614
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "SequenceRecovery rejects incomplete recovered sequence",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/feed_handler/FeedHandlerTest.cpp",
      "line" : 556
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "SequenceRecovery rejects out-of-order recovered sequence",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/feed_handler/FeedHandlerTest.cpp",
      "line" : 585
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "SequenceRecovery rejects zero received sequence",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/feed_handler/FeedHandlerTest.cpp",
      "line" : 641
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "SequenceRecovery validates recovered sequence range",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/feed_handler/FeedHandlerTest.cpp",
      "line" : 523
    },
    "tags" : 
  }
]==] [==[FeedHandler converts Quote messages]==] [==[FeedHandler converts Trade messages]==] [==[FeedHandler publishes Quote and Trade market events]==] [==[FeedHandler recovers missing sequence]==] [==[FeedHandler stops cleanly while market event queue is full]==] [==[FeedHandler stops when sequence recovery fails]==] [==[SequenceRecovery handles maximum sequence]==] [==[SequenceRecovery logs sequence gap]==] [==[SequenceRecovery rejects backward sequence]==] [==[SequenceRecovery rejects incomplete recovered sequence]==] [==[SequenceRecovery rejects out-of-order recovered sequence]==] [==[SequenceRecovery rejects zero received sequence]==] [==[SequenceRecovery validates recovered sequence range]==])
