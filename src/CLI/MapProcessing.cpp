/**
 * @file MapProcessing.cpp
 * @author Ollie
 * @brief Functions for CLI to process DEMs with watershed, slope, aspect, and flow accumulation
 * @version 1.0.0
 * @date 2025-03-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "MapProcessing.h"
#include <sstream>

/**
 * @brief Pulls necessary maps for process types
 */
void processMap(Map<double>& elevationMap, char* process, Map<int>& D8Map, Map<double>& flowMap, Map<double>& GMap, Map<double>& aspectMap, std::string& flowType) {
    if (strcmp(process, "d8") == 0) {
        // Create D8 map for D8 analysis
        flowType = "d8";
        D8FlowAnalyser analyser(elevationMap);
        analyser.analyseFlow();
        D8Map = analyser.getMap();
    }
    else if (strcmp(process, "dinf") == 0){
        // Create slope and gradient maps for dinf analysis
        flowType = "dinf";
        SlopeAnalyser sAnalyser(elevationMap);
        GMap = sAnalyser.computeSlope("combined");
        aspectMap = sAnalyser.computeDirection();
    }
    else if (strcmp(process, "mdf") == 0){
        // Create gradient map for MDF analysis
        flowType = "mdf";
        SlopeAnalyser sAnalyser(elevationMap);
        GMap = sAnalyser.computeSlope("combined");
    }
    // Create slope map for slope
    else if (strcmp(process, "slope") == 0) {
        SlopeAnalyser sAnalyser(elevationMap);
        GMap = sAnalyser.computeSlope("combined");
    }
    // Create aspect map for aspect
    else if (strcmp(process, "aspect") == 0) {
        SlopeAnalyser sAnalyser(elevationMap);
        aspectMap = sAnalyser.computeDirection();
    }
    else {
        std::cerr << "Error: Unknown process: " << process << std::endl;
    }
}

/**
 * @brief run flow accumulation for process specified if flow accumulation selected (-fa)
 */
void handleFlowAccumulation(Map<double>& elevationMap, Map<int>& D8Map, Map<double>& flowMap, Map<double>& GMap, Map<double>& aspectMap, std::string& flowType, bool totalFlow) {
    // Given that total flow was selected
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
            std::cerr << "Unrecognised process for flow accumulation." << std::endl;
        }
    }
}

/**
 * @brief run watershed delineation for process if watershed was selected (-w)
 */
void handleWatershed(Map<double>& elevationMap, Map<int>& D8Map, Map<double>& flowMap,
    Map<double>& GMap, Map<double>& aspectMap, std::string& flowType, bool watershed,
    int nPourPoints, char* watershed_directory, char* watershed_colour) {
        // Given watershed was chosen
        if (watershed) {
        if (flowType == "d8") {
            // Run flow
            FlowAccumulator<double, int, double> flowAccumulator(elevationMap, nullptr, nullptr, &D8Map);
            flowMap = flowAccumulator.accumulateFlow(flowType);
            
            // Identify pour points
            std::vector<std::pair<int, int>> pourPoints;
            watershedAnalysis<double, int> watershedAnalyser(elevationMap, &D8Map, &flowMap, nullptr, nullptr);
            pourPoints = watershedAnalyser.getPourPoints(nPourPoints, "d8");
            
            // Iterate over pour points found
            int i = 0;
            for (const auto& p : pourPoints) {
                Map<double> outputWatershed = watershedAnalyser.calculateWatershed(p, "d8");
                outputWatershed.applyScaling("log");
                std::ostringstream oss;
                oss << watershed_directory << "watershed_" << i << ".bmp";  // Format as "../test/watershed_i"
                std::string filename = oss.str();
                ImageExport<double>::exportMapToImage(outputWatershed, filename, watershed_colour, true);
                i++;
            }
        }
        else if (flowType == "dinf") {
            // Run flow
            FlowAccumulator<double, int, double> flowAccumulator(elevationMap, &aspectMap, &GMap, nullptr);
            flowMap = flowAccumulator.accumulateFlow(flowType);

           // Identify pour points 
            std::vector<std::pair<int, int>> pourPoints;
            watershedAnalysis<double, int> watershedAnalyser(elevationMap, nullptr, &flowMap, &GMap, &aspectMap);
            pourPoints = watershedAnalyser.getPourPoints(nPourPoints, "dinf");
            
            // iterate over pour points
            int i = 0;
            for (const auto& p : pourPoints) {
                Map<double> outputWatershed = watershedAnalyser.calculateWatershed(p, "dinf");
                
                outputWatershed.applyScaling("log");
                std::ostringstream oss;
                oss << watershed_directory << "watershed_" << i << ".bmp";  // Format as "../test/watershed_i"
                std::string filename = oss.str();
                ImageExport<double>::exportMapToImage(outputWatershed, filename, watershed_colour, true);
                i++;
            }

        }
        else if (flowType == "mdf") {
            // Run flow
            FlowAccumulator<double, int, double> flowAccumulator(elevationMap, nullptr, &GMap, nullptr);
            flowMap = flowAccumulator.accumulateFlow(flowType);

            // Identify pour points
            std::vector<std::pair<int, int>> pourPoints;
            watershedAnalysis<double, int> watershedAnalyser(elevationMap, nullptr, &flowMap, nullptr, nullptr);
            pourPoints = watershedAnalyser.getPourPoints(nPourPoints, "mdf");

            // iterate over pour points
            int i = 0;
            for (const auto& p : pourPoints) {
                Map<double> outputWatershed = watershedAnalyser.calculateWatershed(p, "mdf");
                outputWatershed.applyScaling("log");
                std::ostringstream oss;
                oss << watershed_directory << "watershed_" << i << ".bmp";  // Format as "../test/watershed_i"
                std::string filename = oss.str();
                ImageExport<double>::exportMapToImage(outputWatershed, filename, watershed_colour, true);
                i++;
            }
        }
    }
}

