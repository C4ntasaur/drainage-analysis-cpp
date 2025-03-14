/**
 * @file colourUtils.h
 * @author Ollie
 * @brief Utility functions for colourmaps
 * @version 1.0.0
 * @date 2025-03-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "BMP.h"
#include <vector>

/**
 * @brief Interpolate colour function.
 * Takes two BGR colours and interpolates between them linearly.
 * 
 * @param colour1 
 * @param colour2 
 * @param t 
 * @return RGBTRIPLE 
 */
inline RGBTRIPLE interpolateColour(const RGBTRIPLE& colour1, const RGBTRIPLE& colour2, double t) {
    RGBTRIPLE result;
    result.rgbtRed   = static_cast<uint8_t>(colour1.rgbtRed   + t * (colour2.rgbtRed   - colour1.rgbtRed));
    result.rgbtGreen = static_cast<uint8_t>(colour1.rgbtGreen + t * (colour2.rgbtGreen - colour1.rgbtGreen));
    result.rgbtBlue  = static_cast<uint8_t>(colour1.rgbtBlue  + t * (colour2.rgbtBlue  - colour1.rgbtBlue));
    return result;
}

/**
 * @brief Finds colour from a colourmap in a constinuous fashion.
 * 
 * @param value Between 0 and 1
 * @param colourmap vector of BGR RGBTRIPLE structs
 * @see BMP.h
 * @return RGBTRIPLE Returns single colour
 */
inline RGBTRIPLE getColourFromColourmapContinuous(double value, const std::vector<RGBTRIPLE>& colourmap) {
    // Find number of colours in map
    const int numcolours = colourmap.size();
    // Find width
    double segmentWidth = 1.0 / (numcolours - 1);
    // Edge cases
    if (value <= 0.0) return colourmap.front();
    if (value >= 1.0) return colourmap.back();

    // Find index of colour
    int index = static_cast<int>(value / segmentWidth);
    if (index >= numcolours - 1) index = numcolours - 2;  // Cap index

    // Find scaling values
    double localT = (value - index * segmentWidth) / segmentWidth;
    RGBTRIPLE result = interpolateColour(colourmap[index], colourmap[index + 1], localT);
    return result;
}

inline RGBTRIPLE getColourFromColourmapDiscrete(double value, const std::vector<RGBTRIPLE>& colourmap) {
    // Find number of colours in map
    const int numcolours = colourmap.size();
    // Find width
    double segmentWidth = 1.0 / numcolours;
    // Edge cases
    if (value <= 0.0) return colourmap.front();
    if (value >= 1.0) return colourmap.back();

    // Find index of colour
    int index = static_cast<int>(value / segmentWidth);
    if (index >= numcolours) index = numcolours - 1;
    // Return single colour
    return colourmap[index];
}
