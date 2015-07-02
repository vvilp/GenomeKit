cmake_minimum_required(VERSION 3.2)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
project(GenomeKit)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

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



