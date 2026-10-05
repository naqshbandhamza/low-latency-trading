add_test( [==[File ITCH recovery source builds record index]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/FileItchRecoverySourceTest [==[File ITCH recovery source builds record index]==]  )
set_tests_properties( [==[File ITCH recovery source builds record index]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading SKIP_RETURN_CODE 4)
add_test( [==[File ITCH recovery source fails for missing file]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/FileItchRecoverySourceTest [==[File ITCH recovery source fails for missing file]==]  )
set_tests_properties( [==[File ITCH recovery source fails for missing file]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading SKIP_RETURN_CODE 4)
add_test( [==[File ITCH recovery source invalidates index when source file size changes]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/FileItchRecoverySourceTest [==[File ITCH recovery source invalidates index when source file size changes]==]  )
set_tests_properties( [==[File ITCH recovery source invalidates index when source file size changes]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading SKIP_RETURN_CODE 4)
add_test( [==[File ITCH recovery source loads persisted sparse index]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/FileItchRecoverySourceTest [==[File ITCH recovery source loads persisted sparse index]==]  )
set_tests_properties( [==[File ITCH recovery source loads persisted sparse index]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading SKIP_RETURN_CODE 4)
add_test( [==[File ITCH recovery source persists sparse index]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/FileItchRecoverySourceTest [==[File ITCH recovery source persists sparse index]==]  )
set_tests_properties( [==[File ITCH recovery source persists sparse index]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading SKIP_RETURN_CODE 4)
add_test( [==[File ITCH recovery source rebuilds corrupted persisted index]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/FileItchRecoverySourceTest [==[File ITCH recovery source rebuilds corrupted persisted index]==]  )
set_tests_properties( [==[File ITCH recovery source rebuilds corrupted persisted index]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading SKIP_RETURN_CODE 4)
add_test( [==[File ITCH recovery source recovers exact packet range]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/FileItchRecoverySourceTest [==[File ITCH recovery source recovers exact packet range]==]  )
set_tests_properties( [==[File ITCH recovery source recovers exact packet range]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading SKIP_RETURN_CODE 4)
add_test( [==[File ITCH recovery source recovers single packet]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/FileItchRecoverySourceTest [==[File ITCH recovery source recovers single packet]==]  )
set_tests_properties( [==[File ITCH recovery source recovers single packet]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading SKIP_RETURN_CODE 4)
add_test( [==[File ITCH recovery source rejects invalid range]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/FileItchRecoverySourceTest [==[File ITCH recovery source rejects invalid range]==]  )
set_tests_properties( [==[File ITCH recovery source rejects invalid range]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading SKIP_RETURN_CODE 4)
add_test( [==[File ITCH recovery source rejects range beyond indexed file]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/FileItchRecoverySourceTest [==[File ITCH recovery source rejects range beyond indexed file]==]  )
set_tests_properties( [==[File ITCH recovery source rejects range beyond indexed file]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading SKIP_RETURN_CODE 4)
add_test( [==[File ITCH recovery source rejects sequence zero]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/FileItchRecoverySourceTest [==[File ITCH recovery source rejects sequence zero]==]  )
set_tests_properties( [==[File ITCH recovery source rejects sequence zero]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading SKIP_RETURN_CODE 4)
add_test( [==[File ITCH recovery source rejects zero checkpoint interval]==] /Users/hamzalatif/Desktop/low-latency-trading/build-profile/tests/FileItchRecoverySourceTest [==[File ITCH recovery source rejects zero checkpoint interval]==]  )
set_tests_properties( [==[File ITCH recovery source rejects zero checkpoint interval]==] PROPERTIES WORKING_DIRECTORY /Users/hamzalatif/Desktop/low-latency-trading SKIP_RETURN_CODE 4)
set( FileItchRecoverySourceTest_TESTS [==[
  {
    "class-name" : "",
    "name" : "File ITCH recovery source builds record index",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/FileItchRecoverySourceTest.cpp",
      "line" : 209
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "File ITCH recovery source fails for missing file",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/FileItchRecoverySourceTest.cpp",
      "line" : 422
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "File ITCH recovery source invalidates index when source file size changes",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/FileItchRecoverySourceTest.cpp",
      "line" : 792
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "File ITCH recovery source loads persisted sparse index",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/FileItchRecoverySourceTest.cpp",
      "line" : 548
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "File ITCH recovery source persists sparse index",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/FileItchRecoverySourceTest.cpp",
      "line" : 488
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "File ITCH recovery source rebuilds corrupted persisted index",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/FileItchRecoverySourceTest.cpp",
      "line" : 646
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "File ITCH recovery source recovers exact packet range",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/FileItchRecoverySourceTest.cpp",
      "line" : 226
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "File ITCH recovery source recovers single packet",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/FileItchRecoverySourceTest.cpp",
      "line" : 299
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "File ITCH recovery source rejects invalid range",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/FileItchRecoverySourceTest.cpp",
      "line" : 335
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "File ITCH recovery source rejects range beyond indexed file",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/FileItchRecoverySourceTest.cpp",
      "line" : 396
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "File ITCH recovery source rejects sequence zero",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/FileItchRecoverySourceTest.cpp",
      "line" : 373
    },
    "tags" : 
  }]==] [==[
  {
    "class-name" : "",
    "name" : "File ITCH recovery source rejects zero checkpoint interval",
    "source-location" : 
    {
      "filename" : "/Users/hamzalatif/Desktop/low-latency-trading/tests/market_data/itch/FileItchRecoverySourceTest.cpp",
      "line" : 460
    },
    "tags" : 
  }
]==] [==[File ITCH recovery source builds record index]==] [==[File ITCH recovery source fails for missing file]==] [==[File ITCH recovery source invalidates index when source file size changes]==] [==[File ITCH recovery source loads persisted sparse index]==] [==[File ITCH recovery source persists sparse index]==] [==[File ITCH recovery source rebuilds corrupted persisted index]==] [==[File ITCH recovery source recovers exact packet range]==] [==[File ITCH recovery source recovers single packet]==] [==[File ITCH recovery source rejects invalid range]==] [==[File ITCH recovery source rejects range beyond indexed file]==] [==[File ITCH recovery source rejects sequence zero]==] [==[File ITCH recovery source rejects zero checkpoint interval]==])
