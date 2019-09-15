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
On I7 4720HQ(2.8GHZ),HDD, each thread has 100,000 lines to write. All LOGS are Guarenteed!
Totally 150,0000 Lines, which means 150 files, are written.

Because the usage of std::mutex,the lock, the time spent on logging has a linear relationship with the threads number.

Thread count: 1
	Average Log Latency = 4964 nanoseconds
Thread count: 2
	Average Log Latency = 4782 nanoseconds
	Average Log Latency = 5566 nanoseconds
Thread count: 3
	Average Log Latency = 8843 nanoseconds
	Average Log Latency = 9168 nanoseconds
	Average Log Latency = 9279 nanoseconds
Thread count: 4
	Average Log Latency = 15335 nanoseconds
	Average Log Latency = 15874 nanoseconds
	Average Log Latency = 16076 nanoseconds
	Average Log Latency = 16450 nanoseconds
Thread count: 5
	Average Log Latency = 22226 nanoseconds
	Average Log Latency = 22267 nanoseconds
	Average Log Latency = 22302 nanoseconds
	Average Log Latency = 22402 nanoseconds
	Average Log Latency = 22509 nanoseconds

Known Issue:
1. Due to the lack of std::filesystem, it fails when logs dir doesn't exist.
2. Sometimes one log file may have less or more than 10000 lines. I'm digging into the reasons.
