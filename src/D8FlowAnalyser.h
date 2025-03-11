/**
 * @file D8FlowAnalyser.h
 * @author Ollie (C4ntasaur)
 * @brief Header file for D8FlowAnalyser object.
 * Object is based upon Map object.
 * @see Map.h
 * @version 1.0.0
 * @date 2025-03-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef D8FLOWANALYSER_H
#define D8FLOWANALYSER_H

#include "Map.h"

/**
 * @brief Class definition for D8FlowAnalyser.
 * Creates Directional 8 (D8) map of DEM such that:
 * 5    6   7
 * 4    c   0
 * 3    2   1
 * where 0-8 represent position of lowest elevation neighbour in 3x3 grid relative to central cell 'c'.
 * 
 * @tparam T 
 * Numeric template parameter: double, float, int
 */
template <typename T>
class D8FlowAnalyser {
public:
    /**
     * @brief Construct a new D8FlowAnalyser object
     * 
     * @param map
     * Reference to existing elevation (DEM) map (2D array)
     */
    D8FlowAnalyser(Map<T>& map);
    
    /// @brief analyseFlow at every point in _elevationMap
    void analyseFlow(void);

    /// @return Map (2D array) of D8 directions (or empty if .analyseFlow() not called)
    Map<int> getMap(void);

private:
    /// All declared with '_' prefix to indicate private member.
    int _width, _height;
    Map<T> _elevationData;
    Map<int> _flowDirections;
    
    /// @brief Lowest elevation neighbour search function for 3x3 about cell declared by x, y.
    /// @param x Coord in row
    /// @param y Coord in column
    void analyseFlowAt(int x, int y);

    
};

#endif