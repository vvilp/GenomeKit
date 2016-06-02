# cmake_minimum_required(VERSION 3.1)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/bin")

ADD_DEFINITIONS(
    -std=c++11
)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# set(CMAKE_CXX_COMPILER "/usr/local/bin/g++")
# set(CMAKE_C_COMPILER "/usr/local/bin/gcc")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

project(GenomeKit)

set(HEADER_FILES Util.hpp)


set(SemanticNeuralNetwork SemanticNeuralNetwork.cpp)
add_executable(SemanticNeuralNetwork ${SemanticNeuralNetwork} ${HEADER_FILES})

set(GeneSigDistance GeneSigDistance.cpp)
add_executable(GeneSigDistance ${GeneSigDistance} ${HEADER_FILES})

set(GeneSigDistance-partition GeneSigDistance-partition.cpp)
add_executable(GeneSigDistance-partition ${GeneSigDistance-partition} ${HEADER_FILES})

# find_package(Boost 1.58.0 REQUIRED COMPONENTS filesystem)
# include_directories(${Boost_INCLUDE_DIR})
#
# #APP SequenceGenerator
# set(SequenceGenerator SequenceGenerator.cpp)
# add_executable(SequenceGenerator ${SequenceGenerator})
#
# #APP semantic statistics
# set(SemanticStat SemanticStat.cpp)
# add_executable(SemanticsStat ${SemanticStat})
#
# set(SOURCE_FILES main.cpp)
# add_executable(GenomeKit ${SOURCE_FILES})
#
# set(RandomIndexing RandomIndexing.cpp)
# add_executable(RandomIndexing ${RandomIndexing})
# target_link_libraries(RandomIndexing ${Boost_LIBRARIES} boost_system)
#
# set(RIGenome RIGenome.cpp)
# add_executable(RIGenome ${RIGenome})
# target_link_libraries(RIGenome ${Boost_LIBRARIES} boost_system)
