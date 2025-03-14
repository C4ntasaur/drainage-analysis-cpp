#include "MapProcessing.h"
#include <sstream>

void processMap(Map<double>& elevationMap, char* process, Map<int>& D8Map, Map<double>& flowMap, Map<double>& GMap, Map<double>& aspectMap, std::string& flowType) {
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
    else if (strcmp(process, "aspect") == 0) {
        SlopeAnalyser sAnalyser(elevationMap);
        aspectMap = sAnalyser.computeDirection();
    }
    else {
        std::cerr << "Error: Unknown process: " << process << std::endl;
    }
}

void handleFlowAccumulation(Map<double>& elevationMap, Map<int>& D8Map, Map<double>& flowMap, Map<double>& GMap, Map<double>& aspectMap, std::string& flowType, bool totalFlow) {
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

void handleWatershed(Map<double>& elevationMap, Map<int>& D8Map, Map<double>& flowMap, Map<double>& GMap, Map<double>& aspectMap, std::string& flowType, bool watershed, int nPourPoints, char* watershed_directory, char* watershed_colour) {
    if (watershed) {
        if (flowType == "d8") {
            FlowAccumulator<double, int, double> flowAccumulator(elevationMap, nullptr, nullptr, &D8Map);
            flowMap = flowAccumulator.accumulateFlow(flowType);
            std::vector<std::pair<int, int>> pourPoints;
            watershedAnalysis<double, int> watershedAnalyser(elevationMap, &D8Map, &flowMap, nullptr, nullptr);
            pourPoints = watershedAnalyser.getPourPoints(nPourPoints, "d8");
            int i = 0;
            for (const auto& p : pourPoints) {
                std::cout << D8Map.getData(p.first, p.second) << std::endl;
                Map<double> outputWatershed = watershedAnalyser.calculateWatershed(p, "d8");
                outputWatershed.applyScaling("log");
                std::ostringstream oss;
                oss << watershed_directory << "_" << i << ".bmp";  // Format as "../test/watershed_i"
                std::string filename = oss.str();
                ImageExport<double>::exportMapToImage(outputWatershed, filename, watershed_colour, true);
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
                Map<double> outputWatershed = watershedAnalyserDINF.calculateWatershed(p, "dinf");
                outputWatershed.applyScaling("log");
                std::ostringstream oss;
                oss << "../test/watershed_" << i << ".bmp";  // Format as "../test/watershed_i"
                std::string filename = oss.str();
                ImageExport<double>::exportMapToImage(outputWatershed, filename, "g1", true);
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
                Map<double> outputWatershed = watershedAnalyser.calculateWatershed(p, "mdf");
                outputWatershed.applyScaling("log");
                std::ostringstream oss;
                oss << "../test/watershed_" << i << ".bmp";  // Format as "../test/watershed_i"
                std::string filename = oss.str();
                ImageExport<double>::exportMapToImage(outputWatershed, filename, "g1", true);
                i++;
            }
        }
    }
}

void handleOutput(Map<double>& flowMap, Map<int>& D8Map, Map<double>& aspectMap, Map<double>& GMap, char* output_file, char* image_file, char* input_file_type, char* colour_type, char* process, bool totalFlow) {
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
        else if (strcmp(process, "mdf") == 0) {
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