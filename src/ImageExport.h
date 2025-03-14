/**
 * @file ImageExport.h
 * @author Ollie
 * @brief Image exporter class to save Maps as .bmp
 * @version 1.1.0
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
    static bool exportMapToImage(const Map<T>& map, const std::string& filename,
        const std::string& colourmapName, bool continuous);

private:
    /**
     * @brief Loads colourmap from a file specified in ../data/colourmaps/
     * 
     * @param filename Full file pathway with .txt extension
     * @return std::vector<RGBTRIPLE> 
     */
    static std::vector<RGBTRIPLE> loadColourmap(const std::string& filename);
};

#endif // IMAGE_EXPORT_H