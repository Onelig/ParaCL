cmake_minimum_required(VERSION 3.14)

project(ParaCL)

set(CMAKE_CXX_STANDARD 20)          
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCE_MAIN ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
add_executable(ParaCL ${SOURCE_MAIN})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/definitions)

target_link_libraries(ParaCL PUBLIC definitions_lib)
