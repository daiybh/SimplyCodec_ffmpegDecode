cmake_minimum_required (VERSION 3.8)
include(submoduleX/simplylivebase/simplylive.make)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CUDA_STANDARD_REQUIRED TRUE)
set(CMAKE_CUDA_ARCHITECTURES 60)

project("DNxGPU" CUDA CXX)
add_subdirectory ("source/Lib.tcMarker")
add_subdirectory ("source/Main")




