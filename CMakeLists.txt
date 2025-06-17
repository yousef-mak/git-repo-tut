cmake_minimum_required(VERSION 3.16)
project(TicTacToeGame)

# — استخدام C و C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# — فرض MinGW toolchain
set(CMAKE_C_COMPILER "C:/Qt/Tools/mingw1310_64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/Qt/Tools/mingw1310_64/bin/g++.exe")

# — مسار تثبيت Qt6
set(CMAKE_PREFIX_PATH "C:/Qt/6.9.1/mingw_64/lib/cmake")

# ======== Coverage Support ========
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(STATUS "Code coverage enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -O0")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

option(ENABLE_TESTING "Enable testing" ON)
# Enable testing
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
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/GUI/include)

# Core source files (your specific files)
set(CORE_SOURCES
    core/src/aiplayer.cpp
    core/src/gameboard.cpp
    core/src/gamehistory.cpp
    core/src/gamestack.cpp
    core/src/_main.cpp
    core/src/_usermanager.cpp
)

add_library(game_core ${CORE_SOURCES})
target_link_libraries(game_core Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets)

# Main executable
add_executable(tic_tac_toe_game ${CORE_SOURCES})
target_link_libraries(tic_tac_toe_game Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets)

# Testing configuration
if(ENABLE_TESTING)
    # Individual component tests
    add_executable(aiplayer_test tests/unit_tests/AIPlayer_test.cpp)
    target_link_libraries(aiplayer_test game_core gtest gtest_main)
    add_test(NAME AIPlayerTest COMMAND aiplayer_test)
    
    add_executable(gameboard_test tests/unit_tests/GameBoard_test.cpp)
    target_link_libraries(gameboard_test game_core gtest gtest_main)
    add_test(NAME GameBoardTest COMMAND gameboard_test)
    
    add_executable(gamehistory_test tests/unit_tests/GameHistory_test.cpp)
    target_link_libraries(gamehistory_test game_core gtest gtest_main)
    add_test(NAME GameHistoryTest COMMAND gamehistory_test)
    
    add_executable(gamestack_test tests/unit_tests/GameStack_test.cpp)
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
