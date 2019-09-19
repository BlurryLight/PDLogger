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
On I7 4720HQ(2.8GHZ), HDD, each thread has 100,000 lines to write. All LOGS are Guarenteed!
Totally 150,0000 Lines, which means 150 files, are written.

Because the usage of std::mutex,the lock, the time spent on logging has a linear relationship with the threads number.

1. Manjaro 18.10, gcc 9.1, release
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

2. Windows 10, MSVC 2019, release
Thread count: 1
        Average Log Latency = 5685 nanoseconds
Thread count: 2
        Average Log Latency = 7040 nanoseconds
        Average Log Latency = 7860 nanoseconds
Thread count: 3
        Average Log Latency = 9694 nanoseconds
        Average Log Latency = 9750 nanoseconds
        Average Log Latency = 9823 nanoseconds
Thread count: 4
        Average Log Latency = 18900 nanoseconds
        Average Log Latency = 19044 nanoseconds
        Average Log Latency = 19184 nanoseconds
        Average Log Latency = 19293 nanoseconds
Thread count: 5
        Average Log Latency = 24487 nanoseconds
        Average Log Latency = 24849 nanoseconds
        Average Log Latency = 24882 nanoseconds
        Average Log Latency = 25018 nanoseconds
        Average Log Latency = 25224 nanoseconds

3. Windows 10, Mingw 9 and Mingw 7, release
Fails, serious data racing.

Known Issue:
1. In Linux with gcc, when meeting tons of logs and multiple threads, one log file may have less or more than 10000 lines(like 9999 or 10001 lines).
It's a rare situation, at least for me, so I will just keep an eye on it and try to find the reasons.

