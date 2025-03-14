/**
 * @file D8FlowAnalyser.cpp
 * @author Ollie
 * @brief Constructor and methods for D8FlowAnalyser class
 * @see Map.h
 * @version 1.0.1
 * @date 2025-03-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "D8FlowAnalyser.h"
#include <iostream>

/**
 * @brief Construct a new D8FlowAnalyser<T>::D8FlowAnalyser object
 */
template <typename T>
D8FlowAnalyser<T>::D8FlowAnalyser(Map<T>& map) : _elevationData(map){
    _height = map.getHeight();
    _width = map.getWidth();
    if (_height == 0 || _width == 0) {
        // Error if new Map is of an invalid size
        std::cerr << "Map cannot be empty." << std::endl;
    }
    // Create new Map for storing directions
    _flowDirections = Map<int>(_width, _height);
}

/**
 * @brief Method of D8FlowAnalyser that sets cells of _flowDirections to repsective D8 direction.
 */
template <typename T>
void D8FlowAnalyser<T>::analyseFlow(void) {
    /// Iteration over every cell in _flowDirections
    for (int y = 0; y < _height; y++) {
        for (int x = 0; x < _width; x++) {
            analyseFlowAt(x, y);
        }
    }
}

/**
 * @brief Returns Map<int> _flowDirections from D8FlowAnalyser object.
 * If called before .analyseFlow(), a blank map will be returned
 */
template <typename T>
Map<int> D8FlowAnalyser<T>::getMap(void) {
    return _flowDirections;
}

/**
 * @brief Search 3x3 grid about given cell (x, y) for the direction of its lowest elevation neighbour
 */
template <typename T>
void D8FlowAnalyser<T>::analyseFlowAt(int x, int y) {
    // @param currentValue Avoids multiple table lookups
    T currentValue = _elevationData.getData(x, y);

    // @param dx, dy Arrays of int where index in both correspond to D8 directions
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

    // @param bestX, bestY Init best coords with current coords
    int bestX = x, bestY = y;
    // @param lowestValue Init with current cell value for lowest elevation checks
    T lowestValue = currentValue;
    // @param bestDirection Init bestDirection with -1. Used for no lowest direction found as default
    int bestDirection = -1;

    // iterate over all directions in dx/dy
    for (int dir = 0; dir < 8; dir++) {
        // @param nx, ny Create new (n) coords for x and y
        int nx = x + dx[dir];
        int ny = y + dy[dir];

        // Inbounds check
        if (nx >= 0  && nx < _width && ny >= 0  && ny < _height) {
           T neighbourValue = _elevationData.getData(nx, ny);
           // Neighbour elevation checks with lowest value
            if (neighbourValue < lowestValue) {
                // Update vars if found
                lowestValue = neighbourValue;
                bestX = nx;
                bestY = ny;
                bestDirection = dir;
            }
            else if (neighbourValue == lowestValue) {
                // Randomness if two elevations of equal height are discovered
                if (rand() % 2 == 0) {
                    bestX = nx;
                    bestY = ny;
                    bestDirection = dir;
                }
            }
        }
    }
    if (bestDirection != -1) {
        // Set direction to direction indice of lowest elevation neighbour
        _flowDirections.setData(x, y, bestDirection);
    }
    else {
        // Failure direction set to -1
        _flowDirections.setData(x, y, -1);
    }
    
    
}

/// Init template class for different numeric types
template class D8FlowAnalyser<int>;
template class D8FlowAnalyser<float>;
template class D8FlowAnalyser<double>;