cmake_minimum_required(VERSION 3.22)
project(parallels1 C)

set(CMAKE_C_STANDARD 11)

add_executable(parallels1 main.c)
target_link_libraries(parallels1 pthread)
