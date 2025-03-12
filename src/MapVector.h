/**
 * @file MapVector.h
 * @author Ollie
 * @brief Templated Map class for methods regarding a 2D array of vector numeric values.
 * @version 1.0.0
 * @date 2025-03-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef MAPVECTOR_H
#define MAPVECTOR_H

#include <vector>
#include <string>
#include "Map.h"

// Specialised Vector Class
template <typename T>
class Map<std::vector<T>> {
public:
    // Constructors
    Map();
    Map(int w, int h);
    // Methods
    bool loadFromFile(const std::string& filename, const std::string& format);
    bool saveToFile(const std::string& filename, const std::string& format) const;
    std::vector<T>& getData(int x, int y);
    void setData(int x, int y, const std::vector<T>& value);
    int getWidth(void) const;
    int getHeight(void) const;



private:
    // Vars
    std::vector<std::vector<std::vector<T>>> mapData;
    int width, height;

    // Private Methods
    bool loadFromTXT(const std::string& filename);
    bool loadFromBin(const std::string& filename);

    bool saveToTXT(const std::string& filename) const;
    bool saveToBin(const std::string& filename) const;
    
};

#endif