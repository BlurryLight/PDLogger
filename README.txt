Inspired by kevinkreiser's great logging tool[https://gist.github.com/kevinkreiser/39f2e39273c625d96790],
PDLog is a thread-safe, cross-platform, fast and user-friendly logging tool.


https://github.com/BlurryLight/PDLogger


Requires:
    CXX compiler supports C++0x
    CMake >= 3.5
    
How to compile
    mkdir build && cd build && cmake .. & make
    
Usage:
    Examples show how to work with the tool.

Benchmark:

Because the usage of std::mutex,the lock, the time spent on logging has a linear relationship with the threads number.

Thread count: 1
        Average Log Latency = 5382 nanoseconds
Thread count: 2
        Average Log Latency = 8360 nanoseconds
        Average Log Latency = 8400 nanoseconds
Thread count: 3
        Average Log Latency = 16731 nanoseconds
        Average Log Latency = 16765 nanoseconds
        Average Log Latency = 16781 nanoseconds
Thread count: 4
        Average Log Latency = 23563 nanoseconds
        Average Log Latency = 23992 nanoseconds
        Average Log Latency = 24026 nanoseconds
        Average Log Latency = 24030 nanoseconds
Thread count: 5
        Average Log Latency = 31948 nanoseconds
        Average Log Latency = 32414 nanoseconds
        Average Log Latency = 32493 nanoseconds
        Average Log Latency = 32589 nanoseconds
        Average Log Latency = 32666 nanoseconds
