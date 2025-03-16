/**
 * @file MapProcessing.h
 * @author Ollie
 * @brief Function for handling DEMs whilst using CLI UI
 * @version 1.0.0
 * @date 2025-03-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef MAP_PROCESSING_H
#define MAP_PROCESSING_H

#include "../map_core/Map.h"
#include "../DEM_analysis/SobelAnalysis.h"
#include "../DEM_analysis/D8FlowAnalyser.h"
#include "../DEM_analysis/FlowAccumulation.h"
#include "../DEM_analysis/watershedAnalysis.h"
#include "../image_handling/ImageExport.h"

/**
 * @brief Create necessary maps for later analysis processes.
 * 

 */
void processMap(Map<double>& elevationMap, char* process, Map<int>& D8Map, Map<double>& flowMap,
    Map<double>& GMap, Map<double>& aspectMap, std::string& flowType);

/**
 * @brief Function that runs specific flow accumulaton algorithm for specified prcoess
 * 
 * @param elevationMap Input DEM
 * @param process Process specified by -p flag
 * @param D8Map Container for D8 flow directions map
 * @param flowMap Container for flow accumulation map
 * @param GMap Container for gradient map
 * @param aspectMap Container for aspect map
 * @param flowType container for flow type (mdf, dinf, d8)
 */
void handleFlowAccumulation(Map<double>& elevationMap, Map<int>& D8Map, Map<double>& flowMap,
    Map<double>& GMap, Map<double>& aspectMap, std::string& flowType, bool totalFlow);

/**
 * @brief Function that runs watershed delineation for specified process
 * 
 * @param elevationMap Input DEM
 * @param process Process specified by -p flag
 * @param D8Map Container for D8 flow directions map
 * @param flowMap Container for flow accumulation map
 * @param GMap Container for gradient map
 * @param aspectMap Container for aspect map
 * @param flowType container for flow type (mdf, dinf, d8)
 * @param watershed If watershed was selected
 * @param nPourPoints Number of pour points to identify
 * @param watershed_directory Where watershed output images are to be stored
 * @param watershed_colour Colour shortcode for colourmap used in watershed images
 */
void handleWatershed(Map<double>& elevationMap, Map<int>& D8Map, Map<double>& flowMap, Map<double>& GMap,
    Map<double>& aspectMap, std::string& flowType, bool watershed, int nPourPoints, char* watershed_directory,
    char* watershed_colour);

/**
 * @brief 
 * 
 * @param flowMap Container for flow accumulation map
 * @param D8Map Container for D8 flow directions map
 * @param aspectMap Container for aspect map
 * @param GMap Container for gradient map
 * @param output_file output file full pathway
 * @param image_file image file full pathway
 * @param input_file_type .bmp
 * @param colour_type colourmap for image output
 * @param process process specified (-p flag)
 * @param totalFlow flow accumulation specified (-fa flag)
 * @param watershed watershed delineation specified (-w flag)
 */
void handleOutput(Map<double>& flowMap, Map<int>& D8Map, Map<double>& aspectMap, Map<double>& GMap,
    char* output_file, char* image_file, char* input_file_type, char* colour_type, char* process,
    bool totalFlow, bool watershed);

#endif // MAP_PROCESSING_H