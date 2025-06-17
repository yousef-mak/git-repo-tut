cmake_minimum_required(VERSION 3.16)

# Define the core library
add_library(TicTacToeCore STATIC
    src/GameBoard.cpp
    src/AIPlayer.cpp
    src/UserManager.cpp
    src/GameHistory.cpp
    src/GameStateStack.cpp
)

# Add include directories for public headers
target_include_directories(TicTacToeCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)