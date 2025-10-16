cmake_minimum_required(VERSION 3.20)
project(ASC_HPC)


# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELWITHDEBINFO CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")


include_directories(src concurrentqueue)


add_executable (demo_tasks demos/demo_tasks.cpp 
                src/taskmanager.cpp src/timer.cpp)
                
target_sources (demo_tasks PUBLIC src/taskmanager.hpp src/timer.hpp)




add_executable (demo_simd demos/demo_simd.cpp
                src/timer.cpp)
                
target_sources (demo_simd PUBLIC src/simd.hpp src/timer.hpp)


add_executable (simd_timings demos/simd_timings.cpp)
target_sources (simd_timings PUBLIC src/simd.hpp)


add_executable (timing_mem demos/timing_mem.cpp)
target_sources (timing_mem PUBLIC src/simd.hpp)

