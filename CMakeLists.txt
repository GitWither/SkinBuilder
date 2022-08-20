cmake_minimum_required(VERSION 3.10)

project("skin-builder")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

add_executable("${PROJECT_NAME}" "src/Main.cpp")