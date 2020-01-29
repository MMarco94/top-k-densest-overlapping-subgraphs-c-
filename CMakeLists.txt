# cmake_minimum_required(VERSION <specify CMake version here>)
project(c__)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -march=native")

add_executable(c__
        src/main.cpp src/Graph.h src/Distances.h src/DOS.h src/Peeler.h src/GraphTranslator.h src/utils.cpp
        src/priorityQueue/MinHeap.h src/priorityQueue/BatchMinHeap.h src/priorityQueue/LinearQueue.h)