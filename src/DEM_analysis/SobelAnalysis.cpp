/**
 * @file SobelAnalysis.cpp
 * @author Methods and constructors for SlopeAnalyser object
 * @brief 
 * @version 0.1
 * @date 2025-03-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "SobelAnalysis.h"

#include <iostream>
#include <cmath>
#include <stdexcept>

/**
 * @brief Construct a new Slope Analyser< T>:: Slope Analyser object
 */
template <typename T>
SlopeAnalyser<T>::SlopeAnalyser(const Map<T>& map) : _elevationMap(map),
_height(map.getHeight()), _width(map.getWidth()) {
    
    if (_height == 0 || _width == 0) {
        std::cerr << "Map for slopeAnalyser cannot be empty." << std::endl;
    }
}

/**
 * @brief Delegation method for different gradient map algorithms
 */
template <typename T>
Map<T> SlopeAnalyser<T>::computeSlope(const std::string& type) {
    if (type == "gx") {
        return computeSlopeGx(); 
    }
    else if (type == "gy") {
        return computeSlopeGy();
    }
    else if (type == "combined") {
        return computeSlopeCombined();
    }
    else if (type == "direction") {
        return computeDirection();
    }
    else {
        std::cerr << "Type: " << type << " not recognised." << std::endl;
        return Map<T>(_width, _height);
    }
}

/**
 * @brief Create gradient map of overall x, y magnitude
 */
template <typename T>
Map<T> SlopeAnalyser<T>::computeSlopeCombined(void) {
    // Create gradient map
    Map<T> slopeMap(_width, _height);
    
    // iterate for cell in _elevationMap
    for (int y = 0; y < _height; y++) {
        for (int x = 0; x < _width; x++) {
            int Gx = 0, Gy = 0;
            // Iteratre for positions in sobel kernel
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;

                    // Kernel edge case check. Reflecting values for similar gradient.
                    if (nx < 0) nx = -nx;
                    if (ny < 0) ny = -ny;
                    if (nx >= _width) nx = 2 * _width - nx - 2;
                    if (ny >= _height) ny = 2 * _height - ny - 2;

                    T elevationValue = _elevationMap.getData(nx, ny);

                    Gx += _sobelX[dy + 1][dx + 1] * elevationValue;
                    Gy += _sobelY[dy + 1][dx + 1] * elevationValue;
                }
            }

            T slope = 0;
            if (Gx != 0 || Gy != 0) {
                slope = std::sqrt(static_cast<T>(Gx * Gx + Gy * Gy));
            }
            slopeMap.setData(x, y, slope);
        }
    }
    return slopeMap;
}

/**
 * @brief Create gradient map of x magnitude
 */
template <typename T>
Map<T> SlopeAnalyser<T>::computeSlopeGx(void) {
    // create gradient map
    Map<T> slopeMap(_width, _height);
    
    for (int y = 0; y < _height; y++) {
        for (int x = 0; x < _width; x++) {
            int Gx = 0, Gy = 0;

            // Iteratre for positions in sobel kernel
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;

                    // Kernel edge case check. Reflecting values for similar gradient.
                    if (nx < 0) nx = -nx;
                    if (ny < 0) ny = -ny;
                    if (nx >= _width) nx = 2 * _width - nx - 2;
                    if (ny >= _height) ny = 2 * _height - ny - 2;
                    T elevationValue = _elevationMap.getData(nx, ny);
                    
                    Gx += _sobelX[dy + 1][dx + 1] * elevationValue;
                }
            }

            T slope = 0;
            if (Gx != 0 || Gy != 0) {
                slope = std::sqrt(static_cast<T>(Gx * Gx));
            }
            slopeMap.setData(x, y, slope);
        }
    }
    return slopeMap;
}

/**
* @brief Create gradient map of y magnitude
 */
template <typename T>
Map<T> SlopeAnalyser<T>::computeSlopeGy(void) {
    // Create gradient map
    Map<T> slopeMap(_width, _height);
    
    // Iterate over cell in _elevationMap
    for (int y = 0; y < _height; y++) {
        for (int x = 0; x < _width; x++) {
            int Gx = 0, Gy = 0;

            // Iterate over position in sobel kernels
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;

                    // Kernel edge case check. Reflecting values for similar gradient.
                    if (nx < 0) nx = -nx;
                    if (ny < 0) ny = -ny;
                    if (nx >= _width) nx = 2 * _width - nx - 2;
                    if (ny >= _height) ny = 2 * _height - ny - 2;

                    T elevationValue = _elevationMap.getData(nx, ny);
                    Gy += _sobelY[dy + 1][dx + 1] * elevationValue;
                }
            }

            T slope = 0;
            if (Gx != 0 || Gy != 0) {
                slope = std::sqrt(static_cast<T>(Gy * Gy));
            }
            slopeMap.setData(x, y, slope);
        }
    }
    return slopeMap;
}

/**
 * @brief Create aspect map
 */
template <typename T>
Map<T> SlopeAnalyser<T>::computeDirection(void) {
    T elevationValue;
    Map<T> dirMap(_width, _height);
    
    // Threshold for small gradients (to avoid assigning 0 slope in flat areas)
    const T threshold = static_cast<T>(0.01); 

    // Iterate over cells in _elevationMap
    for (int y = 0; y < _height; y++) {
        for (int x = 0; x < _width; x++) {
            int Gx = 0, Gy = 0;

            // Apply Sobel kernel for the 3x3 grid around the current cell
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;

                    // Kernel edge case check. Reflecting values for similar gradient.
                    if (nx < 0) nx = -nx;
                    if (ny < 0) ny = -ny;
                    if (nx >= _width) nx = 2 * _width - nx - 2;
                    if (ny >= _height) ny = 2 * _height - ny - 2;

                    elevationValue = _elevationMap.getData(nx, ny);
                    Gx += _sobelX[dy + 1][dx + 1] * elevationValue;
                    Gy += _sobelY[dy + 1][dx + 1] * elevationValue;
                }
            }

            // Compute the gradient magnitude
            T gradientMagnitude = std::sqrt(static_cast<T>(Gx * Gx + Gy * Gy));
            
            // If the gradient is small, consider this a flat area (no slope)
            if (gradientMagnitude < threshold) {
                dirMap.setData(x, y, static_cast<T>(-1)); // No slope (flat area)
            } else {
                // Compute the angle of the gradient
                T angleRad = std::atan2(static_cast<T>(Gy), static_cast<T>(Gx));
                T angleDeg = angleRad * static_cast<T>(180.0 / M_PI);
                
                if (angleDeg < 0) {
                    angleDeg += 360;
                }
                angleDeg = fmod(angleDeg, 360);

                dirMap.setData(x, y, angleDeg);
            }
        }
    }
    return dirMap;
}

// Instantiation
template class SlopeAnalyser<int>;
template class SlopeAnalyser<float>;
template class SlopeAnalyser<double>;