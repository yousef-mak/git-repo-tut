enable_testing()

add_executable(AllTests
    main.cpp
    AIPlayer_test.cpp
    GameHistory_test.cpp
    GameStateStack_test.cpp
    UserManager_test.cpp
    IntegrationTests.cpp
)

target_link_libraries(AllTests
    PRIVATE
        TicTacToeCore
        GTest::gtest
        GTest::gtest_main
)

#include(GoogleTest)
#gtest_discover_tests(AllTests)
add_test(NAME RunAllTests COMMAND AllTests)