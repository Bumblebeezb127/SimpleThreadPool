cmake_minimum_required(VERSION 3.16)
project(SimpleThreadPool LANGUAGES CXX)

# C++11 is required (the thread pool uses std::thread, std::function, etc.)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Put all binaries in a single tree when using a single-config generator
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()

if(MSVC)
    add_compile_options(/W3)
endif()

add_subdirectory(ThreadPool)
