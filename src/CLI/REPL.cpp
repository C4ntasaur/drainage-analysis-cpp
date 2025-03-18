/**
 * @file REPL.cpp
 * @author Ollie
 * @brief Loop function for REPL UI
 * @version 1.0.1
 * @date 2025-03-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #include "REPL.h"

 /**
  * @brief Main loop for REPL UI
  */
void runREPL() {
    // Permanent containers
    Map<double>* elevationMap = nullptr;
    Map<int>* D8Map = nullptr;
    Map<double>* flowMap = nullptr;
    Map<double>* gradientMap = nullptr;
    Map<double>* aspectMap = nullptr;

    std::cout << "Welcome to the DEM Processor REPL. Type 'help' for a list of commands.\n";

    while (true) {
        // Pretty line thing
        std::cout << "> ";
        char command[256];
        std::cin.getline(command, 256);

        // Return on empty line
        if (strlen(command) == 0) continue;
        
        char cmd[50];
        sscanf(command, "%s", cmd);
        
        // If else checks for valid operators
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

 /**
  * @brief Load DEM file to container
  */
bool loadFile(Map<double>*& elevationMap, const char* command) {
    // Scan in file pathway
    char inputFile[128];
    if (sscanf(command, "%*s %127s", inputFile) != 1) {
        std::cerr << "Error: Usage - load <input_file>\n";
        return false;
    }

    // Get extension
    std::string inputFileType = getFileExtension(inputFile);
    if (inputFileType.empty()) {
        std::cerr << "Error: The input file does not have a valid extension.\n";
        return false;
    }

    // Free up allocated mem
    if (elevationMap) {
        delete elevationMap;
        elevationMap = nullptr;
    }
    elevationMap = new Map<double>();

    // Load file
    if (!elevationMap->loadFromFile(inputFile, inputFileType.c_str())) {
        // Failure
        std::cerr << "Error: Failed to load file: " << inputFile << "\n";
        delete elevationMap;
        elevationMap = nullptr;
        return false;
    }

    std::cout << "File loaded successfully.\n";
    return true;
}

 /**
  * @brief Run process operator keywords on loaded DEM
  */
void processData(Map<double>* elevationMap, Map<int>*& D8Map, Map<double>*& flowMap, Map<double>*& gradientMap, Map<double>*& aspectMap, const char* command) {
    // Grab process keyword
    char processType[10];
    if (sscanf(command, "%*s %9s", processType) != 1) {
        std::cerr << "Error: Usage - process <process_type>\n";
        return;
    }

    // Check DEM is loaded first
    if (!elevationMap) {
        std::cerr << "Error: No file loaded. Use 'load' first.\n";
        return;
    }

    // If else for process type
    if (strcmp(processType, "d8") == 0) {
        // Finds D8 direction map
        D8FlowAnalyser analyser(*elevationMap);
        analyser.analyseFlow();
        if (D8Map) delete D8Map;
        D8Map = new Map<int>(analyser.getMap());
        std::cout << "D8 flow analysis completed.\n";
    }
    else if (strcmp(processType, "aspect") == 0) {
        // Finds aspect map
        SlopeAnalyser sAnalyser(*elevationMap);
        if (aspectMap) delete aspectMap;
        aspectMap = new Map<double>(sAnalyser.computeDirection());
        std::cout << "Aspect analysis completed.\n";
    }
    else if (strcmp(processType, "slope") == 0) {
        // Finds gradient map
        SlopeAnalyser sAnalyser(*elevationMap);
        if (gradientMap) delete gradientMap;
        gradientMap = new Map<double>(sAnalyser.computeSlope("combined"));
        std::cout << "Slope analysis completed.\n";
    }
    else if (strcmp(processType, "d8_flow") == 0) {
        // Find D8 direction map
        D8FlowAnalyser analyser(*elevationMap);
        analyser.analyseFlow();
        if (D8Map) delete D8Map;
        D8Map = new Map<int>(analyser.getMap());

        // Run flow accumulation
        FlowAccumulator<double, int, double> flowAccumulator(*elevationMap, nullptr, nullptr, D8Map);
        if (flowMap) delete flowMap;
        flowMap = new Map<double>(flowAccumulator.accumulateFlow("d8"));
        std::cout << "D8 Flow accumulation completed.\n";
    }
    else if (strcmp(processType, "dinf_flow") == 0) {
        // Finds Dinf flow accumulation map

        // Find slope and gradient maps
        SlopeAnalyser sAnalyser(*elevationMap);
        if (gradientMap) delete gradientMap;
        gradientMap = new Map<double>(sAnalyser.computeSlope("combined"));

        if (aspectMap) delete aspectMap;
        aspectMap = new Map<double>(sAnalyser.computeDirection());

        // Run flow accumulation
        FlowAccumulator<double, int, double> flowAccumulator(*elevationMap, aspectMap, gradientMap, nullptr);
        if (flowMap) delete flowMap;
        flowMap = new Map<double>(flowAccumulator.accumulateFlow("dinf"));
        std::cout << "Dinf Flow accumulation completed.\n";
    }
    else if (strcmp(processType, "mdf_flow") == 0) {
        // Finds MDF flow accumulation map

        // Find gradient map
        SlopeAnalyser sAnalyser(*elevationMap);
        if (gradientMap) delete gradientMap;
        gradientMap = new Map<double>(sAnalyser.computeSlope("combined"));

        // Run flow accumulation
        FlowAccumulator<double, int, double> flowAccumulator(*elevationMap, nullptr, gradientMap, nullptr);
        if (flowMap) delete flowMap;
        flowMap = new Map<double>(flowAccumulator.accumulateFlow("mdf"));
        std::cout << "MDF Flow accumulation completed.\n";
    }
    else if (strcmp(processType, "watershed") == 0) {
        // Run watershed delineation
        handleWatershedAnalysis(elevationMap, D8Map, flowMap, gradientMap, aspectMap);
    }
    else {
        // Failure
        std::cerr << "Error: Unknown process type: " << processType << "\n";
    }
}

 /**
  * @brief Save processed maps
  */
void saveData(Map<double>* flowMap, Map<int>* D8Map, Map<double>* aspectMap, Map<double>* gradientMap, const char* command) {
    // Check file was provided to save to
    char outputFile[128];
    if (sscanf(command, "%*s %127s", outputFile) != 1) {
        std::cerr << "Error: Usage - save <output_file>\n";
        return;
    }

    // Find file extension
    std::string outputFileType = getFileExtension(outputFile);
    if (outputFileType.empty()) {
        std::cerr << "Error: The output file does not have a valid extension.\n";
        return;
    }

    // if else for maps to save out
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

 /**
  * @brief Export maps as image
  */
void exportData(Map<double>* flowMap, Map<int>* D8Map, Map<double>* aspectMap, Map<double>* gradientMap, const char* command) {
    // Containers
    char imageFile[128] = "";
    char colourType[10] = "";

    // Check image file pathway and colour was specified
    if (sscanf(command, "%*s %127s %9s", imageFile, colourType) < 1) {
        std::cerr << "Error: Usage - export <image_file> [colour_type]\n";
        return;
    }
    
    // if no colour given, use greyscale as default
    if (strlen(colourType) == 0) {
        strcpy(colourType, "g1");
        std::cout << "No colour specified. Using 'g1' (greyscale) as default." << std::endl;
    }
    
    // If else for processes to save as image
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
        // Failure
        std::cerr << "Error: No processed data to export.\n";
    }
}

 /**
  * @brief Print help
  */
void displayHelp() {
    std::cout << "Commands:\n"
              << "  load <input_file> - Load a DEM file.\n"
              << "  process <process_type> - Run a process (e.g., d8, slope, aspect).\n"
              << "  save <output_file>  - Save processed data to a file.\n"
              << "  export <image_file> [colour_type] - Export processed data to an image.\n"
              << "  quit - Exit the program.\n";
}

 /**
  * @brief Exit program and free memory
  */
void quitProgram(Map<double>*& elevationMap, Map<int>*& D8Map, Map<double>*& flowMap, Map<double>*& gradientMap, Map<double>*& aspectMap) {
    std::cout << "Exiting..." << std::endl;
    delete elevationMap;
    delete D8Map;
    delete flowMap;
    delete gradientMap;
    delete aspectMap;
    std::cout << "Successful" << std::endl;
}

 /**
  * @brief Run watershed delineation
  */
void handleWatershedAnalysis(Map<double>* elevationMap, Map<int>*& D8Map, Map<double>*& flowMap, Map<double>*& gradientMap, Map<double>*& aspectMap) {
    std::cout << "Entering watershed mode" << std::endl;

    //Containers
    char process[10], type[10];

    // Get process type
    std::cout << "Enter name of process to be used: ";
    std::cin.getline(process, 10);
    sscanf(process, "%s", type);

    // Process check
    if (strcmp(type, "mdf") != 0 && strcmp(type, "d8") != 0 && strcmp(type, "dinf") != 0) {
        std::cerr << "Invalid process. Exiting watershed mode.\n";
        return;
    }

    // Get number of pour points
    int nPourPoints;
    std::cout << "Enter number of pour points: ";
    std::cin >> nPourPoints;
    if (nPourPoints <= 0) {
        std::cerr << "Invalid number of pour points. Exiting watershed mode.\n";
        return;
    }

    // Get output directory for images
    std::string outputDir;
    std::cout << "Enter directory to store watershed images: ";
    std::cin >> outputDir;

    std::cout << "Using directory: " << outputDir << std::endl;
    
    // Get colourmap for images
    std::string colourmap;
    std::cout << "Enter colourmap for watershed images: ";
    std::cin >> colourmap;

    std::cout << "Using colourmap: " << colourmap << std::endl;

    // if else for watershed types
    if (strcmp(type, "d8") == 0) {
        //Create D8 direction map
        D8FlowAnalyser analyser(*elevationMap);
        analyser.analyseFlow();
        if (D8Map) delete D8Map;
        D8Map = new Map<int>(analyser.getMap());

        // Create flow accumulation map
        FlowAccumulator<double, int, double> flowAccumulator(*elevationMap, nullptr, nullptr, D8Map);
        if (flowMap) delete flowMap;
        flowMap = new Map<double>(flowAccumulator.accumulateFlow("d8"));

        // Find pour points
        std::vector<std::pair<int, int>> pourPoints;
        watershedAnalysis<double, int> watershedAnalyser(*elevationMap, D8Map, flowMap, nullptr, nullptr);
        pourPoints = watershedAnalyser.getPourPoints(nPourPoints, "d8");

        // Iterate over pour points
        int i = 0;
        for (const auto& p : pourPoints) {
            // Run watershed
            Map<double> outputWatershed = watershedAnalyser.calculateWatershed(p, "d8");
            outputWatershed.applyScaling("log");

            // Create full file pathway
            std::ostringstream oss;
            oss << outputDir << "/watershed_" << i << ".bmp";
            std::string filename = oss.str();

            // Export as image
            ImageExport<double>::exportMapToImage(outputWatershed, filename, colourmap, true);
            i++;
        }
        std::cout << "Exported watershed images to: " << outputDir << std::endl;
    }
    else if (strcmp(type, "dinf") == 0) {
        // Create gradient and slope maps
        SlopeAnalyser sAnalyser(*elevationMap);
        if (gradientMap) delete gradientMap;
        gradientMap = new Map<double>(sAnalyser.computeSlope("combined"));

        if (aspectMap) delete aspectMap;
        aspectMap = new Map<double>(sAnalyser.computeDirection());

        // Run flow accumulation
        FlowAccumulator<double, int, double> flowAccumulator(*elevationMap, aspectMap, gradientMap, nullptr);
        if (flowMap) delete flowMap;
        flowMap = new Map<double>(flowAccumulator.accumulateFlow("dinf"));

        // Find pour points
        std::vector<std::pair<int, int>> pourPoints;
        watershedAnalysis<double, int> watershedAnalyser(*elevationMap, nullptr, flowMap, gradientMap, aspectMap);
        pourPoints = watershedAnalyser.getPourPoints(nPourPoints, "dinf");
        
        // Iterate over pour points
        int i = 0;
        for (const auto& p : pourPoints) {
            // Run watershed
            Map<double> outputWatershed = watershedAnalyser.calculateWatershed(p, "dinf");
            outputWatershed.applyScaling("log");

            // Create full file pathway
            std::ostringstream oss;
            oss << outputDir << "/watershed_" << i << ".bmp";
            std::string filename = oss.str();

            // Export as image
            ImageExport<double>::exportMapToImage(outputWatershed, filename, colourmap, true);
            i++;
        }
        std::cout << "Exported watershed images to: " << outputDir << std::endl;
    }
    else if (strcmp(type, "mdf") == 0) {
        // Create gradient map
        SlopeAnalyser sAnalyser(*elevationMap);
        if (gradientMap) delete gradientMap;
        gradientMap = new Map<double>(sAnalyser.computeSlope("combined"));

        // Run flow accumulation
        FlowAccumulator<double, int, double> flowAccumulator(*elevationMap, nullptr, gradientMap, nullptr);
        if (flowMap) delete flowMap;
        flowMap = new Map<double>(flowAccumulator.accumulateFlow("mdf"));

        // Find pour points
        std::vector<std::pair<int, int>> pourPoints;
        watershedAnalysis<double, int> watershedAnalyser(*elevationMap, nullptr, flowMap, nullptr, nullptr);
        pourPoints = watershedAnalyser.getPourPoints(nPourPoints, "mdf");

        // Iterate over pour points
        int i = 0;
        for (const auto& p : pourPoints) {
            // Run watershed
            Map<double> outputWatershed = watershedAnalyser.calculateWatershed(p, "mdf");
            outputWatershed.applyScaling("log");
            
            // Create full file pathway
            std::ostringstream oss;
            oss << outputDir << "/watershed_" << i << ".bmp";
            std::string filename = oss.str();

            // Export as image
            ImageExport<double>::exportMapToImage(outputWatershed, filename, colourmap, true);
            i++;
        }
        std::cout << "Exported watershed images to: " << outputDir << std::endl;
    }
    else {
        // Failure
        std::cerr << "Invalid flow type specified for watershed analysis. Exiting watershed mode.\n";
    }
}