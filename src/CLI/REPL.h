#include <iostream>
#include <sstream>
#include "../map_core/Map.h"
#include "../map_core/Map.h"
#include "../image_handling/BMP.h"
#include "../DEM_analysis/SobelAnalysis.h"
#include "../DEM_analysis/D8FlowAnalyser.h"
#include "../DEM_analysis/FlowAccumulation.h" 
#include "../DEM_analysis/watershedAnalysis.h"
#include "../image_handling/ImageExport.h"

void runREPL() {
    Map<double>* elevationMap = nullptr;
    Map<int>* D8Map = nullptr;
    Map<double>* flowMap = nullptr;
    Map<double>* gradientMap = nullptr;
    Map<double>* aspectMap = nullptr;
    std::string flowType;
    bool verbose = false;

    std::cout << "Welcome to the DEM Processor REPL. Type 'help' for a list of commands.\n";

    while (true) {
        std::cout << "> ";
        std::string command;
        std::getline(std::cin, command);

        if (command.empty()) continue;

        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;
        if (cmd == "load") {
            std::string inputFile, inputFileType;
            iss >> inputFile >> inputFileType;

            if (inputFile.empty() || inputFileType.empty()) {
                std::cerr << "Error: Usage - load <input_file> <input_file_type>\n";
                continue;
            }

            // Free existing elevation map if it exists
            if (elevationMap) {
                delete elevationMap;
                elevationMap = nullptr;
            }

            elevationMap = new Map<double>();
            if (!elevationMap->loadFromFile(inputFile, inputFileType)) {
                std::cerr << "Error: Failed to load file: " << inputFile << "\n";
                delete elevationMap;
                elevationMap = nullptr;
            } else {
                std::cout << "File loaded successfully.\n";
            }
        }
        else if (cmd == "process") {
            std::string processType;
            iss >> processType;

            if (!elevationMap) {
                std::cerr << "Error: No file loaded. Use 'load' first.\n";
                continue;
            }

            if (processType == "d8") {
                flowType = "d8";
                D8FlowAnalyser analyser(*elevationMap);
                analyser.analyseFlow();
                if (D8Map) delete D8Map; // Free existing D8 map
                D8Map = new Map<int>(analyser.getMap());
                std::cout << "D8 flow analysis completed.\n";
            }
            else if (processType == "dinf") {
                flowType = "dinf";
                SlopeAnalyser sAnalyser(*elevationMap);
                if (gradientMap) delete gradientMap; // Free existing G map
                gradientMap = new Map<double>(sAnalyser.computeSlope("combined"));
                if (aspectMap) delete aspectMap; // Free existing aspect map
                aspectMap = new Map<double>(sAnalyser.computeDirection());
                std::cout << "D∞ slope and aspect analysis completed.\n";
            }
            else if (processType == "mdf") {
                flowType = "mdf";
                SlopeAnalyser sAnalyser(*elevationMap);
                if (gradientMap) delete gradientMap; // Free existing G map
                gradientMap = new Map<double>(sAnalyser.computeSlope("combined"));
                std::cout << "MDF slope analysis completed.\n";
            }
            else if (processType == "slope") {
                SlopeAnalyser sAnalyser(*elevationMap);
                if (gradientMap) delete gradientMap; // Free existing G map
                gradientMap = new Map<double>(sAnalyser.computeSlope("combined"));
                std::cout << "Slope analysis completed.\n";
            }
            else if (processType == "aspect") {
                SlopeAnalyser sAnalyser(*elevationMap);
                if (aspectMap) delete aspectMap; // Free existing aspect map
                aspectMap = new Map<double>(sAnalyser.computeDirection());
                std::cout << "Aspect analysis completed.\n";
            }
            else {
                std::cerr << "Error: Unknown process type: " << processType << "\n";
            }
        }
        else if (cmd == "save") {
            std::string outputFile, outputFileType;
            iss >> outputFile >> outputFileType;

            if (outputFile.empty() || outputFileType.empty()) {
                std::cerr << "Error: Usage - save <output_file> <output_file_type>\n";
                continue;
            }

            if (flowMap) {
                flowMap->saveToFile(outputFile, outputFileType);
                std::cout << "Flow map saved to " << outputFile << "\n";
            }
            else if (D8Map) {
                D8Map->saveToFile(outputFile, outputFileType);
                std::cout << "D8 map saved to " << outputFile << "\n";
            }
            else if (aspectMap) {
                aspectMap->saveToFile(outputFile, outputFileType);
                std::cout << "Aspect map saved to " << outputFile << "\n";
            }
            else {
                std::cerr << "Error: No processed data to save.\n";
            }
        }
        else if (cmd == "export") {
            std::string imageFile, colourType;
            iss >> imageFile >> colourType;

            if (imageFile.empty()) {
                std::cerr << "Error: Usage - export <image_file> [colour_type]\n";
                continue;
            }

            if (flowMap) {
                flowMap->applyScaling("log");
                ImageExport<double>::exportMapToImage(*flowMap, imageFile, colourType.empty() ? "g1" : colourType, true);
                std::cout << "Flow map exported to " << imageFile << "\n";
            }
            else if (D8Map) {
                ImageExport<int>::exportMapToImage(*D8Map, imageFile, colourType.empty() ? "g1" : colourType, true);
                std::cout << "D8 map exported to " << imageFile << "\n";
            }
            else if (aspectMap) {
                ImageExport<double>::exportMapToImage(*aspectMap, imageFile, colourType.empty() ? "g1" : colourType, true);
                std::cout << "Aspect map exported to " << imageFile << "\n";
            }
            else if (gradientMap) {
                gradientMap->applyScaling("log");
                ImageExport<double>::exportMapToImage(*gradientMap, imageFile, colourType.empty() ? "g1" : colourType, true);
                std::cout << "Gradient map exported to " << imageFile << "\n";
            }
            else {
                std::cerr << "Error: No processed data to export.\n";
            }
        }
        else if (cmd == "verbose") {
            verbose = !verbose;
            std::cout << "Verbose mode " << (verbose ? "enabled" : "disabled") << ".\n";
        }
        else if (cmd == "help") {
            std::cout << "Commands:\n"
                      << "  load <input_file> <input_file_type> - Load a DEM file.\n"
                      << "  process <process_type> - Run a process (e.g., d8, dinf, mdf, slope, aspect).\n"
                      << "  save <output_file> <output_file_type> - Save processed data to a file.\n"
                      << "  export <image_file> [colour_type] - Export processed data to an image.\n"
                      << "  verbose - Toggle verbose output.\n"
                      << "  quit - Exit the program.\n";
        }
        else if (cmd == "quit") {
            // Clean up dynamically allocated memory
            if (elevationMap) delete elevationMap;
            if (D8Map) delete D8Map;
            if (flowMap) delete flowMap;
            if (gradientMap) delete gradientMap;
            if (aspectMap) delete aspectMap;
            std::cout << "Exiting...\n";
            break;
        }
        else {
            std::cerr << "Error: Unknown command. Type 'help' for a list of commands.\n";
        }
    }
}