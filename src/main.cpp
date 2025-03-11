#include "argumentParser.h"
#include "Map.h"
#include "BMP.h"
#include "SobelAnalysis.h"
#include "D8FlowAnalyser.h"
#include "FlowAccumulation.h" 
#include "watershedAnalysis.h"

#include <iostream>
#include <sstream>
#include <cstring>


int main(int argc, char* argv[]) {
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
    bool verbose = false;
    char* process = nullptr;

    // Parse arguments
    if (!parseArguments(argc, argv, input_file, input_file_type, output_file, image_file, colour, colour_type, totalFlow, watershed, nPourPoints, verbose, process)) {
        return 1; // Exit if argument parsing fails
    }
    
    // Argument Checks

    // No inputs
    if (!input_file) {
        std::cerr << "Error: No -i / --input flag provided." << std::endl;
        // Memory deletion
        delete[] input_file;
        delete[] input_file_type;
        delete[] colour_type;
        delete[] process;
        return 1;
    }
    // No outputs
    if (!output_file && !image_file) {
        std::cerr << "Error: At least one of -o (output file) or -img (image file) must be specified." << std::endl;
        
        // Memory deletion
        delete[] input_file;
        delete[] input_file_type;
        delete[] colour_type;
        delete[] process;
        return 1;
    }

    // Colour and image check
    if (image_file && !colour) {
        std::cout << "No -c flag. Greyscale chosen." << std::endl;
        colour_type = new char[3];
        strcpy(colour_type, "g1");
    }

    // Total flow w/ process check
    if (totalFlow && !((strcmp(process, "mdf") == 0 || strcmp(process, "d8") == 0 || strcmp(process, "dinf") == 0 ))) {
            std::cerr << "Process " << process << " is not compatible with flow accumulation (-fa)." << std::endl;
            delete[] input_file;
            delete[] input_file_type;
            delete[] output_file;
            delete[] image_file;
            delete[] colour_type;
            delete[] process;
            return 1;
        }
    
    // Watershed / flow accumulation clash
    if (totalFlow && watershed) {
        std::cerr << "Watershed and flow accumulation are incompatible processes." << std::endl;
            delete[] input_file;
            delete[] input_file_type;
            delete[] output_file;
            delete[] image_file;
            delete[] colour_type;
            delete[] process;
            return 1;
    }

    // Verbose output
    if (verbose) {
        std::cout << "Verbose mode enabled." << std::endl;
        std::cout << "Input file: " << input_file << std::endl;
        std::cout << "Process: " << process << std::endl;
        if (output_file) {
            std::cout << "Output file: " << output_file << std::endl;
        }
        if (image_file) {
            std::cout << "Image file: " << image_file << std::endl;
            if (colour_type) {
                std::cout << "Colour: " << colour_type << std::endl;
            }
        }
    }

    
    /*
    Note to self: Finalise Verbose
    */

    Map<double> elevationMap;
    std::cout << "Opening map" << std::endl;
    if (!elevationMap.loadFromFile(input_file, input_file_type)) {
        std::cerr << "File: " << input_file << " does not exist." << std::endl;

        // Memory deletion
        delete[] input_file;
        delete[] input_file_type;
        delete[] colour_type;
        delete[] process;
        return 1;
    }
    std::cout << elevationMap.getData(0,0) << std::endl;
    std::cout << elevationMap.getHeight() << std::endl;
    std::cout << elevationMap.getWidth() << std::endl;
    elevationMap.fillSinks();
    
    

    // D8 Init
    Map<int> D8Map;
    Map<double> flowMap;
    std::string flowType;

    // Dinf Init
    Map<double> GMap;
    Map<double> aspectMap;


    // Run gradient / direction processes
    if (strcmp(process, "d8") == 0) {
        flowType = "d8";
        D8FlowAnalyser analyser(elevationMap);
        analyser.analyseFlow();
        D8Map = analyser.getMap();
    }
    else if (strcmp(process, "dinf") == 0){
        flowType = "dinf";
        SlopeAnalyser sAnalyser(elevationMap);
        GMap = sAnalyser.computeSlope("combined");
        aspectMap = sAnalyser.computeDirection();
    }
    else if (strcmp(process, "mdf") == 0){
        flowType = "mdf";
        SlopeAnalyser sAnalyser(elevationMap);
        GMap = sAnalyser.computeSlope("combined");
    }
    else if (strcmp(process, "slope") == 0) {
        SlopeAnalyser sAnalyser(elevationMap);
        GMap = sAnalyser.computeSlope("combined");
    }
    else {
        std::cerr << "Error: Unknown process: " << process << std::endl;
    }
    
    // Run flow processes
    if (totalFlow) {
        if (flowType == "d8") {
            
            FlowAccumulator<double, int, double> flowAccumulator(elevationMap, nullptr, nullptr, &D8Map);
            flowMap = flowAccumulator.accumulateFlow(flowType);
        }
        else if (flowType == "dinf") {
            FlowAccumulator<double, int, double> flowAccumulator(elevationMap, &aspectMap, &GMap, nullptr);
            flowMap = flowAccumulator.accumulateFlow(flowType);
        }
        else if (flowType == "mdf") {
            FlowAccumulator<double, int, double> flowAccumulator(elevationMap, nullptr, &GMap, nullptr);
            flowMap = flowAccumulator.accumulateFlow(flowType);
        }
        else {
            std::cerr << "Unrecognised process for watershed anaylsis." << std::endl;
        }
    }

    // Run watershed processes
    if (watershed) {
        if (flowType == "d8") {
            FlowAccumulator<double, int, double> flowAccumulator(elevationMap, nullptr, nullptr, &D8Map);
            flowMap = flowAccumulator.accumulateFlow(flowType);
            std::vector<std::pair<int, int>> pourPoints;
            watershedAnalysis<double, int> watershedAnalyser(elevationMap, &D8Map, &flowMap, nullptr, nullptr);
            pourPoints = watershedAnalyser.getPourPoints(nPourPoints, "d8");
            int i = 0;
            for (const auto& p : pourPoints) {
                std::cout << "Pour point: " << p.first << "," << p.second << std::endl;
                std::cout << D8Map.getData(p.first, p.second) << std::endl;
                Map<double> outputWatershed = watershedAnalyser.calculateWatershed(p, "d8");
                outputWatershed.applyScaling("log");
                std::ostringstream oss;
                oss << "../data/watershed_" << i << ".bmp";  // Format as "../data/watershed_i"
                std::string filename = oss.str();
                outputWatershed.exportToImage(filename, "g1");
                i++;
            }
        }
        else if (flowType == "dinf") {
            D8FlowAnalyser analyser(elevationMap);
            analyser.analyseFlow();
            D8Map = analyser.getMap();


            FlowAccumulator<double, int, double> flowAccumulator(elevationMap, &aspectMap, &GMap, nullptr);
            flowMap = flowAccumulator.accumulateFlow(flowType);

            std::vector<std::pair<int, int>> pourPoints;
            watershedAnalysis<double, int> watershedAnalyser(elevationMap, &D8Map, &flowMap, nullptr, nullptr);
            pourPoints = watershedAnalyser.getPourPoints(nPourPoints, "d8");

            watershedAnalysis<double, int> watershedAnalyserDINF(elevationMap, nullptr, &flowMap, &GMap, &aspectMap);

            int i = 0;
            for (const auto& p : pourPoints) {
                std::cout << "Pour point: " << p.first << "," << p.second << std::endl;
                Map<double> outputWatershed = watershedAnalyserDINF.calculateWatershed(p, "dinf");
                outputWatershed.applyScaling("log");
                std::ostringstream oss;
                oss << "../data/watershed_" << i << ".bmp";  // Format as "../data/watershed_i"
                std::string filename = oss.str();
                outputWatershed.exportToImage(filename, "sf");
                i++;
            }
        }
        else if (flowType == "mdf") {
            D8FlowAnalyser analyser(elevationMap);
            analyser.analyseFlow();
            D8Map = analyser.getMap();
            FlowAccumulator<double, int, double> flowAccumulator(elevationMap, nullptr, &GMap, nullptr);
            flowMap = flowAccumulator.accumulateFlow(flowType);
            std::vector<std::pair<int, int>> pourPoints;
            watershedAnalysis<double, int> watershedAnalyser(elevationMap, nullptr, &flowMap, nullptr, nullptr);
            pourPoints = watershedAnalyser.getPourPoints(nPourPoints, "mdf");

            int i = 0;
            for (const auto& p : pourPoints) {
                std::cout << "Pour point: " << p.first << "," << p.second << std::endl;
                std::cout << D8Map.getData(p.first, p.second) << std::endl;
                Map<double> outputWatershed = watershedAnalyser.calculateWatershed(p, "mdf");
                outputWatershed.applyScaling("log");
                std::ostringstream oss;
                oss << "../data/watershed_" << i << ".bmp";  // Format as "../data/watershed_i"
                std::string filename = oss.str();
                outputWatershed.exportToImage(filename, "g1");
                i++;
            }
        }
    }

    // Output handling
    if (totalFlow) {
        if (output_file) {
            flowMap.saveToFile(output_file, input_file_type);
            std::cout << "Saved ." << input_file_type << " file as " << output_file << std::endl;
        }
        if (image_file) {
            flowMap.applyScaling("log");
            flowMap.exportToImage(image_file, colour_type);
            std::cout << "Saved image file to: " << image_file << std::endl;
        }
    }
    else {
        if (strcmp(process, "d8") == 0) {
            if (output_file) {
                D8Map.saveToFile(output_file, input_file_type);
                std::cout << "Saved D8 flow map as ." << input_file_type << " file: " << output_file << std::endl;
            }
            if (image_file) {
                D8Map.exportToImage(image_file, colour_type);
                std::cout << "Saved D8 flow map image to: " << image_file << std::endl;
            }
        }
        else if (strcmp(process, "dinf") == 0) {
            if (output_file) {
                aspectMap.saveToFile(output_file, input_file_type);
                std::cout << "Saved D∞ aspect map as ." << input_file_type << " file: " << output_file << std::endl;
            }
            if (image_file) {
                aspectMap.exportToImage(image_file, colour_type);
                std::cout << "Saved D∞ aspect map image to: " << image_file << std::endl;
            }
        }
        else if (strcmp(process, "mdf") == 0) {
            std::cerr << "MDF process does not have output without Flow Accumulation (-fa). " << std::endl;
        }
        else if (strcmp(process, "slope") == 0) {
            if (image_file) {
                GMap.exportToImage(image_file, colour_type);
                std::cout << "Saved slope map image to: " << image_file << std::endl;
            }
        }
    }


    // Memory deletion
    delete[] input_file;
    delete[] input_file_type;
    delete[] output_file;
    delete[] image_file;
    delete[] colour_type;
    delete[] process;
    return 0;
}
