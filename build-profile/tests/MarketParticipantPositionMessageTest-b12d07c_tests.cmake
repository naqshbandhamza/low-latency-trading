add_test( [==[ItchDecoder decodes Market Participant Position message]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/MarketParticipantPositionMessageTest [==[ItchDecoder decodes Market Participant Position message]==]  )
set_tests_properties( [==[ItchDecoder decodes Market Participant Position message]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder decodes maximum Market Participant Position header fields]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/MarketParticipantPositionMessageTest [==[ItchDecoder decodes maximum Market Participant Position header fields]==]  )
set_tests_properties( [==[ItchDecoder decodes maximum Market Participant Position header fields]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects null Market Participant Position data]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/MarketParticipantPositionMessageTest [==[ItchDecoder rejects null Market Participant Position data]==]  )
set_tests_properties( [==[ItchDecoder rejects null Market Participant Position data]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects oversized Market Participant Position]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/MarketParticipantPositionMessageTest [==[ItchDecoder rejects oversized Market Participant Position]==]  )
set_tests_properties( [==[ItchDecoder rejects oversized Market Participant Position]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects truncated Market Participant Position]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/MarketParticipantPositionMessageTest [==[ItchDecoder rejects truncated Market Participant Position]==]  )
set_tests_properties( [==[ItchDecoder rejects truncated Market Participant Position]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDecoder rejects wrong Market Participant Position type]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/MarketParticipantPositionMessageTest [==[ItchDecoder rejects wrong Market Participant Position type]==]  )
set_tests_properties( [==[ItchDecoder rejects wrong Market Participant Position type]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDispatcher rejects malformed Market Participant Position]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/MarketParticipantPositionMessageTest [==[ItchDispatcher rejects malformed Market Participant Position]==]  )
set_tests_properties( [==[ItchDispatcher rejects malformed Market Participant Position]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchDispatcher routes Market Participant Position]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/MarketParticipantPositionMessageTest [==[ItchDispatcher routes Market Participant Position]==]  )
set_tests_properties( [==[ItchDispatcher routes Market Participant Position]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[ItchReplay counts Market Participant Position as supported]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/MarketParticipantPositionMessageTest [==[ItchReplay counts Market Participant Position as supported]==]  )
set_tests_properties( [==[ItchReplay counts Market Participant Position as supported]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[Market Participant Position preserves eight character stock]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/MarketParticipantPositionMessageTest [==[Market Participant Position preserves eight character stock]==]  )
set_tests_properties( [==[Market Participant Position preserves eight character stock]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[Market Participant Position preserves four character MPID]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/MarketParticipantPositionMessageTest [==[Market Participant Position preserves four character MPID]==]  )
set_tests_properties( [==[Market Participant Position preserves four character MPID]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[Market Participant Position preserves status fields]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/MarketParticipantPositionMessageTest [==[Market Participant Position preserves status fields]==]  )
set_tests_properties( [==[Market Participant Position preserves status fields]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
add_test( [==[Market Participant Position trims padded MPID]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/MarketParticipantPositionMessageTest [==[Market Participant Position trims padded MPID]==]  )
set_tests_properties( [==[Market Participant Position trims padded MPID]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests SKIP_RETURN_CODE 4)
set( MarketParticipantPositionMessageTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder decodes Market Participant Position message",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/MarketParticipantPositionMessageTest.cpp",
      "line" : 58
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder decodes maximum Market Participant Position header fields",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/MarketParticipantPositionMessageTest.cpp",
      "line" : 306
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects null Market Participant Position data",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/MarketParticipantPositionMessageTest.cpp",
      "line" : 290
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects oversized Market Participant Position",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/MarketParticipantPositionMessageTest.cpp",
      "line" : 249
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects truncated Market Participant Position",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/MarketParticipantPositionMessageTest.cpp",
      "line" : 230
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDecoder rejects wrong Market Participant Position type",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/MarketParticipantPositionMessageTest.cpp",
      "line" : 269
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDispatcher rejects malformed Market Participant Position",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/MarketParticipantPositionMessageTest.cpp",
      "line" : 384
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchDispatcher routes Market Participant Position",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/MarketParticipantPositionMessageTest.cpp",
      "line" : 353
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "ItchReplay counts Market Participant Position as supported",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/MarketParticipantPositionMessageTest.cpp",
      "line" : 403
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Market Participant Position preserves eight character stock",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/MarketParticipantPositionMessageTest.cpp",
      "line" : 156
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Market Participant Position preserves four character MPID",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/MarketParticipantPositionMessageTest.cpp",
      "line" : 100
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Market Participant Position preserves status fields",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/MarketParticipantPositionMessageTest.cpp",
      "line" : 195
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "Market Participant Position trims padded MPID",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/MarketParticipantPositionMessageTest.cpp",
      "line" : 128
    },
    "tags" : 
  }
]==] [==[ItchDecoder decodes Market Participant Position message]==] [==[ItchDecoder decodes maximum Market Participant Position header fields]==] [==[ItchDecoder rejects null Market Participant Position data]==] [==[ItchDecoder rejects oversized Market Participant Position]==] [==[ItchDecoder rejects truncated Market Participant Position]==] [==[ItchDecoder rejects wrong Market Participant Position type]==] [==[ItchDispatcher rejects malformed Market Participant Position]==] [==[ItchDispatcher routes Market Participant Position]==] [==[ItchReplay counts Market Participant Position as supported]==] [==[Market Participant Position preserves eight character stock]==] [==[Market Participant Position preserves four character MPID]==] [==[Market Participant Position preserves status fields]==] [==[Market Participant Position trims padded MPID]==])
