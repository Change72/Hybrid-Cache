cmake_minimum_required(VERSION 3.10)

project(HybridCacheExample)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)
file(GLOB APP_HEADERS include/*.h)
file(GLOB APP_SOURCES src/*.cpp)

add_executable(testFileReadAndWrite tests/testFileReadAndWrite.cpp)
add_executable(testTypeSize tests/testTypeSize.cpp)

add_library(HybridCache SHARED ${APP_HEADERS} ${APP_SOURCES})

add_executable(HybridCacheExample main.cpp ${APP_SOURCES})