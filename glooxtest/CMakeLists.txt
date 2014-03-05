project(glooxtest)

cmake_minimum_required(VERSION 2.8)

add_executable(glooxtest main.cpp)

find_library(GLOOX_LIBRARY gloox)
target_link_libraries(glooxtest ${GLOOX_LIBRARY})

