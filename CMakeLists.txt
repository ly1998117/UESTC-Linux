cmake_minimum_required(VERSION 3.10)
project(LinuxCode)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

aux_source_directory(. SRC_LIST) #搜索当前目录下的所有.cpp文件

add_executable(data ${SRC_LIST})



add_executable(BPlusTree BPlusTree.cpp)