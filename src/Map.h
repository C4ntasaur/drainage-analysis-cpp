/**
 * @file Map.h
 * @author Ollie
 * @brief Templated Map class for methods regarding a 2D array of numeric values.
 * @version 1.0.0
 * @date 2025-03-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>

/**
 * @brief Template Map class, a 2D array.
 * 
 * @tparam T Numeric types: double, float, int
 */
template <typename T>
class Map {
public:
    // Constructors
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
    
    // Methods
    /**
     * @brief Delegation method for loading a DEM into Map for different file types.
     * 
     * @param filename Full file pathway
     * @param format Accepts: "txt", "csv", and "bin"
     * @return true 
     * @return false 
     */
    bool loadFromFile(const std::string& filename, const std::string& format);

    /**
     * @brief Delegation method for saving a Map into to different file types.
     * 
     * @param filename Fill file pathway
     * @param format Accepts: "txt", "csv", and "bin"
     * @return true 
     * @return false 
     */
    bool saveToFile(const std::string& filename, const std::string& format) const;

    /**
     * @brief Get value at given cell (x, y) in Map
     * 
     * @param x Corresponds to row index
     * @param y Corresponds to column index
     * @return T Numerical template: double, float, int
     */
    T getData(int x, int y) const;

    /**
     * @brief Set value at given cell (x, y) in Map
     * 
     * @param x Corresponds to row index
     * @param y Corresponds to column index
     * @param value Numeric value to be set
     */
    void setData(int x, int y, T value);

    /**
     * @brief Return private member _width of Map
     * 
     * @return int 
     */
    int getWidth(void) const;

    /**
     * @brief Return private member _height of Map
     * 
     * @return int 
     */
    int getHeight(void) const;

    /**
     * @brief Export of Map to .bmp image
     * To be moved to another class soon
     * .txt colourmaps to be implemented
     * 
     * @param filename Full file pathway to save .bmp to
     * @param format Colour schemes
     * @return true 
     * @return false 
     */
    bool exportToImage(const std::string& filename, const std::string& format) const;

    /**
     * @brief Method that fills sinks in Map.
     * Sinks are cells with lower elevation than all their neighbours in a 3x3 grid
     * Removed by increasing sink value to that of the lowest neighbour
     * 
     */
    void fillSinks(void);

    /**
     * @brief Apply scaling to all values in a Map
     * 
     * @param scale Options: "log" and "log-filter"
     * @param percentile Set to 0.5 as default
     * For "log-filter" option, removes all values below specified percentile
     */
    void applyScaling(const std::string& scale, double percentile = 0.5);

private:
    // 2D array to store Map values in
    std::vector<std::vector<T>> _mapData;
    // Dimensions of map
    int _width, _height;

    /**
     * @brief Load Map from a space seperated .txt file
     * 
     * @param filename Full file pathway with extension .txt
     * @return true 
     * @return false 
     */
    bool loadFromTXT(const std::string& filename);

    /**
     * @brief Load Map from a .csv file
     * 
     * @param filename Full file pathway with extension .csv
     * @return true 
     * @return false 
     */
    bool loadFromCSV(const std::string& filename);

    /**
     * @brief Load Map from a binary file.
     * Binary files require integer height and width values at start for correct sizing
     * 
     * @param filename Full file pathway with extension .bin
     * @return true 
     * @return false 
     */
    bool loadFromBin(const std::string& filename);

    /**
     * @brief Save Map as a space seperated .txt file
     * 
     * @param filename Full file pathway with extension .txt
     * @return true 
     * @return false 
     */
    bool saveToTXT(const std::string& filename) const;

    /**
     * @brief Save Map as a csv file
     * 
     * @param filename Full file pathway with extension .csv
     * @return true 
     * @return false 
     */
    bool saveToCSV(const std::string& filename) const;

    /**
     * @brief Save Map as a binary file.
     * Uses Map dimension _height and _width as file headers to allow storing of dimensions
     * 
     * @param filename Full file pathway with extension .bin
     * @return true 
     * @return false 
     */
    bool saveToBin(const std::string& filename) const;

    /**
     * @brief Helper function for exportToImage().
     * Saves Map as a .bmp where pixel colours are greyscale, white high.
     * 
     * @param filename Full file pathway with extension .bmp
     * @return true 
     * @return false 
     */
    bool exportToBW(const std::string& filename) const;

    /**
     * @brief Helper function for exportToImage().
     * Saves Map as a .bmp where pixel colours are greyscale, black high.
     * 
     * @param filename Full file pathway with extension .bmp
     * @return true 
     * @return false 
     */
    bool exportToWB(const std::string& filename) const;

    /**
     * @brief Helper function for exportToImage().
     * Saves Map as a .bmp where pixel colours are of DryWet colourmap.
     * 
     * @param filename Full file pathway with extension .bmp
     * @return true 
     * @return false 
     */
    bool exportToDryWet(const std::string& filename) const;

    /**
     * @brief Helper function for exportToImage().
     * Saves Map as a .bmp where pixel colours are D8 colourmap.
     * 
     * @param filename Full file pathway with extension .bmp
     * @return true 
     * @return false 
     */
    bool exportToD8(const std::string& filename) const;

    /**
     * @brief Helper function for exportToImage().
     * Saves Map as a .bmp where pixel colours are bluescale, dark blue high.
     * 
     * @param filename Full file pathway with extension .bmp
     * @return true 
     * @return false 
     */
    bool exportToSeaFloor(const std::string& filename) const;

    /**
     * @brief Check if current cell (x, y) is a sink by comparing elevations with neighbours
     * 
     * @param x Corresponds to row index
     * @param y Corresponds to column index
     * @return true 
     * @return false 
     */
    bool is_sink(int x, int y);
};

#endif