/**
 * @brief Handle outputs for images and files if selected
 */
void handleOutput(Map<double>& flowMap, Map<int>& D8Map, Map<double>& aspectMap, Map<double>& GMap,
    char* output_file, char* image_file, char* input_file_type, char* colour_type, char* process,
    bool totalFlow, bool watershed) {
    // Flow accumulation out
    if (totalFlow) {
        if (output_file) {
            flowMap.saveToFile(output_file, input_file_type);
            std::cout << "Saved ." << input_file_type << " file as " << output_file << std::endl;
        }
        if (image_file) {
            flowMap.applyScaling("log");
            ImageExport<double>::exportMapToImage(flowMap, image_file, colour_type, true);
            std::cout << "Saved image file to: " << image_file << std::endl;
        }
    }
    // Regular map types out
    else {
        if (strcmp(process, "d8") == 0) {
            if (output_file) {
                D8Map.saveToFile(output_file, input_file_type);
                std::cout << "Saved D8 flow map as ." << input_file_type << " file: " << output_file << std::endl;
            }
            if (image_file) {
                ImageExport<int>::exportMapToImage(D8Map, image_file, colour_type, true);
                std::cout << "Saved D8 flow map image to: " << image_file << std::endl;
            }
        }
        else if (strcmp(process, "dinf") == 0) {
            if (output_file) {
                aspectMap.saveToFile(output_file, input_file_type);
                std::cout << "Saved D∞ aspect map as ." << input_file_type << " file: " << output_file << std::endl;
            }
            if (image_file) {
                ImageExport<double>::exportMapToImage(aspectMap, image_file, colour_type, true);
                std::cout << "Saved D∞ aspect map image to: " << image_file << std::endl;
            }
        }
        else if (strcmp(process, "mdf") == 0 && !(watershed)) {
            std::cerr << "MDF process does not have output without Flow Accumulation (-fa). " << std::endl;
        }
        else if (strcmp(process, "slope") == 0) {
            if (image_file) {
                ImageExport<double>::exportMapToImage(GMap, image_file, colour_type, true);
                std::cout << "Saved slope map image to: " << image_file << std::endl;
            }
        }
        else if (strcmp(process, "aspect") == 0) {
            if (image_file) {
                ImageExport<double>::exportMapToImage(aspectMap, image_file, colour_type, true);
                std::cout << "Saved aspect map image to: " << image_file << std::endl;
            }
        }
    }
}