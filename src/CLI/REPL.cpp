/**
 * @file REPL.cpp
 * @author Ollie
 * @brief Loop function for REPL UI
 * @version 1.0.0
 * @date 2025-03-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #include "REPL.h"

void runREPL() {
    Map<double>* elevationMap = nullptr;
    Map<int>* D8Map = nullptr;
    Map<double>* flowMap = nullptr;
    Map<double>* gradientMap = nullptr;
    Map<double>* aspectMap = nullptr;

    std::cout << "Welcome to the DEM Processor REPL. Type 'help' for a list of commands.\n";

    while (true) {
        std::cout << "> ";
        char command[256];
        std::cin.getline(command, 256);

        if (strlen(command) == 0) continue;
        
        char cmd[50];
        sscanf(command, "%s", cmd);
        
        if (strcmp(cmd, "load") == 0) {
            loadFile(elevationMap, command);
        }
        else if (strcmp(cmd, "process") == 0) {
            processData(elevationMap, D8Map, flowMap, gradientMap, aspectMap, command);
        }
        else if (strcmp(cmd, "save") == 0) {
            saveData(flowMap, D8Map, aspectMap, gradientMap, command);
        }
        else if (strcmp(cmd, "export") == 0) {
            exportData(flowMap, D8Map, aspectMap, gradientMap, command);
        }
        else if (strcmp(cmd, "help") == 0) {
            displayHelp();
        }
        else if (strcmp(cmd, "quit") == 0) {
            quitProgram(elevationMap, D8Map, flowMap, gradientMap, aspectMap);
            break;
        }
        else {
            std::cerr << "Error: Unknown command. Type 'help' for a list of commands.\n";
        }
    }
}

bool loadFile(Map<double>*& elevationMap, const char* command) {
    char inputFile[128];
    if (sscanf(command, "%*s %127s", inputFile) != 1) {
        std::cerr << "Error: Usage - load <input_file>\n";
        return false;
    }

    std::string inputFileType = getFileExtension(inputFile);
    if (inputFileType.empty()) {
        std::cerr << "Error: The input file does not have a valid extension.\n";
        return false;
    }

    if (elevationMap) {
        delete elevationMap;
        elevationMap = nullptr;
    }

    elevationMap = new Map<double>();
    if (!elevationMap->loadFromFile(inputFile, inputFileType.c_str())) {
        std::cerr << "Error: Failed to load file: " << inputFile << "\n";
        delete elevationMap;
        elevationMap = nullptr;
        return false;
    }

    std::cout << "File loaded successfully.\n";
    return true;
}

void processData(Map<double>* elevationMap, Map<int>*& D8Map, Map<double>*& flowMap, Map<double>*& gradientMap, Map<double>*& aspectMap, const char* command) {
    char processType[10];
    if (sscanf(command, "%*s %9s", processType) != 1) {
        std::cerr << "Error: Usage - process <process_type>\n";
        return;
    }

    if (!elevationMap) {
        std::cerr << "Error: No file loaded. Use 'load' first.\n";
        return;
    }

    if (strcmp(processType, "d8") == 0) {
        D8FlowAnalyser analyser(*elevationMap);
        analyser.analyseFlow();
        if (D8Map) delete D8Map;
        D8Map = new Map<int>(analyser.getMap());
        std::cout << "D8 flow analysis completed.\n";
    }
    else if (strcmp(processType, "aspect") == 0) {
        SlopeAnalyser sAnalyser(*elevationMap);
        if (aspectMap) delete aspectMap;
        aspectMap = new Map<double>(sAnalyser.computeDirection());
        std::cout << "Aspect analysis completed.\n";
    }
    else if (strcmp(processType, "slope") == 0) {
        SlopeAnalyser sAnalyser(*elevationMap);
        if (gradientMap) delete gradientMap;
        gradientMap = new Map<double>(sAnalyser.computeSlope("combined"));
        std::cout << "Slope analysis completed.\n";
    }
    else if (strcmp(processType, "d8_flow") == 0) {
        D8FlowAnalyser analyser(*elevationMap);
        analyser.analyseFlow();
        if (D8Map) delete D8Map;
        D8Map = new Map<int>(analyser.getMap());

        FlowAccumulator<double, int, double> flowAccumulator(*elevationMap, nullptr, nullptr, D8Map);
        if (flowMap) delete flowMap;
        flowMap = new Map<double>(flowAccumulator.accumulateFlow("d8"));
        std::cout << "D8 Flow accumulation completed.\n";
    }
    else if (strcmp(processType, "dinf_flow") == 0) {
        SlopeAnalyser sAnalyser(*elevationMap);
        if (gradientMap) delete gradientMap;
        gradientMap = new Map<double>(sAnalyser.computeSlope("combined"));

        if (aspectMap) delete aspectMap;
        aspectMap = new Map<double>(sAnalyser.computeDirection());

        FlowAccumulator<double, int, double> flowAccumulator(*elevationMap, aspectMap, gradientMap, nullptr);
        if (flowMap) delete flowMap;
        flowMap = new Map<double>(flowAccumulator.accumulateFlow("dinf"));
        std::cout << "Dinf Flow accumulation completed.\n";
    }
    else if (strcmp(processType, "mdf_flow") == 0) {
        SlopeAnalyser sAnalyser(*elevationMap);
        if (gradientMap) delete gradientMap;
        gradientMap = new Map<double>(sAnalyser.computeSlope("combined"));

        FlowAccumulator<double, int, double> flowAccumulator(*elevationMap, nullptr, gradientMap, nullptr);
        if (flowMap) delete flowMap;
        flowMap = new Map<double>(flowAccumulator.accumulateFlow("mdf"));
        std::cout << "MDF Flow accumulation completed.\n";
    }
    else if (strcmp(processType, "watershed") == 0) {
        handleWatershedAnalysis(elevationMap, D8Map, flowMap, gradientMap, aspectMap);
    }
    else {
        std::cerr << "Error: Unknown process type: " << processType << "\n";
    }
}

void saveData(Map<double>* flowMap, Map<int>* D8Map, Map<double>* aspectMap, Map<double>* gradientMap, const char* command) {
    char outputFile[128];
    if (sscanf(command, "%*s %127s", outputFile) != 1) {
        std::cerr << "Error: Usage - save <output_file>\n";
        return;
    }

    std::string outputFileType = getFileExtension(outputFile);
    if (outputFileType.empty()) {
        std::cerr << "Error: The output file does not have a valid extension.\n";
        return;
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
    else if (gradientMap) {
        gradientMap->saveToFile(outputFile, outputFileType);
        std::cout << "Gradient map saved to " << outputFile << "\n";
    }
    else {
        std::cerr << "Error: No processed data to save.\n";
    }
}

void exportData(Map<double>* flowMap, Map<int>* D8Map, Map<double>* aspectMap, Map<double>* gradientMap, const char* command) {
    char imageFile[128] = "";
    char colourType[10] = "";
    
    if (sscanf(command, "%*s %127s %9s", imageFile, colourType) < 1) {
        std::cerr << "Error: Usage - export <image_file> [colour_type]\n";
        return;
    }
    
    if (strlen(colourType) == 0) {
        strcpy(colourType, "g1");
    }
    
    if (flowMap) {
        flowMap->applyScaling("log");
        ImageExport<double>::exportMapToImage(*flowMap, imageFile, colourType, true);
        std::cout << "Flow map exported to " << imageFile << "\n";
    }
    else if (D8Map) {
        ImageExport<int>::exportMapToImage(*D8Map, imageFile, colourType, true);
        std::cout << "D8 map exported to " << imageFile << "\n";
    }
    else if (aspectMap) {
        ImageExport<double>::exportMapToImage(*aspectMap, imageFile, colourType, true);
        std::cout << "Aspect map exported to " << imageFile << "\n";
    }
    else if (gradientMap) {
        gradientMap->applyScaling("log");
        ImageExport<double>::exportMapToImage(*gradientMap, imageFile, colourType, true);
        std::cout << "Gradient map exported to " << imageFile << "\n";
    }
    else {
        std::cerr << "Error: No processed data to export.\n";
    }
}

void displayHelp() {
    std::cout << "Commands:\n"
              << "  load <input_file> - Load a DEM file.\n"
              << "  process <process_type> - Run a process (e.g., d8, slope, aspect).\n"
              << "  save <output_file>  - Save processed data to a file.\n"
              << "  export <image_file> [colour_type] - Export processed data to an image.\n"
              << "  quit - Exit the program.\n";
}

void quitProgram(Map<double>*& elevationMap, Map<int>*& D8Map, Map<double>*& flowMap, Map<double>*& gradientMap, Map<double>*& aspectMap) {
    delete elevationMap;
    delete D8Map;
    delete flowMap;
    delete gradientMap;
    delete aspectMap;
    std::cout << "Exiting...\n";
}

void handleWatershedAnalysis(Map<double>* elevationMap, Map<int>*& D8Map, Map<double>*& flowMap, Map<double>*& gradientMap, Map<double>*& aspectMap) {
    std::cout << "Entering watershed mode" << std::endl;

    char process[10], type[10];
    std::cout << "Enter name of process to be used: ";
    std::cin.getline(process, 10);
    sscanf(process, "%s", type);
    if (strcmp(type, "mdf") != 0 && strcmp(type, "d8") != 0 && strcmp(type, "dinf") != 0) {
        std::cerr << "Invalid process. Exiting watershed mode.\n";
        return;
    }

    int nPourPoints;
    std::cout << "Enter number of pour points: ";
    std::cin >> nPourPoints;
    if (nPourPoints <= 0) {
        std::cerr << "Invalid number of pour points. Exiting watershed mode.\n";
        return;
    }

    std::string outputDir;
    std::cout << "Enter directory to store watershed images: ";
    std::cin >> outputDir;

    std::cout << "Using directory: " << outputDir << std::endl;
    
    std::string colourmap;
    std::cout << "Enter colourmap for watershed images: ";
    std::cin >> colourmap;

    std::cout << "Using directory: " << outputDir << std::endl;

    if (strcmp(type, "d8") == 0) {
        D8FlowAnalyser analyser(*elevationMap);
        analyser.analyseFlow();
        if (D8Map) delete D8Map;
        D8Map = new Map<int>(analyser.getMap());

        FlowAccumulator<double, int, double> flowAccumulator(*elevationMap, nullptr, nullptr, D8Map);
        if (flowMap) delete flowMap;
        flowMap = new Map<double>(flowAccumulator.accumulateFlow("d8"));

        std::vector<std::pair<int, int>> pourPoints;
        watershedAnalysis<double, int> watershedAnalyser(*elevationMap, D8Map, flowMap, nullptr, nullptr);
        pourPoints = watershedAnalyser.getPourPoints(nPourPoints, "d8");

        int i = 0;
        for (const auto& p : pourPoints) {
            Map<double> outputWatershed = watershedAnalyser.calculateWatershed(p, "d8");
            outputWatershed.applyScaling("log");

            std::ostringstream oss;
            oss << outputDir << "/watershed_" << i << ".bmp";
            std::string filename = oss.str();

            ImageExport<double>::exportMapToImage(outputWatershed, filename, colourmap, true);
            i++;
        }
        std::cout << "Exported watershed images to: " << outputDir << std::endl;
    }
    else if (strcmp(type, "dinf") == 0) {
        D8FlowAnalyser analyser(*elevationMap);
        analyser.analyseFlow();
        if (D8Map) delete D8Map;
        D8Map = new Map<int>(analyser.getMap());

        SlopeAnalyser sAnalyser(*elevationMap);
        if (gradientMap) delete gradientMap;
        gradientMap = new Map<double>(sAnalyser.computeSlope("combined"));

        if (aspectMap) delete aspectMap;
        aspectMap = new Map<double>(sAnalyser.computeDirection());

        FlowAccumulator<double, int, double> flowAccumulator(*elevationMap, aspectMap, gradientMap, nullptr);
        if (flowMap) delete flowMap;
        flowMap = new Map<double>(flowAccumulator.accumulateFlow("dinf"));

        std::vector<std::pair<int, int>> pourPoints;
        watershedAnalysis<double, int> watershedAnalyser(*elevationMap, D8Map, flowMap, nullptr, nullptr);
        pourPoints = watershedAnalyser.getPourPoints(nPourPoints, "d8");

        int i = 0;
        for (const auto& p : pourPoints) {
            Map<double> outputWatershed = watershedAnalyser.calculateWatershed(p, "dinf");
            outputWatershed.applyScaling("log");

            std::ostringstream oss;
            oss << outputDir << "/watershed_" << i << ".bmp";
            std::string filename = oss.str();

            ImageExport<double>::exportMapToImage(outputWatershed, filename, colourmap, true);
            i++;
        }
        std::cout << "Exported watershed images to: " << outputDir << std::endl;
    }
    else if (strcmp(type, "mdf") == 0) {
        SlopeAnalyser sAnalyser(*elevationMap);
        if (gradientMap) delete gradientMap;
        gradientMap = new Map<double>(sAnalyser.computeSlope("combined"));

        FlowAccumulator<double, int, double> flowAccumulator(*elevationMap, nullptr, gradientMap, nullptr);
        if (flowMap) delete flowMap;
        flowMap = new Map<double>(flowAccumulator.accumulateFlow("mdf"));

        std::vector<std::pair<int, int>> pourPoints;
        watershedAnalysis<double, int> watershedAnalyser(*elevationMap, nullptr, flowMap, nullptr, nullptr);
        pourPoints = watershedAnalyser.getPourPoints(nPourPoints, "mdf");

        int i = 0;
        for (const auto& p : pourPoints) {
            Map<double> outputWatershed = watershedAnalyser.calculateWatershed(p, "mdf");
            outputWatershed.applyScaling("log");

            std::ostringstream oss;
            oss << outputDir << "/watershed_" << i << ".bmp";
            std::string filename = oss.str();

            ImageExport<double>::exportMapToImage(outputWatershed, filename, colourmap, true);
            i++;
        }
        std::cout << "Exported watershed images to: " << outputDir << std::endl;
    }
    else {
        std::cerr << "Invalid flow type specified for watershed analysis. Exiting watershed mode.\n";
    }
}