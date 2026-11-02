add_library(feed_handler STATIC
    FeedHandler.cpp
)


target_link_libraries(feed_handler
    PRIVATE
        common
)


target_include_directories(feed_handler
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)