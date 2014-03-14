project(glooxtest)

cmake_minimum_required(VERSION 2.8)

add_executable(glooxtest main.cpp configuredclientfactory.cpp spacecontrolclient.cpp)

find_library(GLOOX_LIBRARY gloox)
target_link_libraries(glooxtest ${GLOOX_LIBRARY})

find_library(CONFIG_LIBRARY config++)
target_link_libraries(glooxtest ${CONFIG_LIBRARY})
