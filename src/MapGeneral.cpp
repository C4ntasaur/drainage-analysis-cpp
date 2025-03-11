#include "Map.h"
#include <iostream>
#include <cmath>

// Constructors
template <typename T>
Map<T>::Map() {
    width = 0;
    height = 0;
}

template <typename T>
Map<T>::Map(int w, int h) : width(w), height(h) {
    // Sets all positions to default T
    mapData.resize(height, std::vector<T>(width));
}

// Public Methods
template <typename T>
T Map<T>::getData(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        std::cerr << "Error: Index out of bounds (" << x << ", " << y << ")" << std::endl;
        std::cerr << "Map size: (" << width <<  ", " << height << ")" << std::endl;
        return T();  // Return default value if out of bounds
    }
    return mapData[y][x];
}

template <typename T>
void Map<T>::setData(int x, int y, T value) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        std::cerr << "Error: Index out of bounds (" << x << ", " << y << ")" << std::endl;
        std::cerr << "Map size: (" << width <<  ", " << height << ")" << std::endl;
        return;
    }
    mapData[y][x] = value;
}

template <typename T>
int Map<T>::getWidth(void) const{
    return width;
}

template <typename T>
int Map<T>::getHeight(void) const{
    return height;
}

template <typename T>
void Map<T>::applyScaling(const std::string& scale, double percentile) {
    if (scale == "log") {
        // Apply simple log scaling
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                T val = mapData[y][x];
                if (val > 0) { 
                    mapData[y][x] = std::log1p(val);
                } else {
                    mapData[y][x] = 0; // Keep zero for non-positive values
                }
            }
        }
    } 
    else if (scale == "log-filter") {
        // Ensure percentile is between 0 and 1
        percentile = std::clamp(percentile, 0.0, 1.0);

        // Store log values for filtering
        std::vector<T> logValues;

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                T val = mapData[y][x];
                if (val > 0) {  
                    logValues.push_back(std::log1p(val));
                }
            }
        }

        // Determine the threshold based on the percentile
        if (!logValues.empty()) {
            size_t index = static_cast<size_t>(percentile * logValues.size());
            std::nth_element(logValues.begin(), logValues.begin() + index, logValues.end());
            T threshold = logValues[index];  

            // Apply log transformation and filtering
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    T val = mapData[y][x];
                    if (val > 0) {
                        T logVal = std::log1p(val);
                        mapData[y][x] = (logVal >= threshold) ? logVal : 0;  // Keep values above threshold
                    } else {
                        mapData[y][x] = 0;
                    }
                }
            }
        }
    }
}


// Instantiation
template class Map<int>;
template class Map<float>;
template class Map<double>;