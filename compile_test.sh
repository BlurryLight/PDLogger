#!/bin/bash 

mkdir -p build && cd build
export C=clang
export CXX=clang++
cmake .. && make

# build test
clang++ -O3 -lpthread -L. -lPDLog -I.. -Wl,-rpath=. ../examples/file_logger.cc -o file_logger.out


cd ..
