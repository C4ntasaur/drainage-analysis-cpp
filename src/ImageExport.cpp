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

template <typename T>
bool ImageExport<T>::exportMapToImage(const Map<T>& map, const std::string& filename, const std::string& format) {
    if (format == "g1" || format == "greyscale1") {
        return exportToBW(map, filename);
    } else if (format == "g2" || format == "greyscale2") {
        return exportToWB(map, filename);
    } else if (format == "dw" || format == "drywet") {
        return exportToDryWet(map, filename);
    } else if (format == "d8" || format == "d8") {
        return exportToD8(map, filename);
    } else if (format == "sf" || format == "seafloor") {
        return exportToSeaFloor(map, filename);
    } else {
        std::cerr << "Invalid export type: " << format << std::endl;
        return false;
    }
}

template <typename T>
bool ImageExport<T>::exportToBW(const Map<T>& map, const std::string& filename) {
    int width = map.getWidth();
    int height = map.getHeight();
    BMP image(width, height);

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

    double range = (maxValue != minValue) ? (maxValue - minValue) : 1.0;

    // Write pixel data
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            T value = map.getData(j, i);
            uint8_t pixelValue = static_cast<uint8_t>(255 * (value - minValue) / range);
            int row = height - 1 - i;
            RGBTRIPLE pixel = {pixelValue, pixelValue, pixelValue};
            image.setPixel(j, row, pixel);
        }
    }

    image.write(filename.c_str());
    return true;
}

template <typename T>
bool ImageExport<T>::exportToWB(const Map<T>& map, const std::string& filename) {
    int width = map.getWidth();
    int height = map.getHeight();
    BMP image(width, height);

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

    double range = (maxValue != minValue) ? (maxValue - minValue) : 1.0;

    // Write pixel data
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            T value = map.getData(j, i);
            uint8_t pixelValue = static_cast<uint8_t>(255 * (value - minValue) / range);
            uint8_t inversePixelValue = 255 - pixelValue;
            int row = height - 1 - i;
            RGBTRIPLE pixel = {inversePixelValue, inversePixelValue, inversePixelValue};
            image.setPixel(j, row, pixel);
        }
    }

    image.write(filename.c_str());
    return true;
}

template <typename T>
bool ImageExport<T>::exportToDryWet(const Map<T>& map, const std::string& filename) {
    int width = map.getWidth();
    int height = map.getHeight();
    BMP image(width, height);

    std::vector<RGBTRIPLE> colourmap = {
        {77, 137, 168}, {120, 204, 226}, {144, 232, 199}, {201, 236, 139},
        {232, 192, 93}, {224, 106, 255}, {183, 28, 30}, {133, 30, 20}
    };

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

    // Write pixel data
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            T value = map.getData(j, i);
            double normalizedValue = static_cast<double>(value - minValue) / (maxValue - minValue);
            RGBTRIPLE pixel = getColourFromColourmapContinuous(normalizedValue, colourmap);
            int row = height - 1 - i;
            image.setPixel(j, row, pixel);
        }
    }

    image.write(filename.c_str());
    return true;
}

template <typename T>
bool ImageExport<T>::exportToD8(const Map<T>& map, const std::string& filename) {
    int width = map.getWidth();
    int height = map.getHeight();
    BMP image(width, height);

    std::vector<RGBTRIPLE> colourmap = {
        {255, 255, 255}, {103, 184, 103}, {54, 123, 54}, {169, 211, 169},
        {72, 165, 72}, {56, 133, 56}, {196, 157, 196}, {221, 84, 221}, {244, 181, 224}
    };

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

    // Write pixel data
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            T value = map.getData(j, i);
            double normalizedValue = static_cast<double>(value - minValue) / (maxValue - minValue);
            RGBTRIPLE pixel = getColourFromColourmapDiscrete(normalizedValue, colourmap);
            int row = height - 1 - i;
            image.setPixel(j, row, pixel);
        }
    }

    image.write(filename.c_str());
    return true;
}

template <typename T>
bool ImageExport<T>::exportToSeaFloor(const Map<T>& map, const std::string& filename) {
    int width = map.getWidth();
    int height = map.getHeight();
    BMP image(width, height);

    std::vector<RGBTRIPLE> colourmap = {
        {248, 233, 206}, {240, 197, 139}, {232, 167, 93}, {221, 128, 55},
        {214, 97, 51}, {204, 55, 73}, {198, 36, 93}, {183, 26, 103}
    };

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

    // Write pixel data
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            T value = map.getData(j, i);
            double normalizedValue = static_cast<double>(value - minValue) / (maxValue - minValue);
            RGBTRIPLE pixel = getColourFromColourmapContinuous(normalizedValue, colourmap);
            int row = height - 1 - i;
            image.setPixel(j, row, pixel);
        }
    }

    image.write(filename.c_str());
    return true;
}

// Explicit template instantiation
template class ImageExport<int>;
template class ImageExport<float>;
template class ImageExport<double>;