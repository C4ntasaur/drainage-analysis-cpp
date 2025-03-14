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

 #ifndef REPL_H
 #define REPL_H

 #include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include "../map_core/Map.h"
#include "../image_handling/BMP.h"
#include "../DEM_analysis/SobelAnalysis.h"
#include "../DEM_analysis/D8FlowAnalyser.h"
#include "../DEM_analysis/FlowAccumulation.h" 
#include "../DEM_analysis/watershedAnalysis.h"
#include "../image_handling/ImageExport.h"
#include "../CLI/CLIhelperFunctions.h"

// Function declarations
void runREPL(void);
bool loadFile(Map<double>*& elevationMap, const char* command);
void processData(Map<double>* elevationMap, Map<int>*& D8Map, Map<double>*& flowMap, Map<double>*& gradientMap, Map<double>*& aspectMap, const char* command);
void saveData(Map<double>* flowMap, Map<int>* D8Map, Map<double>* aspectMap, Map<double>* gradientMap, const char* command);
void exportData(Map<double>* flowMap, Map<int>* D8Map, Map<double>* aspectMap, Map<double>* gradientMap, const char* command);
void displayHelp();
void quitProgram(Map<double>*& elevationMap, Map<int>*& D8Map, Map<double>*& flowMap, Map<double>*& gradientMap, Map<double>*& aspectMap);
void handleWatershedAnalysis(Map<double>* elevationMap, Map<int>*& D8Map, Map<double>*& flowMap, Map<double>*& gradientMap, Map<double>*& aspectMap);
#endif 