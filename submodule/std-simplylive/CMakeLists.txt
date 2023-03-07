cmake_minimum_required (VERSION 3.8)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED True)

project("std-simplylive")

add_definitions(-DUNICODE -D_UNICODE)

include_directories(include)

add_library (std-simplylive INTERFACE)

target_include_directories(std-simplylive INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

