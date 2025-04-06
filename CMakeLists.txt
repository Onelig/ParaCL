cmake_minimum_required(VERSION 3.14)

project(ParaCL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


add_executable(ParaCL src/main.cpp definitions/lexer.cpp definitions/parser.cpp definitions/simulator.cpp)

target_include_directories(ParaCL PRIVATE include)
