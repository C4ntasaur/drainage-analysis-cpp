/**
 * @file FlowAccumulation.h
 * @author Ollie
 * @brief Flow accumulation class for multiple flow algorithms
 * @version 1.0.0
 * @date 2025-03-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef FLOWACCUMULATOR_H
#define FLOWACCUMULATOR_H

#include "../map_core/Map.h"
#include "SobelAnalysis.h"
#include <cmath>
#include <vector>

/**
 * @brief Class that determines flow accumulation over a DEM across multiple algortithms
 * 
 * @tparam elevationT 
 * @tparam D8T 
 * @tparam DinfT 
 */
template <typename elevationT, typename D8T, typename DinfT>
class FlowAccumulator {
public:
    /**
     * @brief Construct a new Flow Accumulator object
     * 
     * @param elevation 
     * Reference to elevation map (DEM). Used across all flow algorithms
     * @param aspect
     * Pointer to aspect map (0 - 359 degrees). Used for Dinf algorithm
     * @param G 
     * Pointer to gradient map. Used for MDF algorithm
     * @param D8 
     * Pointer to Directional 8 map. Used for D8 algoithm
     */
    FlowAccumulator(const Map<elevationT>& elevation, 
                    const Map<DinfT>* aspect = nullptr, 
                    const Map<DinfT>* G = nullptr, 
                    const Map<D8T>* D8 = nullptr);

    /**
     * @brief Public method used to determine flow accumulation over a DEM (elevationMap)
     * 
     * @param method 
     * String method used to call a specific algorithm
     * @return Map<elevationT> 
     * Returns flow accumulation map.
     */
    Map<elevationT> accumulateFlow(const std::string& method);

private:
    //
    const Map<elevationT>& _elevationMap;
    const Map<DinfT>* _aspectMap;  
    const Map<DinfT>* _gradientMap;      
    const Map<D8T>* _D8Map;

    int _width, _height;
    Map<elevationT> _flowMap;
    
    /**
     * @brief D8 flow accumulation method.
     * Determines flow to single neighbouring cell based on directions from _D8Map.
     * Works in descending elevation order (_elevationMap).
     * 
     * @see D8FLowAnalyser.h
     * @param _flowMap 
     * Reference to _flowMap
     * 
     */
    void accumulateD8(Map<elevationT>& _flowMap);

    /**
     * @brief Dinf flow accumulation method
     * Determines flow to two neighbouring cells based on aspect from _aspectMap.
     * Weighting between to cells is determined by difference in aspect of nearest cell to current aspect.
     * Works in descending elevation order (_elevationMap).
     * 
     * @param _flowMap 
     * Reference to _flowMap
     */
    void accumulateDinf(Map<elevationT>& _flowMap);

    /**
     * @brief Multi-directional Flow flow accumulation method
     * Determines flow to all lower elevation neighbours in a 3x3 grid based on _elevationMap.
     * Weighting of flow to cells is based on difference in gradient per cell to average gradient (_gradientMap) of all valid neighbours.
     * Works in descending order
     * 
     * @param _flowMap 
     * Reference to _flowMap
     */
    void accumulateMDF(Map<elevationT>& _flowMap);

    /**
     * @brief Returns two nearest directions (e.g. (0, 1)) for a given aspect and their weighting
     * 
     * @param theta
     * Aspect direction. 0 = North. Increasing theta -> clockwise cardinal directions.
     * @return std::tuple<std::array<int, 2>, std::array<int, 2>, double, double>
     * Tuple of (Direction1, Direction2, Weighting1, Weighting2)
     */
    std::tuple<std::array<int, 2>, std::array<int, 2>, double, double> getNearestTwoDirections(double theta);
};

#endif
