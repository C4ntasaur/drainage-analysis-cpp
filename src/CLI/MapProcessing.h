#ifndef MAP_PROCESSING_H
#define MAP_PROCESSING_H

#include "../map_core/Map.h"
#include "../DEM_analysis/SobelAnalysis.h"
#include "../DEM_analysis/D8FlowAnalyser.h"
#include "../DEM_analysis/FlowAccumulation.h"
#include "../DEM_analysis/watershedAnalysis.h"
#include "../image_handling/ImageExport.h"


void processMap(Map<double>& elevationMap, char* process, Map<int>& D8Map, Map<double>& flowMap, Map<double>& GMap, Map<double>& aspectMap, std::string& flowType);

void handleFlowAccumulation(Map<double>& elevationMap, Map<int>& D8Map, Map<double>& flowMap, Map<double>& GMap, Map<double>& aspectMap, std::string& flowType, bool totalFlow);

void handleWatershed(Map<double>& elevationMap, Map<int>& D8Map, Map<double>& flowMap, Map<double>& GMap, Map<double>& aspectMap, std::string& flowType, bool watershed, int nPourPoints);

void handleOutput(Map<double>& flowMap, Map<int>& D8Map, Map<double>& aspectMap, Map<double>& GMap, char* output_file, char* image_file, char* input_file_type, char* colour_type, char* process, bool totalFlow);

#endif // MAP_PROCESSING_H