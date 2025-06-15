cmake_minimum_required(VERSION 3.16)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# تفعيل MOC تلقائياً
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# إنشاء التطبيق الرئيسي
qt6_add_executable(TicTacToeGUI
    src/main.cpp
    src/TicTacToeGUI.cpp
    include/TicTacToeGUI.h
)

target_include_directories(TicTacToeGUI PRIVATE include)

target_link_libraries(TicTacToeGUI PRIVATE
    Qt6::Core
    Qt6::Widgets
    TicTacToeCore
)
