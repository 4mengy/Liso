cmake_minimum_required(VERSION 3.12)
project(echo-server-with-select)

set(CMAKE_CXX_STANDARD 11)

add_executable(server server.cpp common.h common.cpp)

add_executable(client client.cpp common.h common.cpp)
