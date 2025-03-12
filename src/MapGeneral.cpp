#include "Map.h"
#include <iostream>
#include <cmath>

/**
 * @brief Construct a new Map<T> object with 0,0 dimensions
 */
template <typename T>
Map<T>::Map() {
    _width = 0;
    _height = 0;
}

/**
 * @brief Construct a new Map<T> object with _width, _height dimensions
 */
template <typename T>
Map<T>::Map(int w, int h) : _width(w), _height(h) {
    // Sets all positions to default T
    _mapData.resize(_height, std::vector<T>(_width));
}

/**
 * @brief Get data from Map object at position x, y
 */
template <typename T>
T Map<T>::getData(int x, int y) const {
    if (x < 0 || x >= _width || y < 0 || y >= _height) {
        std::cerr << "Error: Index out of bounds (" << x << ", " << y << ")" << std::endl;
        std::cerr << "Map size: (" << _width <<  ", " << _height << ")" << std::endl;
        return T();  // Return default value if out of bounds
    }
    return _mapData[y][x];
}

/**
 * @brief Set data at position x, y of Map object with value
 */
template <typename T>
void Map<T>::setData(int x, int y, T value) {
    if (x < 0 || x >= _width || y < 0 || y >= _height) {
        std::cerr << "Error: Index out of bounds (" << x << ", " << y << ")" << std::endl;
        std::cerr << "Map size: (" << _width <<  ", " << _height << ")" << std::endl;
        return;
    }
    _mapData[y][x] = value;
}

/**
 * @brief Return _width member from Map
 */
template <typename T>
int Map<T>::getWidth(void) const{
    return _width;
}

/**
 * @brief Return _height member from Map
 */
template <typename T>
int Map<T>::getHeight(void) const{
    return _height;
}

/**
 * @brief Apply scaling of type 'scale' to _mapData of Map object
 */
template <typename T>
void Map<T>::applyScaling(const std::string& scale, double percentile) {
    if (scale == "log") {
        // Apply simple log scaling
        for (int y = 0; y < _height; y++) {
            for (int x = 0; x < _width; x++) {
                T val = _mapData[y][x];
                if (val > 0) { 
                    _mapData[y][x] = std::log1p(val);
                } else {
                    _mapData[y][x] = 0; // Keep zero for non-positive values
                }
            }
        }
    } 
    else if (scale == "log-filter") {
        // Ensure percentile is between 0 and 1
        percentile = std::clamp(percentile, 0.0, 1.0);

        // Store log values for filtering
        std::vector<T> logValues;

        for (int y = 0; y < _height; y++) {
            for (int x = 0; x < _width; x++) {
                T val = _mapData[y][x];
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
            for (int y = 0; y < _height; y++) {
                for (int x = 0; x < _width; x++) {
                    T val = _mapData[y][x];
                    if (val > 0) {
                        T logVal = std::log1p(val);
                        _mapData[y][x] = (logVal >= threshold) ? logVal : 0;  // Keep values above threshold
                    } else {
                        _mapData[y][x] = 0;
                    }
                }
            }
        }
    }
}


// Instantiation of class templates
template class Map<int>;
template class Map<float>;
template class Map<double>;