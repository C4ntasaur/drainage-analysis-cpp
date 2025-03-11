#include "Map.h"
#include "colourUtils.h"
#include <fstream>
#include <iostream>

// Function Tree
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

// Export Functions
template <typename T>
bool Map<T>::exportToBW(const std::string& filename) const {
    BMP image(width, height);

    T minValue = INT_MAX;
    T maxValue = INT_MIN;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            T value = mapData[i][j];

            if (value < minValue) minValue = value;
            if (value > maxValue) maxValue = value;
        }
    }

    // Avoid division by zero
    double range = (maxValue != minValue) ? (maxValue - minValue) : 1.0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            T value = mapData[i][j];

            // Normalise to greyscale
            uint8_t pixelValue = static_cast<uint8_t>(255 * (value - minValue) / (range));

            // Write into BMP reverse
            int row = height - 1 - i;
            RGBTRIPLE pixel = {pixelValue, pixelValue, pixelValue};
            image.setPixel(j, row, pixel); // Set pixel at (j, row)
        }
    }
    image.write(filename.c_str());
    return true;
}

template <typename T>
bool Map<T>::exportToWB(const std::string& filename) const {
    BMP image(width, height);

    T minValue = INT_MAX;
    T maxValue = INT_MIN;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            T value = mapData[i][j];

            if (value < minValue) minValue = value;
            if (value > maxValue) maxValue = value;
        }
    }

    // Avoid division by zero
    double range = (maxValue != minValue) ? (maxValue - minValue) : 1.0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            T value = mapData[i][j];

            // Normalise to greyscale
            uint8_t pixelValue = static_cast<uint8_t>(255 * (value - minValue) / (range));
            uint8_t inversePixelValue = 255 - pixelValue;

            // Write into BMP reverse
            int row = height - 1 - i;
            RGBTRIPLE pixel = {inversePixelValue, inversePixelValue, inversePixelValue};
            image.setPixel(j, row, pixel); // Set pixel at (j, row)
        }
    }
    image.write(filename.c_str());
    return true;
}

template <typename T>
bool Map<T>::exportToDryWet(const std::string& filename) const {
    // Define Image
    BMP image(width, height);

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

    // Find minimum and maximum values for casting to [0,1]
    T minValue = INT_MAX;
    T maxValue = INT_MIN;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            T value = mapData[i][j];

            if (value < minValue) minValue = value;
            if (value > maxValue) maxValue = value;
        }
    }

    // Iterate over 2D array
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            T value = mapData[i][j];

            value = (value - minValue) / (maxValue - minValue);

            RGBTRIPLE pixel = getColourFromColourmapContinuous(value, colourmap);
            int row = height - 1 - i;
            image.setPixel(j, row, pixel); // Set pixel at (j, row)
        }
    }
    image.write(filename.c_str());
    return true;
}

template <typename T>
bool Map<T>::exportToD8(const std::string& filename) const {
    // Define Image
    BMP image(width, height);

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

    // Find minimum and maximum values for casting to [0,1]
    T minValue = INT_MAX;
    T maxValue = INT_MIN;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            T value = mapData[i][j];

            if (value == -1) continue;

            if (value < minValue) minValue = value;
            if (value > maxValue) maxValue = value;
        }
    }

    // Iterate over 2D array
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            T value = mapData[i][j];

            double normalizedValue = static_cast<double>(value - minValue) / (maxValue - minValue);

            RGBTRIPLE pixel = getColourFromColourmapDiscrete(normalizedValue, colourmap);
            int row = height - 1 - i;
            image.setPixel(j, row, pixel); // Set pixel at (j, row)
        }
    }
    image.write(filename.c_str());
    return true;
}

template <typename T>
bool Map<T>::exportToSeaFloor(const std::string& filename) const {
    // Define Image
    BMP image(width, height);

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

    // Find minimum and maximum values for casting to [0,1]
    T minValue = INT_MAX;
    T maxValue = INT_MIN;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            T value = mapData[i][j];

            if (value < minValue) minValue = value;
            if (value > maxValue) maxValue = value;
        }
    }

    // Iterate over 2D array
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            T value = mapData[i][j];

            value = (value - minValue) / (maxValue - minValue);

            RGBTRIPLE pixel = getColourFromColourmapContinuous(value, colourmap);
            int row = height - 1 - i;
            image.setPixel(j, row, pixel); // Set pixel at (j, row)
        }
    }
    image.write(filename.c_str());
    return true;
}



template class Map<int>;
template class Map<float>;
template class Map<double>;
