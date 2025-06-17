cmake_minimum_required(VERSION 3.16)
project(AdvancedTicTacToe VERSION 1.0.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt
find_package(Qt6 COMPONENTS Core Widgets QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

# Enable testing
option(ENABLE_TESTING "Enable testing" ON)
if(ENABLE_TESTING)
    enable_testing()
    
    # Fetch GoogleTest
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/GUI/include)

set(SOURCES
    core/src/AIPlayer.cpp
    core/src/GameBoard.cpp
    core/src/GameHistory.cpp
    core/src/GameStateStack.cpp
    core/src/UserManager.cpp
    GUI/src/TicTacToeGUI.cpp
    GUI/src/main.cpp
    GUI/include/TicTacToeGUI.h 
)

# Create library from core components
add_library(game_core ${SOURCES})
target_link_libraries(game_core Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets)

# Main executable
add_executable(tic_tac_toe_game ${SOURCES} )
target_link_libraries(tic_tac_toe_game Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets)

# Testing configuration
if(ENABLE_TESTING)
    # Individual component tests
    add_executable(AIPlayer_test_test tests/unit_tests/AIPlayer_test.cpp)
    target_link_libraries(AIPlayer_test_test game_core gtest gtest_main)
    add_test(NAME AIPlayerTest COMMAND AIPlayer_test_test)
    
    add_executable(gameboard_test tests/unit_tests/GameBoard_test.cpp)
    target_link_libraries(gameboard_test game_core gtest gtest_main)
    add_test(NAME GameBoardTest COMMAND gameboard_test)
    
    add_executable(gamehistory_test tests/unit_tests/GameHistory_test.cpp)
    target_link_libraries(gamehistory_test game_core gtest gtest_main)
    add_test(NAME GameHistoryTest COMMAND gamehistory_test)
    
    add_executable(gamestack_test tests/unit_tests/GameStateStack_test.cpp)
    target_link_libraries(gamestack_test game_core gtest gtest_main)
    add_test(NAME GameStackTest COMMAND gamestack_test)
    
    add_executable(usermanager_test tests/unit_tests/UserManager_test.cpp)
    target_link_libraries(usermanager_test game_core gtest gtest_main)
    add_test(NAME UserManagerTest COMMAND usermanager_test)
    
    # Comprehensive integration test
    add_executable(integration_test tests/integration_tests/IntegrationTests.cpp)
    target_link_libraries(integration_test game_core gtest gtest_main)
    add_test(NAME IntegrationTest COMMAND integration_test)
endif()
