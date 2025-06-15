cmake_minimum_required(VERSION 3.16)

# إنشاء مكتبة Core
add_library(TicTacToeCore STATIC
    src/GameBoard.cpp
    src/AIPlayer.cpp
    src/UserManager.cpp
    src/GameHistory.cpp
    src/GameStateStack.cpp
)

target_include_directories(TicTacToeCore PUBLIC include)
