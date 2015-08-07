cmake_minimum_required(VERSION 3.1)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/bin")
set(CMAKE_CXX_COMPILER "clang-omp++")
project(GenomeKit)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Boost 1.58.0 REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIR})

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

set(RIGenome RIGenome.cpp)
add_executable(RIGenome ${RIGenome})
target_link_libraries(RIGenome ${Boost_LIBRARIES} boost_system)
