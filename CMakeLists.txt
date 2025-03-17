cmake_minimum_required(VERSION 3.10)
project(TerrainAnalysis)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)

# Source Files
set(SOURCES
    src/CLI/argumentParser.cpp
    src/CLI/CLIhelperFunctions.cpp
    src/CLI/REPL.cpp
    src/CLI/MapProcessing.cpp
    src/CLI/CLIHandler.cpp
    src/map_core/MapGeneral.cpp
    src/map_core/MapGeneral_IO.cpp
    src/image_handling/ImageExport.cpp
    src/map_core/modifyDEM.cpp
    src/map_core/MapVector.cpp
    src/DEM_analysis/SobelAnalysis.cpp
    src/DEM_analysis/D8FlowAnalyser.cpp
    src/DEM_analysis/FlowAccumulation.cpp
    src/DEM_analysis/watershedAnalysis.cpp
    src/main.cpp
)

add_executable(drainage-analysis ${SOURCES})
