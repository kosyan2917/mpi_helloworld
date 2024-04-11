cmake_minimum_required(VERSION 3.22)
project(parallels2 C)

set(CMAKE_C_STANDARD 11)

add_executable(parallels2 main.c)
target_link_libraries(parallels2 pthread)
