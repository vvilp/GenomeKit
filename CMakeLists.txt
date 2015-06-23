cmake_minimum_required(VERSION 3.2)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
project(GenomeKit)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#APP SequenceGenerator
set(SequenceGenerator SequenceGenerator.cpp)
add_executable(SequenceGenerator ${SequenceGenerator})

set(SOURCE_FILES main.cpp)
add_executable(GenomeKit ${SOURCE_FILES})



