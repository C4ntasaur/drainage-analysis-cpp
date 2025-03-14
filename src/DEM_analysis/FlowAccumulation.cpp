#include "FlowAccumulation.h"
#include <iostream>
#include <set>
#include <utility>

/**
 * @brief Construct a new Flow Accumulator<elevationT, D8T, DinfT>:: Flow Accumulator object
 */
template <typename elevationT, typename D8T, typename DinfT>
FlowAccumulator<elevationT, D8T, DinfT>::FlowAccumulator(const Map<elevationT>& elevation, 
                                                         const Map<DinfT>* aspect, 
                                                         const Map<DinfT>* G, 
                                                         const Map<D8T>* D8)
    : _elevationMap(elevation), _aspectMap(aspect), _gradientMap(G), _D8Map(D8),
    _flowMap(elevation.getWidth(), elevation.getHeight())
{
    _width = _elevationMap.getWidth();
    _height = _elevationMap.getHeight();
}

/**
 * @brief Delegating method to call specific flow accumulation algorithms
 */
template <typename elevationT, typename D8T, typename DinfT>
Map<elevationT> FlowAccumulator<elevationT, D8T, DinfT>::accumulateFlow(const std::string& method) {
    if (method == "d8") {
        if (!_D8Map) {
            std::cerr << "Error: D8 map is null. Cannot perform D8 flow accumulation." << std::endl;
            return _flowMap;
        }
        accumulateD8(_flowMap);
    }
    else if (method == "dinf") {
        if (!_aspectMap) {
            std::cerr << "Error: aspect map is null. Cannot perform Dinf flow accumulation." << std::endl;
            return _flowMap;
        }
        accumulateDinf(_flowMap);
    }
    else if (method == "mdf") {
        if (!_gradientMap) {
            std::cerr << "Error: gradient map is null. Cannot perform MDF flow accumulation." << std::endl;
            return _flowMap;
        }
        accumulateMDF(_flowMap);
    }
    else {
        std::cerr << "Unknown method: " << method << std::endl;
        return _flowMap;
    }
    return _flowMap;
}

/**
 * @brief D8 flow accumulation algorithm
 */
template <typename elevationT, typename D8T, typename DinfT>
void FlowAccumulator<elevationT, D8T, DinfT>::accumulateD8(Map<elevationT>& _flowMap) {
    // D8 directions where index in dx/dy correspond to D8 number
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

    // Gather all cells by x, y coords and their elevatations in vector<tuple>
    std::vector<std::tuple<elevationT, int, int>> cells;
    for (int y = 0; y < _height; y++) {
        for (int x = 0; x < _width; x++) {
            elevationT elevation = _elevationMap.getData(x, y);
            cells.emplace_back(elevation, x, y);
        }
    }

    // Sort cells by elevation
    std::sort(cells.begin(), cells.end(), [](const auto& a, const auto& b) {
        return std::get<0>(a) > std::get<0>(b); // descending elevation
    });

    // Iterate over all cells
    for (const auto& [elevation, x, y] : cells) {
        _flowMap.setData(x, y, _flowMap.getData(x, y) + 1.0); // Adding 1 to each cell visited
        
        D8T direction = _D8Map->getData(x, y);  // Get direction from D8Map
        if (direction == -1) {
            continue; // Skip no direction (ends loop)
        }

        // New directions
        int nx = x + dx[direction];
        int ny = y + dy[direction];

        // Out of bounds check
        if (nx >= 0 && nx < _width && ny >= 0 && ny < _height) {
            _flowMap.setData(nx, ny, _flowMap.getData(nx, ny) + _flowMap.getData(x, y));
        }
    }
}

/**
 * @brief Dinf Flow Algorithm (Tarboton 1997)
 */
