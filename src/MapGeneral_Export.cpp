/**
 * @file MapGeneral_Export.cpp
 * @author Ollie
 * @brief Selection of methods for export of Map object to .bmp images
 * @version 1.0.0
 * @date 2025-03-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "Map.h"
#include "colourUtils.h"
#include <fstream>
#include <iostream>

/**
 * @brief Delegation method to determine colourmap of output image
 */
template <typename T>
bool Map<T>::exportToImage(const std::string& filename, const std::string& format) const {
    if (format == "g1" || format == "greyscale1") {
        return exportToBW(filename);
    }
    else if (format == "g2" || format == "greyscale2") {
        return exportToWB(filename);
    }
    else if (format == "dw" || format == "drywet") {
        return exportToDryWet(filename);
    }
    else if (format == "d8" || format == "d8") {
        return exportToD8(filename);
    }
    else if (format == "sf" || format == "seafloor") {
        return exportToSeaFloor(filename);
    }
    else {
        std::cerr << "Invalid export type." << std::endl;
        return false;
    }
}

/**
 * @brief Export to greyscale. White high.
 */
template <typename T>
bool Map<T>::exportToBW(const std::string& filename) const {
    // Create BMP object
    BMP image(_width, _height);

    // Find minimum and maximum cells in Map for scaling to 0-255
    T minValue = INT_MAX;
    T maxValue = INT_MIN;

    // Iterate over each cell
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            T value = _mapData[i][j];

            if (value < minValue) minValue = value;
            if (value > maxValue) maxValue = value;
        }
    }

    // Avoid division by zero
    double range = (maxValue != minValue) ? (maxValue - minValue) : 1.0;
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            T value = _mapData[i][j];

            // Normalise to greyscale
            uint8_t pixelValue = static_cast<uint8_t>(255 * (value - minValue) / (range));

            // Write into BMP reverse
            int row = _height - 1 - i;
            RGBTRIPLE pixel = {pixelValue, pixelValue, pixelValue};
            image.setPixel(j, row, pixel); // Set pixel at (j, row)
        }
    }
    image.write(filename.c_str());
    return true;
}

/**
 * @brief Export image as greyscale .bmp. Black high.
 */
template <typename T>
bool Map<T>::exportToWB(const std::string& filename) const {
    // Create BMP object
    BMP image(_width, _height);

    // Find minimum and maximum values for scaling between 0-255
    T minValue = INT_MAX;
    T maxValue = INT_MIN;

    // Iterate over each cell in Map
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            T value = _mapData[i][j];

            if (value < minValue) minValue = value;
            if (value > maxValue) maxValue = value;
        }
    }

    // Avoid division by zero
    double range = (maxValue != minValue) ? (maxValue - minValue) : 1.0;
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            T value = _mapData[i][j];

            // Normalise to greyscale
            uint8_t pixelValue = static_cast<uint8_t>(255 * (value - minValue) / (range));
            uint8_t inversePixelValue = 255 - pixelValue;

            // Write into BMP reverse
            int row = _height - 1 - i;
            RGBTRIPLE pixel = {inversePixelValue, inversePixelValue, inversePixelValue};
            image.setPixel(j, row, pixel); // Set pixel at (j, row)
        }
    }
    image.write(filename.c_str());
    return true;
}

/**
 * @brief Export image as DryWet colourmap .bmp
 */
template <typename T>
bool Map<T>::exportToDryWet(const std::string& filename) const {
    // Define Image
    BMP image(_width, _height);

    // Define ColourMap
    std::vector<RGBTRIPLE> colourmap = {
        //B    G    R
        {77, 137, 168},
        {120, 204, 226},
        {144, 232, 199},
        {201, 236, 139},
        {232, 192, 93},
        {224, 106, 255},
        {183, 28, 030},
        {133, 30, 020}
    };

    // Find minimum and maximum values for scaling to 0-255
    T minValue = INT_MAX;
    T maxValue = INT_MIN;

    // Iterate over every cell in Map
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            T value = _mapData[i][j];

            if (value < minValue) minValue = value;
            if (value > maxValue) maxValue = value;
        }
    }

    // Iterate over 2D array
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            T value = _mapData[i][j];

            value = (value - minValue) / (maxValue - minValue);

            // Find linearly interpolated colour from colour map
            RGBTRIPLE pixel = getColourFromColourmapContinuous(value, colourmap);
            int row = _height - 1 - i;
            image.setPixel(j, row, pixel); // Set pixel at (j, row)
        }
    }
    image.write(filename.c_str());
    return true;
}

/**
 * @brief Export Map as D8 ColourMap .bmp image.
 */
template <typename T>
bool Map<T>::exportToD8(const std::string& filename) const {
    // Define Image
    BMP image(_width, _height);

    // Define ColourMap
    std::vector<RGBTRIPLE> colourmap = {
        //B    G    R
        {255, 255, 255},
        {103, 184, 103},
        {54, 123, 54},
        {169, 211, 169},
        {72, 165, 72},
        {56, 133, 56},
        {196, 157, 196},
        {221, 84, 221},
        {244, 181, 224}
    };

    // Find minimum and maximum values for scaling between 0-255
    T minValue = INT_MAX;
    T maxValue = INT_MIN;

    // Iterate over every cell in Map
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            T value = _mapData[i][j];

            //if (value == -1) continue;

            if (value < minValue) minValue = value;
            if (value > maxValue) maxValue = value;
        }
    }

    // Iterate over 2D array
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            T value = _mapData[i][j];

            double normalizedValue = static_cast<double>(value - minValue) / (maxValue - minValue);

            // Discrete colourmap. Find which colour for each value
            RGBTRIPLE pixel = getColourFromColourmapDiscrete(normalizedValue, colourmap);
            int row = _height - 1 - i;
            image.setPixel(j, row, pixel); // Set pixel at (j, row)
        }
    }
    image.write(filename.c_str());
    return true;
}

/**
 * @brief Export image as SeaFloor colourmap image (bluescale).
 */
template <typename T>
bool Map<T>::exportToSeaFloor(const std::string& filename) const {
    // Define Image
    BMP image(_width, _height);

    // Define ColourMap
    std::vector<RGBTRIPLE> colourmap = {
        //B    G    R
        {248, 233, 206},
        {240, 197, 139},
        {232, 167, 93},
        {221, 128, 55},
        {214, 97, 51},
        {204, 55, 73},
        {198, 36, 93},
        {183, 26, 103}
    };

    // Find minimum and maximum values for scaling between 0-255
    T minValue = INT_MAX;
    T maxValue = INT_MIN;

    // Iterate over every cell in Map
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            T value = _mapData[i][j];

            if (value < minValue) minValue = value;
            if (value > maxValue) maxValue = value;
        }
    }

    // Iterate over 2D array
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            T value = _mapData[i][j];

            value = (value - minValue) / (maxValue - minValue);

            // Get colour value from continuous colour map. Linearly interpolated
            RGBTRIPLE pixel = getColourFromColourmapContinuous(value, colourmap);
            int row = _height - 1 - i;
            image.setPixel(j, row, pixel); // Set pixel at (j, row)
        }
    }
    image.write(filename.c_str());
    return true;
}

// Instantiate class methods
template class Map<int>;
template class Map<float>;
template class Map<double>;
