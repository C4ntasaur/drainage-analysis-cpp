/**
 * @file MapVector.cpp
 * @author your name (you@domain.com)
 * @brief Methods for vector type of Map object
 * @version 1.0.1
 * @date 2025-03-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "MapVector.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <type_traits>

/**
 * @brief Construct a new Map<std::vector< T>>:: Map object
 * @tparam T Numeric type
 */
template <typename T>
Map<std::vector<T>>::Map() {
    _width = 0;
    _height = 0;
}

/**
 * @brief Construct a new Map<std::vector< T>>:: Map object of size _width, _height
 */
template <typename T>
Map<std::vector<T>>::Map(int w, int h) : _width(w), _height(h) {
    // Sets all positions to default T
    _mapData.resize(_height);
    for (int i = 0; i < _height; i++) {
        _mapData.resize(_width);
    }
}

/**
 * @brief Delegation method for loading 2D array of vector data
 */
template <typename T>
bool Map<std::vector<T>>::loadFromFile(const std::string& filename, const std::string& format) {
    if (format == "txt") {
        return loadFromTXT(filename);
    }
    else {
        std::cerr << "Unsupported file format: " << format << std::endl;
        return false;
    }
}

/**
 * @brief Delegation method for saving 2D array of vector data
 */
template <typename T>
bool Map<std::vector<T>>::saveToFile(const std::string& filename, const std::string& format) const {
    if (format == "txt") {
        return saveToTXT(filename);
    }
    else {
        std::cerr << "Unsupported file format: " << format << std::endl;
        return false;
    }
}

/**
 * @brief Retrieve vector from _mapdata
 */
template <typename T>
std::vector<T>& Map<std::vector<T>>::getData(int x, int y) {
    return _mapData[y][x];
}

/**
 * @brief Set vector in _mapData
 */
template <typename T>
void Map<std::vector<T>>::setData(int x, int y, const std::vector<T>& value) {
    _mapData[y][x] = value;
}

/**
 * @brief Return _width
 */
template <typename T>
int Map<std::vector<T>>::getWidth(void) const{
    return _width;
}

/**
 * @brief Return _height
 */
template <typename T>
int Map<std::vector<T>>::getHeight(void) const{
    return _height;
}

/**
 * @brief Load 2D array of vector data from .txt file
 */
template <typename T>
bool Map<std::vector<T>>::loadFromTXT(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;

    // Check correct opening
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    // Iterate over file to get data
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;

        std::vector<std::vector<T>> rowData;
        // Split by space to get individual vectors
        while (std::getline(ss, token, ' ')) {
            std::stringstream tokenStream(token);
            std::vector<T> vecData;
            
            // Read comma separated data inside the vector
            while (std::getline(tokenStream, token, ',')) {
                std::stringstream valueStream(token);
                T value;
                valueStream >> value;
                vecData.push_back(value);
            }
            rowData.push_back(vecData);
        }
        _mapData.push_back(rowData);
    }
    _height = _mapData.size();
    if (_height > 0) {
        _width = _mapData[0].size();
    }
    return true;
}

/**
 * @brief Save 2D array of vector data to .txt file
 */
template <typename T>
bool Map<std::vector<T>>::saveToTXT(const std::string& filename) const {
    std::ofstream file(filename);
    // Check opening of file
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }

    // Per row write values with space separation
    for (const auto& row : _mapData) {
        bool firstVector = true;
        
        for (const auto& vec : row) {
            if (!firstVector) {
                file << " ";
            }
            firstVector = false;
            
            // Per vector write values with comma separation
            for (size_t i = 0; i < vec.size(); i++) {
                file << vec[i];
                if (i < vec.size() - 1) {
                    file << ",";
                }
            }
        }
    file << std::endl;
    }
    return true;
}

// Instantiation
template class Map<std::vector<int>>;
template class Map<std::vector<float>>;
template class Map<std::vector<double>>;