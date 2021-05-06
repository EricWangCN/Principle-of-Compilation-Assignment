cmake_minimum_required(VERSION 3.17)
project(code)

set(CMAKE_CXX_STANDARD 20)

add_executable(code main.cpp Classification.h Classification.cpp Handler.h Handler.cpp Output.h Output.cpp)