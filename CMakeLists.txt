cmake_minimum_required (VERSION 3.8)
include(submodule/simplylivebase/simplylive.make)

set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CUDA_STANDARD_REQUIRED TRUE)
set(CMAKE_CUDA_ARCHITECTURES 60)

project("DNxGPU" CUDA CXX)

add_subdirectory ("source/Lib.tcMarker")
add_subdirectory ("source/Main")




