cmake_minimum_required(VERSION 3.5)
project(PDLog
    DESCRIPTION "a small but powerful logger"
    LANGUAGES CXX)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_library(PDLog SHARED
    PDLog.h
    PDLog.cc
   # buffer.h
   # buffer.cc
)
add_executable(benchmark examples/pdlog_benchmark.cpp)
target_link_libraries(benchmark PDLog Threads::Threads)
if ( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
    target_compile_options(PDLog PRIVATE -Wall)
endif()

