add_executable(trading_app
    main.cpp
)


target_link_libraries(trading_app
    PRIVATE
        common
        feed_handler
)