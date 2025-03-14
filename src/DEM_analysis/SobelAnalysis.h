/**
 * @file SobelAnalysis.h
 * @author Ollie
 * @brief SlopeAnalyser object for determination of gradient and aspect maps
 * @version 1.0.0
 * @date 2025-03-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef SLOPE_ANALYSIS_H
#define SLOPE_ANALYSIS_H

#include "Map.h"

/**
 * @brief SlopeAnalyser class that allows determination of gradient and aspect maps from a given
 * elevation map.
 * Gradients are determined using a sobel kernel in the x and y directions.
 * This kernel makes use of reflected values at edges to avoid steep and unrepresentative gradients.
 * Gradient Maps created can be in the x or y directions, or the overall magnitude.
 * 
 * @tparam T Numerical types: double, float, int
 */
template <typename T>
class SlopeAnalyser {
public:
    /**
     * @brief Construct a new Slope Analyser object
     * 
     * @param map Elevation map
     */
    SlopeAnalyser(const Map<T>& map);

    /**
     * @brief Create a gradient map from _elevationMap of specified type
     * 
     * @param type Accepts: "combined", "gx", and "gy".
     * gx and gy represent gradients in the x and y directons respectively.
     * combined represents the overall magnitude of gradient at a given point
     * @return Map<T> Created gradient map
     */
    Map<T> computeSlope(const std::string& type);

    /**
     * @brief Create an aspect map from _elevationMap
     * 
     * @return Map<T> 
     */
    Map<T> computeDirection(void);

private:
    // Stored elevation map from constructor
    const Map<T>& _elevationMap;
    int _height, _width;

    std::vector<std::vector<int>> _sobelX = {
        {-1,  0,  1},
        {-2,  0,  2},
        {-1,  0,  1}
    };
    std::vector<std::vector<int>> _sobelY = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };

    /**
     * @brief Overall magnitude gradient map algorithm
     * @return Map<T> 
     */
    Map<T> computeSlopeCombined(void);

    /**
     * @brief Gradient map algorithm in x direction
     * @return Map<T> 
     */
    Map<T> computeSlopeGx(void);

    /**
     * @brief Gradient map alogrithm in y direction
     * @return Map<T> 
     */
    Map<T> computeSlopeGy(void);
};

#endif