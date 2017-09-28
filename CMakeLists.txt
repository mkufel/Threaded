cmake_minimum_required(VERSION 3.8)
project(Threaded)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        flip.c
        flip.h
        threaded_basics.c
        uint128.h)

add_executable(Threaded ${SOURCE_FILES})