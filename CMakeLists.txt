cmake_minimum_required(VERSION 3.10)
project(https_example)

find_package(ixwebsocket REQUIRED)

add_executable(https_example main.cpp)
target_link_libraries(https_example PRIVATE ixwebsocket::ixwebsocket) 