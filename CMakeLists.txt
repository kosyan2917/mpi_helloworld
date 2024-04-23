cmake_minimum_required(VERSION 3.22)
project(mpi_helloworld)

set(CMAKE_CXX_STANDARD 17)


add_executable(mpi_helloworld main.cpp)
#        tests.cpp
#        task1_hello_world_openmp.cpp
#        task2_1nsum_openmp.cpp)

add_subdirectory(matrix_multiplication)