#include "BMP.h"
#include <vector>

RGBTRIPLE interpolateColor(const RGBTRIPLE& color1, const RGBTRIPLE& color2, double t) {
    RGBTRIPLE result;
    result.rgbtRed   = static_cast<uint8_t>(color1.rgbtRed   + t * (color2.rgbtRed   - color1.rgbtRed));
    result.rgbtGreen = static_cast<uint8_t>(color1.rgbtGreen + t * (color2.rgbtGreen - color1.rgbtGreen));
    result.rgbtBlue  = static_cast<uint8_t>(color1.rgbtBlue  + t * (color2.rgbtBlue  - color1.rgbtBlue));
    return result;
}

RGBTRIPLE getColourFromColourmapContinuous(double value, const std::vector<RGBTRIPLE>& colormap) {
    const int numColors = colormap.size();
    double segmentWidth = 1.0 / (numColors - 1);

    if (value <= 0.0) return colormap.front();
    if (value >= 1.0) return colormap.back();

    int index = static_cast<int>(value / segmentWidth);
    if (index >= numColors - 1) index = numColors - 2;  // Cap index

    double localT = (value - index * segmentWidth) / segmentWidth;

    return interpolateColor(colormap[index], colormap[index + 1], localT);
}

RGBTRIPLE getColourFromColourmapDiscrete(double value, const std::vector<RGBTRIPLE>& colormap) {
    const int numColors = colormap.size();
    double segmentWidth = 1.0 / numColors;

    if (value <= 0.0) return colormap.front();
    if (value >= 1.0) return colormap.back();

    int index = static_cast<int>(value / segmentWidth);
    if (index >= numColors) index = numColors - 1;

    return colormap[index];
}
