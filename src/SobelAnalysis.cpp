#include "SobelAnalysis.h"

#include <iostream>
#include <cmath>
#include <stdexcept>

template <typename T>
SlopeAnalyser<T>::SlopeAnalyser(const Map<T>& map) : elevationMap(map) {
    if (map.getHeight() == 0 || map.getWidth() == 0) {
        std::cerr << "Map cannot be empty." << std::endl;
    }
}

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
        int rows = elevationMap.getHeight();
        int cols = elevationMap.getWidth();
        std::cerr << "Type: " << type << " not recognised." << std::endl;
        return Map<T>(cols, rows);
    }
}

template <typename T>
Map<T> SlopeAnalyser<T>::computeSlopeCombined(void) {
    int rows = elevationMap.getHeight();
    int cols = elevationMap.getWidth();

    T elevationValue;

    Map<T> slopeMap(cols, rows);
    
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            int Gx = 0, Gy = 0;

            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;

                    // Kernel edge case check. Reflecting values for similar gradient.
                    if (nx < 0) nx = -nx;
                    if (ny < 0) ny = -ny;
                    if (nx >= cols) nx = 2 * cols - nx - 2;
                    if (ny >= rows) ny = 2 * rows - ny - 2;

                    elevationValue = elevationMap.getData(nx, ny);

                    Gx += sobelX[dy + 1][dx + 1] * elevationValue;
                    Gy += sobelY[dy + 1][dx + 1] * elevationValue;
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

template <typename T>
Map<T> SlopeAnalyser<T>::computeSlopeGx(void) {
    int rows = elevationMap.getHeight();
    int cols = elevationMap.getWidth();

    T elevationValue;

    Map<T> slopeMap(cols, rows);
    
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            int Gx = 0, Gy = 0;

            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;

                    // Kernel edge case check. Reflecting values for similar gradient.
                    if (nx < 0) nx = -nx;
                    if (ny < 0) ny = -ny;
                    if (nx >= cols) nx = 2 * cols - nx - 2;
                    if (ny >= rows) ny = 2 * rows - ny - 2;
                    elevationValue = elevationMap.getData(nx, ny);
                    
                    Gx += sobelX[dy + 1][dx + 1] * elevationValue;
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

template <typename T>
Map<T> SlopeAnalyser<T>::computeSlopeGy(void) {
    int rows = elevationMap.getHeight();
    int cols = elevationMap.getWidth();

    T elevationValue;

    Map<T> slopeMap(cols, rows);
    
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            int Gx = 0, Gy = 0;

            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;

                    // Kernel edge case check. Reflecting values for similar gradient.
                    if (nx < 0) nx = -nx;
                    if (ny < 0) ny = -ny;
                    if (nx >= cols) nx = 2 * cols - nx - 2;
                    if (ny >= rows) ny = 2 * rows - ny - 2;

                    elevationValue = elevationMap.getData(nx, ny);
                    Gy += sobelY[dy + 1][dx + 1] * elevationValue;
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


template <typename T>
Map<T> SlopeAnalyser<T>::computeDirection(void) {
    int rows = elevationMap.getHeight();
    int cols = elevationMap.getWidth();
    T elevationValue;
    Map<T> dirMap(cols, rows);
    
    // Threshold for small gradients (to avoid assigning 0 slope in flat areas)
    const T threshold = static_cast<T>(0.01); 

    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            int Gx = 0, Gy = 0;

            // Apply Sobel kernel for the 3x3 grid around the current cell
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;

                    // Kernel edge case check. Reflecting values for similar gradient.
                    if (nx < 0) nx = -nx;
                    if (ny < 0) ny = -ny;
                    if (nx >= cols) nx = 2 * cols - nx - 2;
                    if (ny >= rows) ny = 2 * rows - ny - 2;

                    elevationValue = elevationMap.getData(nx, ny);
                    Gx += sobelX[dy + 1][dx + 1] * elevationValue;
                    Gy += sobelY[dy + 1][dx + 1] * elevationValue;
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