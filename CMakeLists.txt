cmake_minimum_required(VERSION 3.20)
project(ASC_HPC)

set (CMAKE_CXX_STANDARD 17)

include_directories(src concurrentqueue)


add_executable (demo_tasks demos/demo_tasks.cc 
                src/taskmanager.cc)