template <typename elevationT, typename D8T, typename DinfT>
void FlowAccumulator<elevationT, D8T, DinfT>::accumulateDinf(Map<elevationT>& _flowMap) {    
    // Gather cells
    std::vector<std::tuple<elevationT, int, int>> cells;
    for (int y = 0; y < _height; y++) {
        for (int x = 0; x < _width; x++) {
            elevationT elevation = _elevationMap.getData(x, y);
            cells.emplace_back(elevation, x, y);
        }
    }

    // Sort by elevation (descending)
    std::sort(cells.begin(), cells.end(), [](const auto& a, const auto& b) {
        return std::get<0>(a) > std::get<0>(b); // Higher elevation first
    });

    // Create a temporary map to store updates
    Map<elevationT> tempFlowMap = _flowMap;

    // Iterate by descending order
    for (const auto& [elevation, x, y] : cells) {
        // Init cell with water
        tempFlowMap.setData(x, y, tempFlowMap.getData(x, y) + 1.0);
        
        // Pull aspect (angle)
        double theta = _aspectMap->getData(x, y);
        if (std::isnan(theta) || theta < 0) continue;  // Ensure aspect is valid

        // Find nearest two cells
        auto [dir1, dir2, weighting1, weighting2] = getNearestTwoDirections(theta);

        // Next 2 cell coordinates
        int nx1 = x + dir1[0];
        int ny1 = y + dir1[1];
        int nx2 = x + dir2[0];
        int ny2 = y + dir2[1];

        // Out of bounds check
        bool isCell1Valid = (nx1 >= 0 && ny1 >= 0 && nx1 < _width && ny1 < _height);
        bool isCell2Valid = (nx2 >= 0 && ny2 >= 0 && nx2 < _width && ny2 < _height);

        // Elevation check
        if (isCell1Valid) {
            elevationT neighbourElevation1 = _elevationMap.getData(nx1, ny1);
            if (neighbourElevation1 >= elevation) {
                isCell1Valid = false; // Cell 1 is not lower in elevation
            }
        }
        if (isCell2Valid) {
            elevationT neighbourElevation2 = _elevationMap.getData(nx2, ny2);
            if (neighbourElevation2 >= elevation) {
                isCell2Valid = false; // Cell 2 is not lower in elevation
            }
        }

        // Adjust weights
        if (!isCell1Valid) weighting1 = 0.0;
        if (!isCell2Valid) weighting2 = 0.0;

        // Normalize weights to ensure total sum is 1
        double weightSum = weighting1 + weighting2;
        if (weightSum > 0) {
            weighting1 /= weightSum;
            weighting2 /= weightSum;
        } else {
            continue;  // Skip flow update if weights are invalid
        }

        // Accumulate flow
        double flowValue = tempFlowMap.getData(x, y);
        if (isCell1Valid && weighting1 > 0.0) {
            tempFlowMap.setData(nx1, ny1, tempFlowMap.getData(nx1, ny1) + (flowValue * weighting1));
        }
        if (isCell2Valid && weighting2 > 0.0) {
            tempFlowMap.setData(nx2, ny2, tempFlowMap.getData(nx2, ny2) + (flowValue * weighting2));
        }
    }

    // Copy the updated flow map back to the original
    _flowMap = tempFlowMap;
}

/**
 * @brief Find two 'cardinal' directions for a given aspect as well as their weightings
 */
