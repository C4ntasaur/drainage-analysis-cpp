cmake_minimum_required(VERSION 3.10)
project(TerrainAnalysis)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)

# Source Files
set(SOURCES
    src/argumentParser.cpp
    src/CLIhelperFunctions.cpp
    src/MapGeneral.cpp
    src/MapGeneral_IO.cpp
    src/MapGeneral_Export.cpp
    src/modifyDEM.cpp
    src/MapVector.cpp
    src/SobelAnalysis.cpp
    src/D8FlowAnalyser.cpp
    src/FlowAccumulation.cpp
    src/watershedAnalysis.cpp
    src/main.cpp
)

add_executable(terrain_analysis ${SOURCES})