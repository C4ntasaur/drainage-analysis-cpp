/**
 * @file MapGeneral_IO.cpp
 * @author Ollie
 * @brief Selection of input and output methods for Map<T> class
 * @version 1.0.0
 * @date 2025-03-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "Map.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <type_traits>

/**
 * @brief Delegation input method for file to maps.
 */
template <typename T>
bool Map<T>::loadFromFile(const std::string& filename, const std::string& format) {
    if (format == "txt") {
        return loadFromTXT(filename);
    }
    else if (format == "csv") {
        return loadFromCSV(filename);
    }
    else if (format == "bin") {
        return loadFromBin(filename);
    }
    else {
        std::cerr << "Unsupported file format: " << format << std::endl;
        return false;
    }
}

/**
 * @brief Delegation output method for Maps to files.
 */
template <typename T>
bool Map<T>::saveToFile(const std::string& filename, const std::string& format) const {
    if (format == "txt") {
        return saveToTXT(filename);
    }
    else if (format == "csv") {
        return saveToCSV(filename);
    }
    else if (format == "bin") {
        return saveToBin(filename);
    }
    else {
        std::cerr << "Unsupported file format: " << format << std::endl;
        return false;
    }
}

/**
 * @brief Method to load Map object from space separated .txt
 */
template <typename T>
bool Map<T>::loadFromTXT(const std::string& filename) {
    // Open file
    std::ifstream file(filename);
    std::string line;
    std::vector<T> rowData;

    // Check successful opening
    if(!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    // Iterate over every row in file
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        T value;

        rowData.clear();
        // Split row by ' '
        while (ss >> value) {
            rowData.push_back(value);
            if (ss.peek() == ' ') {
                ss.ignore();
            }
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
 * @brief Load Map object for csv file
 */
template <typename T>
bool Map<T>::loadFromCSV(const std::string& filename) {
    // Open file
    std::ifstream file(filename);
    std::string line;
    std::vector<T> rowData;

    // Check that file opened successfully
    if(!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    // Iterate over every row in file
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        T value;

        rowData.clear();
        // Split by ' ' in row
        while (ss >> value) {
            rowData.push_back(value);
            if (ss.peek() == ',') {
                ss.ignore();
            }
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
 * @brief Load Map object from binary file
 */
template <typename T>
bool Map<T>::loadFromBin(const std::string& filename) {
    // open file in binary mode
    std::ifstream file(filename.c_str(), std::ios::binary);

    // Check that file opened
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    // Get integer start values
    file.read(reinterpret_cast<char*>(&_height), sizeof(_height));
    file.read(reinterpret_cast<char*>(&_width), sizeof(_width));

    // Check that file is of a valid size
    if (_height <= 0 || _width <= 0) {
        std::cerr << "Invalid height or width from the binary file." << std::endl;
        return false;
    }

    // Shape 2D vector
    _mapData.resize(_height, std::vector<T>(_width));

    // Read data by row
    for (int i = 0; i < _height; i++) {
        file.read(reinterpret_cast<char*>(&_mapData[i][0]), _width * sizeof(T));
    }
    return true;
}

/**
 * @brief Method to save Map object as a space separated txt
 */
template <typename T>
bool Map<T>::saveToTXT(const std::string& filename) const {
    // Open file
    std::ofstream file(filename.c_str());
    
    // Check successful opening
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }

    // Per row write values with space separation
    for (const auto& row : _mapData) {
        for (size_t i = 0; i < row.size(); i++) {
            file << row[i];
            if (i < row.size() - 1) file << " ";
        }
        file << std::endl;
    }
    return true;
}
/**
 * @brief Method to save Map object as csv file
 */
template <typename T>
bool Map<T>::saveToCSV(const std::string& filename) const {
    // Open file
    std::ofstream file(filename.c_str());
    
    // Check successful opening
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }

    // Per row write values with comma separation
    for (const auto& row : _mapData) {
        for (size_t i = 0; i < row.size(); i++) {
            file << row[i];
            if (i < row.size() - 1) file << ",";
        }
        file << std::endl;
    }
    return true;
}

/**
 * @brief Save Map object as binary file
 */
template <typename T>
bool Map<T>::saveToBin(const std::string& filename) const {
    // Open file
    std::ofstream file(filename.c_str(), std::ios::binary);

    // Check successful opening
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }

    // Write integer height and width for reading purpose
    file.write(reinterpret_cast<const char*>(&_height), sizeof(_height));
    file.write(reinterpret_cast<const char*>(&_width), sizeof(_width));

    // Per row write values
    for (const auto& row : _mapData) {
        file.write(reinterpret_cast<const char*>(&row[0]), row.size() * sizeof(T));
    }
    return true;
}

// Instatiation of methods
template class Map<int>;
template class Map<float>;
template class Map<double>;