template <typename elevationT, typename D8T, typename DinfT>
std::tuple<std::array<int, 2>, std::array<int, 2>, double, double> 
FlowAccumulator<elevationT, D8T, DinfT>::getNearestTwoDirections(double aspect) {
    // Normalize aspect to [0, 360)
    aspect = std::fmod(aspect, 360.0);
    if (aspect < 0) aspect += 360.0;  // Ensure non-negative

    // Angle to 'Cardinal' Direction lookup array
    const std::array<std::pair<double, std::array<int, 2>>, 8> D8_DIRECTIONS = {{
        {0.0, {0, -1}},   // N
        {45.0, {1, -1}},  // NE
        {90.0, {1, 0}},   // E
        {135.0, {1, 1}},  // SE
        {180.0, {0, 1}},  // S
        {225.0, {-1, 1}}, // SW
        {270.0, {-1, 0}}, // W
        {315.0, {-1, -1}} // NW
    }};

    // Handle wrapping (aspect 315-360)
    if (aspect >= 315.0) {
        double w1 = (aspect - 315.0) / (360.0 - 315.0);  // Normalized weight for North
        double w2 = 1.0 - w1;  // Remaining weight for NW
        return std::make_tuple(D8_DIRECTIONS[0].second, D8_DIRECTIONS[7].second, w1, w2);
    }

    // Iterate over other directions
    for (size_t i = 1; i < D8_DIRECTIONS.size(); ++i) {
        if (std::fabs(aspect - D8_DIRECTIONS[i].first) < 1e-6) {
            // Exact match to a cardinal direction
            return std::make_tuple(D8_DIRECTIONS[i].second, D8_DIRECTIONS[i].second, 1.0, 0.0);
        }
        if (aspect < D8_DIRECTIONS[i].first) {
            // Weight calculation based on angle difference
            double w1 = (aspect - D8_DIRECTIONS[i-1].first) / (D8_DIRECTIONS[i].first - D8_DIRECTIONS[i-1].first);
            double w2 = 1.0 - w1;
            return std::make_tuple(D8_DIRECTIONS[i].second, D8_DIRECTIONS[i-1].second, w2, w1);
        }
    }

    // This should never be reached now
    std::cerr << "Unexpected aspect value encountered: " << aspect << std::endl;
    return std::make_tuple(D8_DIRECTIONS[0].second, D8_DIRECTIONS[7].second, 1.0, 0.0);
}



/**
 * @brief Multi-directional Flow flow accumulation method
 */
template <typename elevationT, typename D8T, typename DinfT>
void FlowAccumulator<elevationT, D8T, DinfT>::accumulateMDF(Map<elevationT>& _flowMap) {
    // Init Neighbour directions
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

    // Gather cells
    std::vector<std::tuple<elevationT, int, int>> cells;
    for (int y = 0; y < _height; y++) {
        for (int x = 0; x < _width; x++) {
            elevationT elevation = _elevationMap.getData(x, y); // assuming you have elevationMap
            cells.emplace_back(elevation, x, y);
        }
    }

    // Sort by elevation
    std::sort(cells.begin(), cells.end(), [](const auto& a, const auto& b) {
        return std::get<0>(a) > std::get<0>(b); // descending
    });

    // Iterate over descending elevation cells
    for (const auto& [elevation, x, y] : cells) {
        // Initialise cell with 1.0
        _flowMap.setData(x, y, _flowMap.getData(x, y) + 1.0);

        // Pull elevation for comparison with neighbours
        elevationT currentElevation = _elevationMap.getData(x, y);
        // Stores vector of valid neighbour indexes
        std::vector<int> validIndexes;

        // Iterate over directions (dx/dy)
        for (int i = 0; i < 8; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];

            // Out of bounds check
            if (nx < 0 || nx >= _width || ny < 0 || ny >= _height) {
                continue;
            }
            // Lower elevation check
            if (_elevationMap.getData(nx, ny) < currentElevation) {
                validIndexes.push_back(i);
            }
        }

        // No valid neighbours check
        if (validIndexes.size() == 0) {
            continue;
        }

        // Iterate over valid neighbours for total slope
        double overallSlope = 0.0;
        for (const int& i : validIndexes) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            overallSlope += _gradientMap->getData(nx, ny);
        }

        // Flat area check
        if (overallSlope == 0.0) {
            continue;
        }

        // iterate over neighbours again to distribute flow
        for (const int& i : validIndexes) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            
            double magnitude = _gradientMap->getData(nx, ny);
            double weight = magnitude / overallSlope;
            
            _flowMap.setData(nx, ny, _flowMap.getData(nx, ny) + (_flowMap.getData(x, y) * weight));
            
        }
    }   
}



// Explicit template instantiation for numeric types
template class FlowAccumulator<double, int, int>;
template class FlowAccumulator<float, int, int>;
template class FlowAccumulator<int, int, int>;

template class FlowAccumulator<double, int, double>;
template class FlowAccumulator<float, int, float>;

