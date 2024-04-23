cmake_minimum_required(VERSION 3.22)
project(example)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(matplotplusplus
        GIT_REPOSITORY https://github.com/alandefreitas/matplotplusplus
        GIT_TAG origin/master) # or whatever tag you want
FetchContent_GetProperties(matplotplusplus)
if(NOT matplotplusplus_POPULATED)
    FetchContent_Populate(matplotplusplus)
    add_subdirectory(${matplotplusplus_SOURCE_DIR} ${matplotplusplus_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

find_package (OpenMP REQUIRED)

add_executable(example test.cpp)
target_link_libraries(example PRIVATE matplot)
target_link_libraries(example PRIVATE OpenMP::OpenMP_CXX)