#include "Map.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <type_traits>

// Constructors
template <typename T>
Map<std::vector<T>>::Map() {
    width = 0;
    height = 0;
}

template <typename T>
Map<std::vector<T>>::Map(int w, int h) : width(w), height(h) {
    // Sets all positions to default T
    mapData.resize(height);

    for (int i = 0; i < height; i++) {
        mapData.resize(width);
    }
}

// Public Methods
template <typename T>
bool Map<std::vector<T>>::loadFromFile(const std::string& filename, const std::string& format) {
    if (format == "txt") {
        return loadFromTXT(filename);
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
bool Map<std::vector<T>>::saveToFile(const std::string& filename, const std::string& format) const {
    if (format == "txt") {
        return saveToTXT(filename);
    }
    else if (format == "bin") {
        return saveToBin(filename);
    }
    else {
        std::cerr << "Unsupported file format: " << format << std::endl;
        return false;
    }
}

template <typename T>
std::vector<T>& Map<std::vector<T>>::getData(int x, int y) {
    return mapData[x][y];
}

template <typename T>
void Map<std::vector<T>>::setData(int x, int y, const std::vector<T>& value) {
    mapData[x][y] = value;
}

template <typename T>
int Map<std::vector<T>>::getWidth(void) const{
    return width;
}

template <typename T>
int Map<std::vector<T>>::getHeight(void) const{
    return height;
}

//Private Methods

// General int or float use for .txt
template <typename T>
bool Map<std::vector<T>>::loadFromTXT(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;

        std::vector<std::vector<T>> rowData;
        while (std::getline(ss, token, ' ')) {
            std::stringstream tokenStream(token);
            std::vector<T> vecData;
            
            // Read comma-separated data inside the vector
            while (std::getline(tokenStream, token, ',')) {
                std::stringstream valueStream(token);
                T value;
                valueStream >> value;
                vecData.push_back(value);
            }

            rowData.push_back(vecData);
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
bool Map<std::vector<T>>::loadFromBin(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    // Get integer start values: height, width, and vector size
    file.read(reinterpret_cast<char*>(&height), sizeof(height));
    file.read(reinterpret_cast<char*>(&width), sizeof(width));

    int vectorSize = 0;
    file.read(reinterpret_cast<char*>(&vectorSize), sizeof(vectorSize));

    if (height <= 0 || width <= 0 || vectorSize <= 0) {
        std::cerr << "Invalid (height, width, or vector size) from the binary file." << std::endl;
        return false;
    }

    
    mapData.resize(height);

    
    for (int i = 0; i < height; i++) {
        mapData.resize(width);
        for (int j = 0; j < width; j++) {
            mapData[i][j].resize(vectorSize);
            file.read(reinterpret_cast<char*>(mapData[i][j].data()), vectorSize * sizeof(T));
        }
    }

    return true;
}


template <typename T>
bool Map<std::vector<T>>::saveToTXT(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }

    // Per row write values with space separation
    for (const auto& row : mapData) {
        bool firstVector = true;
        
        for (const auto& vec : row) {
            if (!firstVector) {
                file << " ";
            }
            firstVector = false;
        
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

template <typename T>
bool Map<std::vector<T>>::saveToBin(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }

    // Write integer height, width, and internal vector size
    file.write(reinterpret_cast<const char*>(&height), sizeof(height));
    file.write(reinterpret_cast<const char*>(&width), sizeof(width));

    int vectorSize = mapData[0][0].size();
    file.write(reinterpret_cast<const char*>(&vectorSize), sizeof(vectorSize));

    // Per row write values
    for (const auto& row : mapData) {
        for (const auto& vec : row) {
            file.write(reinterpret_cast<const char*>(vec.data()), vectorSize * sizeof(T));
        }
    }
    return true;
}

// Instantiation
template class Map<std::vector<int>>;
template class Map<std::vector<float>>;
template class Map<std::vector<double>>;