/**
 * @file ImageExport.cpp
 * @author Ollie
 * @brief Image exporter methods to save Maps as .bmp
 * @version 1.0.0
 * @date 2025-03-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "ImageExport.h"
#include <limits>
#include <iostream>
#include <fstream>
#include <sstream>

template <typename T>
bool ImageExport<T>::exportMapToImage(const Map<T>& map, const std::string& filename,
    const std::string& colourmapName, bool continuous) {

    int width = map.getWidth();
    int height = map.getHeight();

    BMP image(width, height);

    std::string colourmapFile = "../data/colourmaps/" + colourmapName + ".txt";
    std::vector<RGBTRIPLE> colourmap = loadColourmap(colourmapFile);

    if (colourmap.empty()) {
        std::cerr << "Failed to load colourmap: " << colourmapFile << std::endl;
        return false;
    }

    T minValue = std::numeric_limits<T>::max();
    T maxValue = std::numeric_limits<T>::min();

    // Find min and max values for scaling
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            T value = map.getData(j, i);
            if (value < minValue) minValue = value;
            if (value > maxValue) maxValue = value;
        }
    }

    double range;
    if (maxValue != minValue) {
        range = maxValue - minValue;
    } else {
        range = 1.0;
    }

    // Write pixel data
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            T value = map.getData(j, i);
            double normalizedValue = static_cast<double>(value - minValue) / range;

            RGBTRIPLE pixel;

            if (continuous) {
                pixel = getColourFromColourmapContinuous(normalizedValue, colourmap);
            } else {
                pixel = getColourFromColourmapDiscrete(normalizedValue, colourmap);
            }
            int row = height - 1 - i;
            image.setPixel(j, row, pixel);
        }
    }

    image.write(filename.c_str());
    return true;
}

template <typename T>
std::vector<RGBTRIPLE> ImageExport<T>::loadColourmap(const std::string& filename) {
    std::vector<RGBTRIPLE> colourmap;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Could not open colourmap file: " << filename << std::endl;
        return colourmap;
    }
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        RGBTRIPLE pixel;
        int red, green, blue;
        if (!(iss >> blue >> green >> red)) {
            std::cerr << "Error reading color values from line: " << line << std::endl;
            continue;  // Skip this line
        }
        
        // Cast stream values to int
        pixel.rgbtRed = static_cast<uint8_t>(red);
        pixel.rgbtGreen = static_cast<uint8_t>(green);
        pixel.rgbtBlue = static_cast<uint8_t>(blue);

        colourmap.push_back(pixel);
    }

    return colourmap;
}

// Explicit template instantiation
template class ImageExport<int>;
template class ImageExport<float>;
template class ImageExport<double>;