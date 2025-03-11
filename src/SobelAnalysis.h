#ifndef SLOPE_ANALYSIS_H
#define SLOPE_ANALYSIS_H

#include "Map.h"


template <typename T>
class SlopeAnalyser {
public:
    SlopeAnalyser(const Map<T>& map);
    Map<T> computeSlope(const std::string& type);
    Map<T> computeDirection(void);

private:
    const Map<T>& elevationMap;
    std::vector<std::vector<int>> sobelX = {
        {-1,  0,  1},
        {-2,  0,  2},
        {-1,  0,  1}
    };
    std::vector<std::vector<int>> sobelY = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };
    Map<T> computeSlopeCombined(void);
    Map<T> computeSlopeGx(void);
    Map<T> computeSlopeGy(void);
};

#endif