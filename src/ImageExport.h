/**
 * @file ImageExport.h
 * @author Ollie
 * @brief Image exporter class to save Maps as .bmp
 * @version 1.0.0
 * @date 2025-03-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef IMAGE_EXPORT_H
#define IMAGE_EXPORT_H

#include "Map.h"
#include "BMP.h"
#include "colourUtils.h"
#include <string>
#include <vector>

template <typename T>
class ImageExport {
public:
    /**
     * @brief Export a Map object to a BMP image using the specified color map.
     * 
     * @param map The Map object to export.
     * @param filename The output BMP file name.
     * @param format The color map format (e.g., "greyscale1", "drywet", "d8", etc.).
     * @return true if the export was successful, false otherwise.
     */
    static bool exportMapToImage(const Map<T>& map, const std::string& filename, const std::string& format);

private:
    /**
     * @brief Export to greyscale (white high).
     */
    static bool exportToBW(const Map<T>& map, const std::string& filename);

    /**
     * @brief Export to greyscale (black high).
     */
    static bool exportToWB(const Map<T>& map, const std::string& filename);

    /**
     * @brief Export using the DryWet color map.
     */
    static bool exportToDryWet(const Map<T>& map, const std::string& filename);

    /**
     * @brief Export using the D8 color map.
     */
    static bool exportToD8(const Map<T>& map, const std::string& filename);

    /**
     * @brief Export using the SeaFloor color map.
     */
    static bool exportToSeaFloor(const Map<T>& map, const std::string& filename);
};

#endif // IMAGE_EXPORT_H