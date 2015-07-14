cmake_minimum_required(VERSION 3.2)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
project(GenomeKit)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Boost 1.58.0 REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIR})

#include_directories("/Volumes/Macintosh HD New/source/jellyfish-2.2.0/include/")
#include_directories("/Volumes/Macintosh HD New/source/boost_1_58_0/")

#find_package(OpenMP REQUIRED)
#if (OPENMP_FOUND)
#    message("OPENMP FOUND")
#    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#endif()



#APP SequenceGenerator
set(SequenceGenerator SequenceGenerator.cpp)
add_executable(SequenceGenerator ${SequenceGenerator})

#APP semantic statistics
set(SemanticStat SemanticStat.cpp)
add_executable(SemanticsStat ${SemanticStat})

set(SOURCE_FILES main.cpp)
add_executable(GenomeKit ${SOURCE_FILES})

set(RandomIndexing RandomIndexing.cpp)
add_executable(RandomIndexing ${RandomIndexing})
target_link_libraries(RandomIndexing ${Boost_LIBRARIES} boost_system)
