cmake_minimum_required(VERSION 3.29)
project(faster_huff CUDA)

set(CMAKE_CUDA_STANDARD 20)

# 设置 CMake 使用正确的运行时库



add_executable(faster_huff main.cu
        huffman.cpp
        huffman.h
)

set_target_properties(faster_huff PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON)
