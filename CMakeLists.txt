cmake_minimum_required(VERSION 3.10)

project(HybridCacheExample)

set(CMAKE_CXX_STANDARD 11)

add_executable(HybridCacheExample HybridCache.cpp)

# You may need to adjust the paths if your system has a different location for libraries.
target_include_directories(HybridCacheExample PRIVATE /usr/local/include)
#target_link_libraries(HybridCacheExample PRIVATE /usr/local/lib/libfmt.a)

add_executable(testFileReadAndWrite tests/testFileReadAndWrite.cpp)
add_executable(testTypeSize tests/testTypeSize.cpp)