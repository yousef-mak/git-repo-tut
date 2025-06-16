cmake_minimum_required(VERSION 3.16)
project(AdvancedTicTacToe VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    endif()
endif()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Optional: Find OpenSSL for password hashing (if needed)
find_package(OpenSSL QUIET)
if(OpenSSL_FOUND)
    message(STATUS "OpenSSL found - secure hashing available")
endif()

# Enable testing
option(BUILD_TESTING "Build tests" ON)
if(BUILD_TESTING)
    enable_testing()
    find_package(GTest QUIET)
    if(NOT GTest_FOUND)
        # Try to find GTest manually
        find_path(GTEST_INCLUDE_DIRS gtest/gtest.h)
        find_library(GTEST_LIBRARIES gtest)
        find_library(GTEST_MAIN_LIBRARIES gtest_main)
        if(GTEST_INCLUDE_DIRS AND GTEST_LIBRARIES AND GTEST_MAIN_LIBRARIES)
            set(GTest_FOUND TRUE)
        endif()
    endif()
endif()

# Create directories for source organization
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/include)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/resources)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files (adjust paths as needed for your project structure)
set(CORE_SOURCES
    src/main.cpp
    src/Game.cpp
    src/Board.cpp
    src/Player.cpp
    src/AIPlayer.cpp
    src/GameHistory.cpp
    src/UserAuth.cpp
    src/FileStorage.cpp
)

# Header files (for IDE organization)
set(CORE_HEADERS
    include/Game.h
    include/Board.h
    include/Player.h
    include/AIPlayer.h
    include/GameHistory.h
    include/UserAuth.h
    include/FileStorage.h
)

# GUI source files
set(GUI_SOURCES
    src/gui/MainWindow.cpp
    src/gui/GameWidget.cpp
    src/gui/LoginDialog.cpp
    src/gui/HistoryWidget.cpp
)

set(GUI_HEADERS
    include/gui/MainWindow.h
    include/gui/GameWidget.h
    include/gui/LoginDialog.h
    include/gui/HistoryWidget.h
)

# Qt MOC files (if you use Q_OBJECT)
qt6_standard_project_setup()

# Main executable
add_executable(${PROJECT_NAME}
    ${CORE_SOURCES}
    ${CORE_HEADERS}
    ${GUI_SOURCES}
    ${GUI_HEADERS}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
    pthread
)

# Link OpenSSL if available (for secure password hashing)
if(OpenSSL_FOUND)
    target_link_libraries(${PROJECT_NAME} OpenSSL::SSL OpenSSL::Crypto)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_OPENSSL)
endif()

# Set target properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Include directories for custom file storage
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Add compiler definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    QT_NO_DEBUG_OUTPUT
    $<$<CONFIG:Debug>:DEBUG_MODE>
)

# Copy resources to build directory
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})

# Testing
if(BUILD_TESTING AND GTest_FOUND)
    # Test sources
    set(TEST_SOURCES
        tests/test_board.cpp
        tests/test_game.cpp
        tests/test_ai.cpp
        tests/test_user_auth.cpp
        tests/test_file_storage.cpp
    )
    
    # Core library (without main.cpp for testing)
    set(CORE_LIB_SOURCES ${CORE_SOURCES})
    list(REMOVE_ITEM CORE_LIB_SOURCES src/main.cpp)
    
    add_library(${PROJECT_NAME}_core STATIC
        ${CORE_LIB_SOURCES}
        ${CORE_HEADERS}
    )
    
    target_link_libraries(${PROJECT_NAME}_core
        Qt6::Core
        Qt6::Widgets
        pthread
    )
    
    # Link OpenSSL if available
    if(OpenSSL_FOUND)
        target_link_libraries(${PROJECT_NAME}_core OpenSSL::SSL OpenSSL::Crypto)
        target_compile_definitions(${PROJECT_NAME}_core PRIVATE HAVE_OPENSSL)
    endif()
    
    # Unit tests executable
    add_executable(${PROJECT_NAME}_tests
        ${TEST_SOURCES}
    )
    
    if(TARGET GTest::gtest AND TARGET GTest::gtest_main)
        target_link_libraries(${PROJECT_NAME}_tests
            ${PROJECT_NAME}_core
            GTest::gtest
            GTest::gtest_main
        )
    else()
        target_include_directories(${PROJECT_NAME}_tests PRIVATE ${GTEST_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME}_tests
            ${PROJECT_NAME}_core
            ${GTEST_LIBRARIES}
            ${GTEST_MAIN_LIBRARIES}
        )
    endif()
    
    # Add tests to CTest
    add_test(NAME UnitTests COMMAND ${PROJECT_NAME}_tests)
    
    # Integration tests (optional)
    add_executable(${PROJECT_NAME}_integration_tests
        tests/integration/test_full_game.cpp
        tests/integration/test_file_storage_integration.cpp
    )
    
    if(TARGET GTest::gtest AND TARGET GTest::gtest_main)
        target_link_libraries(${PROJECT_NAME}_integration_tests
            ${PROJECT_NAME}_core
            GTest::gtest
            GTest::gtest_main
        )
    else()
        target_include_directories(${PROJECT_NAME}_integration_tests PRIVATE ${GTEST_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME}_integration_tests
            ${PROJECT_NAME}_core
            ${GTEST_LIBRARIES}
            ${GTEST_MAIN_LIBRARIES}
        )
    endif()
    
    add_test(NAME IntegrationTests COMMAND ${PROJECT_NAME}_integration_tests)
endif()

# Performance tests (optional benchmark executable)
option(BUILD_BENCHMARKS "Build performance benchmarks" OFF)
if(BUILD_BENCHMARKS)
    add_executable(${PROJECT_NAME}_perf_tests
        tests/performance/benchmark_ai.cpp
        tests/performance/benchmark_file_storage.cpp
    )
    
    target_link_libraries(${PROJECT_NAME}_perf_tests
        ${PROJECT_NAME}_core
    )
endif()

# Installation rules
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)

# CPack configuration for packaging
set(CPACK_PACKAGE_NAME "Advanced Tic Tac Toe")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Advanced Tic Tac Toe Game with AI")
set(CPACK_PACKAGE_VENDOR "Your Team Name")

if(WIN32)
    set(CPACK_GENERATOR "ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)

# Print configuration summary
message(STATUS "")
message(STATUS "Advanced Tic Tac Toe Configuration Summary:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Qt6 found: ${Qt6_FOUND}")
message(STATUS "  OpenSSL found: ${OpenSSL_FOUND}")
message(STATUS "  Testing enabled: ${BUILD_TESTING}")
if(BUILD_TESTING)
    message(STATUS "  GTest found: ${GTest_FOUND}")
endif()
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
