cmake_minimum_required(VERSION 3.27)
project(sudoku)

set(CMAKE_CXX_STANDARD 17)

add_executable(sudoku sudoku.cpp)
target_include_directories(sudoku PRIVATE ../galib)
target_link_directories(sudoku PRIVATE ../galib/ga)
target_link_libraries(sudoku PRIVATE ga)