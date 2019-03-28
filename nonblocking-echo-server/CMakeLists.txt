cmake_minimum_required(VERSION 3.12)
project(nonblocking-echo-server)

set(CMAKE_CXX_STANDARD 11)

add_executable(server server.cpp common.h)
add_executable(client client.cpp common.h)
