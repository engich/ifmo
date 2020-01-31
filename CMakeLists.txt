cmake_minimum_required(VERSION 3.15)
project(w_model)

set(CMAKE_CXX_STANDARD 20)


include_directories(paradiseo)

file(GLOB_RECURSE SOURCES "paradiseo/*.cpp")

add_executable(w_model main.cpp ${SOURCES})
