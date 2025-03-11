#include "Map.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <type_traits>

// Function Trees
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

// In Functions
template <typename T>
bool Map<T>::loadFromTXT(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::vector<T> rowData;

    if(!file.is_open()) {
        //std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        T value;

        rowData.clear();

        while (ss >> value) {
            //std::cout << "Parsed value: " << value << std::endl;
            rowData.push_back(value);
            if (ss.peek() == ' ') {
                ss.ignore();
            }
        }
        //std::cout << "Row data size: " << rowData.size() << std::endl;

        mapData.push_back(rowData);
    }
    height = mapData.size();
    //std::cout << "Map height: " << height << std::endl;
    if (height > 0) {
        width = mapData[0].size();
        //std::cout << "Map width: " << width << std::endl;
    }

    return true;
}

template <typename T>
bool Map<T>::loadFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::vector<T> rowData;

    if(!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        T value;

        rowData.clear();

        while (ss >> value) {
            rowData.push_back(value);
            if (ss.peek() == ',') {
                ss.ignore();
            }
        }

        mapData.push_back(rowData);
    }

    height = mapData.size();
    if (height > 0) {
        width = mapData[0].size();
    }
    return true;
}

template <typename T>
bool Map<T>::loadFromBin(const std::string& filename) {
    std::ifstream file(filename.c_str(), std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    // Get integer start values
    file.read(reinterpret_cast<char*>(&height), sizeof(height));
    file.read(reinterpret_cast<char*>(&width), sizeof(width));

    if (height <= 0 || width <= 0) {
        std::cerr << "Invalid height or width from the binary file." << std::endl;
        return false;
    }

    // Shape 2D vector
    mapData.resize(height, std::vector<T>(width));

    // Read data by row
    for (int i = 0; i < height; i++) {
        file.read(reinterpret_cast<char*>(&mapData[i][0]), width * sizeof(T));
    }
    return true;
}

// Out Functions
template <typename T>
bool Map<T>::saveToTXT(const std::string& filename) const {
    std::ofstream file(filename.c_str());
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }

    // Per row write values with space separation
    for (const auto& row : mapData) {
        for (size_t i = 0; i < row.size(); i++) {
            file << row[i];
            if (i < row.size() - 1) file << " ";
        }
        file << std::endl;
    }
    return true;
}

template <typename T>
bool Map<T>::saveToCSV(const std::string& filename) const {
    std::ofstream file(filename.c_str());
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }

    // Per row write values with comma separation
    for (const auto& row : mapData) {
        for (size_t i = 0; i < row.size(); i++) {
            file << row[i];
            if (i < row.size() - 1) file << ",";
        }
        file << std::endl;
    }
    return true;
}

template <typename T>
bool Map<T>::saveToBin(const std::string& filename) const {
    std::ofstream file(filename.c_str(), std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }

    // Write integer height and width for reading purpose
    file.write(reinterpret_cast<const char*>(&height), sizeof(height));
    file.write(reinterpret_cast<const char*>(&width), sizeof(width));

    // Per row write values
    for (const auto& row : mapData) {
        file.write(reinterpret_cast<const char*>(&row[0]), row.size() * sizeof(T));
    }
    return true;
}

template class Map<int>;
template class Map<float>;
template class Map<double>;