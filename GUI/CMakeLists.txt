cmake_minimum_required(VERSION 3.16)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(TicTacToeGUI
    src/main.cpp
    src/TicTacToeGUI.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/TicTacToeGUI.h
)

target_include_directories(TicTacToeGUI PUBLIC
    ${PROJECT_SOURCE_DIR}/core/include
    ${PROJECT_SOURCE_DIR}/gui/include
)

target_link_libraries(TicTacToeGUI
    PRIVATE
        Qt6::Widgets
        TicTacToeCore
)