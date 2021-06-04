cmake_minimum_required(VERSION 3.17)
project(code)

set(CMAKE_CXX_STANDARD 11)

add_executable(code main.cpp Glossary.h Analysis.h Analysis.cpp Looks.h Looks.cpp out.h out.cpp error.h error.cpp Glossary.cpp)