cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Specify the path to the HybridCache library
set(HYBRID_CACHE_DIR "/home/chang/CLionProjects/Hybrid-Cache/cmake-build-debug")

# Locate the HybridCache library
find_library(HYBRID_CACHE_LIB NAMES HybridCache PATHS ${HYBRID_CACHE_DIR} REQUIRED)

# Include directories
include_directories("/home/chang/CLionProjects/Hybrid-Cache/include")  # Path to HybridCache headers

# Add your project's executable
add_executable(MyExecutable testLib.cpp)

# Link against the HybridCache library
target_link_libraries(MyExecutable ${HYBRID_CACHE_LIB})