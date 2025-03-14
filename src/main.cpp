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

#include <iostream>
#include <sstream>
#include <cstring>

int main(int argc, char* argv[]) {
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

    if (!parseArguments(argc, argv, input_file, input_file_type, output_file, image_file, colour, colour_type, totalFlow, watershed, nPourPoints, watershed_directory, watershed_colour, verbose, process)) {
        delete[] input_file;
        delete[] input_file_type;
        delete[] output_file;
        delete[] image_file;
        delete[] colour_type;
        delete[] process;
        return 1;
    }

    if (!validateArguments(input_file, input_file_type, output_file, image_file, colour, colour_type, totalFlow, watershed, nPourPoints, process)) {
        delete[] input_file;
        delete[] input_file_type;
        delete[] output_file;
        delete[] image_file;
        delete[] colour_type;
        delete[] process;
        return 1;
    }

    printVerboseOutput(input_file, process, output_file, image_file, colour_type, verbose, watershed, nPourPoints, watershed_directory, watershed_colour, totalFlow);

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

    elevationMap.fillSinks();

    Map<int> D8Map;
    Map<double> flowMap;
    Map<double> GMap;
    Map<double> aspectMap;
    std::string flowType;

    processMap(elevationMap, process, D8Map, flowMap, GMap, aspectMap, flowType);
    handleFlowAccumulation(elevationMap, D8Map, flowMap, GMap, aspectMap, flowType, totalFlow);
    handleWatershed(elevationMap, D8Map, flowMap, GMap, aspectMap, flowType, watershed, nPourPoints, watershed_directory, watershed_colour);
    handleOutput(flowMap, D8Map, aspectMap, GMap, output_file, image_file, input_file_type, colour_type, process, totalFlow);

    delete[] input_file;
    delete[] input_file_type;
    delete[] output_file;
    delete[] image_file;
    delete[] colour_type;
    delete[] process;

    return 0;
}