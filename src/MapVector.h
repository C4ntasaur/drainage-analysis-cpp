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

/**
 * @brief Template Map object with std::vector<T> at each cell position
 * Does not have the same implementation as standard Map<T> object,
 * may be a little buggy
 * 
 * @tparam T Numeric type: double, float, int
 */
template <typename T>
class Map<std::vector<T>> {
public:
    /**
     * @brief Create new Map object of 0,0 size
     */
    Map();
    /**
     * @brief Create new Map object of specified size
     * 
     * @param w Width
     * @param h Height
     */
    Map(int w, int h);
    
    /**
     * @brief Delegation method for loading an array of vector data into Map for different file types.
     * 
     * @param filename Full file pathway
     * @param format Accepts: "txt"
     * @return true 
     * @return false 
     */
    bool loadFromFile(const std::string& filename, const std::string& format);

    /**
     * @brief Delegation method for saving a Map of std::vector into to different file types.
     * 
     * @param filename Full file pathway
     * @param format Accepts: "txt"
     * @return true 
     * @return false 
     */
    bool saveToFile(const std::string& filename, const std::string& format) const;

    /**
     * @brief Get std::vector stored at position x, y in _mapData
     * 
     * @param x Corresponds to row index
     * @param y Corresponds to column index
     * @return std::vector<T>& Reference to vector at _mapData[y][x]
     */
    std::vector<T>& getData(int x, int y);

    /**
     * @brief Set std::vector at position x, y in _mapData
     * 
     * @param x Corresponds to row index
     * @param y Corresponds to column index
     * @param value std::vector<T>
     */
    void setData(int x, int y, const std::vector<T>& value);

    /**
     * @brief Return _width of Map
     * @return int 
     */
    int getWidth(void) const;

    /**
     * @brief Return _height of Map
     * @return int 
     */
    int getHeight(void) const;



private:
    // 2D array of numerical vectors
    std::vector<std::vector<std::vector<T>>> _mapData;
    // Dimensions of 2D array
    int _width, _height;

    /**
     * @brief Load vector values from comma separated values into cells of space separated vectors
     * 
     * @param filename Full file pathway with extension
     * @return true 
     * @return false 
     */
    bool loadFromTXT(const std::string& filename);

    /**
     * @brief Save Map of vectors to a .txt.
     * Vector data will be comma separated, Cell data (each vector) is space separated.
     * Example:
     * 1,1,1 2,2,2
     * 3,3,3 4,4,4
     * Is a 2D array of size (2, 2) with 4 1x3 vectors.
     * 
     * @param filename Full file pathway with extension
     * @return true 
     * @return false 
     */
    bool saveToTXT(const std::string& filename) const;
    
};

#endif