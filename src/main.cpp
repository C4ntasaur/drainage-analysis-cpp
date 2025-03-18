/**
 * @file main.cpp
 * @author Ollie
 * @brief Main function
 * @version 1.0.0
 * @date 2025-03-18
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "CLI/CLIHandler.h"
#include "CLI/MapProcessing.h"
#include "CLI/argumentParser.h"
#include "map_core/Map.h"
#include "image_handling/BMP.h"
#include "DEM_analysis/SobelAnalysis.h"
#include "DEM_analysis/D8FlowAnalyser.h"
#include "DEM_analysis/FlowAccumulation.h"
#include "DEM_analysis/watershedAnalysis.h"
#include "image_handling/ImageExport.h"
#include "CLI/REPL.h"

#include <iostream>
#include <sstream>
#include <cstring>

/**
 * @brief 
 * 
 * @param argc Number of arguments passed with binary
 * @param argv Array of arguments passed with binary
 * @return int success or failure
 */
int main(int argc, char* argv[]) {
    // Check if REPL mode was wanted
    if ((argc == 2) && (strcmp(argv[1], "-int") == 0 || strcmp(argv[1], "--interactive") == 0)) {
        // Run REPL if -int or --interactive flag is provided
        runREPL();
    }
    // No flags valid with REPL other than -int / --interactive
    else if ((argc > 2) && (strcmp(argv[1], "-int") == 0 || strcmp(argv[1], "--interactive") == 0)) {
        // Error if additional flags are provided with -int or --interactive
        std::cerr << "Error: No other flags should be provided with -int or --interactive.\n";
        return 1;
    }
    else { // Run CLI version
    // Manually allocate memory for strings and flags
    char* input_file = nullptr;
    char* input_file_type = nullptr;
    char* output_file = nullptr;
    char* image_file = nullptr;
    bool colour = false;
    char* colour_type = nullptr;
    bool totalFlow = false;
    bool watershed = false;
    int nPourPoints = 0;
    char* watershed_directory;
    char* watershed_colour;
    bool verbose = false;
    char* process = nullptr;

    // Check all arguments from argv
    if (!parseArguments(argc, argv, input_file, input_file_type, output_file, image_file, colour, colour_type, totalFlow, watershed, nPourPoints, watershed_directory, watershed_colour, verbose, process)) {
        delete[] input_file;
        delete[] input_file_type;
        delete[] output_file;
        delete[] image_file;
        delete[] colour_type;
        delete[] process;
        return 1;
    }

    // Check all arguments are valid and do not conflict
    if (!validateArguments(input_file, input_file_type, output_file, image_file, colour, colour_type, totalFlow, watershed, nPourPoints, process)) {
        delete[] input_file;
        delete[] input_file_type;
        delete[] output_file;
        delete[] image_file;
        delete[] colour_type;
        delete[] process;
        return 1;
    }

    // Print verbose output if specified
    printVerboseOutput(input_file, process, output_file, image_file, colour_type, verbose, watershed, nPourPoints, watershed_directory, watershed_colour, totalFlow);


    // Create elevationMap (DEM) and load it
    Map<double> elevationMap;
    if (!elevationMap.loadFromFile(input_file, input_file_type)) {
        std::cerr << "File: " << input_file << " does not exist." << std::endl;
        delete[] input_file;
        delete[] input_file_type;
        delete[] output_file;
        delete[] image_file;
        delete[] colour_type;
        delete[] process;
        return 1;
    }

    // Fill sinks in DEM
    elevationMap.fillSinks();

    // Init output / processing maps
    Map<int> D8Map;
    Map<double> flowMap;
    Map<double> GMap;
    Map<double> aspectMap;
    std::string flowType;

    // Create processing maps from -p process
    processMap(elevationMap, process, D8Map, flowMap, GMap, aspectMap, flowType);

    // Run flow accumulation for -p if -fa specified
    handleFlowAccumulation(elevationMap, D8Map, flowMap, GMap, aspectMap, flowType, totalFlow);

    // Run watershed delineation for -p if -w specified
    handleWatershed(elevationMap, D8Map, flowMap, GMap, aspectMap, flowType, watershed, nPourPoints, watershed_directory, watershed_colour);

    // Output all necessary types for either -o or -img as specified
    handleOutput(flowMap, D8Map, aspectMap, GMap, output_file, image_file, input_file_type, colour_type, process, totalFlow, watershed);

    // Delete mem.
    delete[] input_file;
    delete[] input_file_type;
    delete[] output_file;
    delete[] image_file;
    delete[] colour_type;
    delete[] process;
    return 0;
    }
}