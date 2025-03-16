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

/**
 * @brief Run main REPL loop
 * 
 */
void runREPL(void);

/**
 * @brief Load DEM from file pathway specified after "load" command
 * 
 * @param elevationMap pointer to elevationMap container
 * @param command input file pathway
 * @return true 
 * @return false 
 */
bool loadFile(Map<double>*& elevationMap, const char* command);

/**
 * @brief Process DEM with type specified after "process" command
 * 
 * @param elevationMap Pointer to elevation map
 * @param D8Map Pointer to D8 directions map
 * @param flowMap Pointer to flow accumulation map
 * @param gradientMap Pointer to gradient map
 * @param aspectMap Pointer to aspect map
 * @param command Process type
 */
void processData(Map<double>* elevationMap, Map<int>*& D8Map, Map<double>*& flowMap, Map<double>*& gradientMap, Map<double>*& aspectMap, const char* command);

/**
 * @brief Save processed DEM as a file (e.g. txt, csv)
 * 
 * @param flowMap Pointer to flow accumulation map
 * @param D8Map Pointer to D8 directions map
 * @param aspectMap Pointer to aspect map
 * @param gradientMap Pointer to gradient map
 * @param command output file pathway
 */
void saveData(Map<double>* flowMap, Map<int>* D8Map, Map<double>* aspectMap, Map<double>* gradientMap, const char* command);

/**
 * @brief Export processed DEM as an image
 * 
 * @param flowMap Pointer to flow accumulation map
 * @param D8Map Pointer to D8 directions map
 * @param aspectMap Pointer to aspect map
 * @param gradientMap Pointer to gradient map
 * @param command image pathway and colourmap data
 * will be space separated as:
 *  > export ../file/pathway.bmp colourmap
 */
void exportData(Map<double>* flowMap, Map<int>* D8Map, Map<double>* aspectMap, Map<double>* gradientMap, const char* command);

/**
 * @brief Display help on commands avaliable
 * 
 */
void displayHelp();

/**
 * @brief Quit program and clears all allocated memory
 * 
 * @param elevationMap Pointer to elevation map
 * @param D8Map Pointer to D8 directions map
 * @param flowMap Pointer to flow accumulation map
 * @param gradientMap Pointer to gradiemt map
 * @param aspectMap Pointer to aspect map
 */
void quitProgram(Map<double>*& elevationMap, Map<int>*& D8Map, Map<double>*& flowMap, Map<double>*& gradientMap, Map<double>*& aspectMap);

/**
 * @brief Enter watershed analysis mode.
 * User will be asked a series of questions on:
 * Watershed delineation method
 * Number of pour points (and therefore images) to run
 * Colourmap of output images
 * 
 * @param elevationMap Pointer to elevation map
 * @param D8Map Pointer to D8 direction map
 * @param flowMap Pointer to flow accumulation map
 * @param gradientMap Pointer to gradient map
 * @param aspectMap Pointer to aspect map
 */
void handleWatershedAnalysis(Map<double>* elevationMap, Map<int>*& D8Map, Map<double>*& flowMap, Map<double>*& gradientMap, Map<double>*& aspectMap);
#endif 