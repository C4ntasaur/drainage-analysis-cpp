#include "FlowAccumulation.h"
#include <iostream>
#include <set>
#include <utility>

// Constructor definition with template parameters elevationT, D8T, and DinfT
template <typename elevationT, typename D8T, typename DinfT>
FlowAccumulator<elevationT, D8T, DinfT>::FlowAccumulator(const Map<elevationT>& elevation, 
                                                         const Map<DinfT>* aspect, 
                                                         const Map<DinfT>* G, 
                                                         const Map<D8T>* D8)
    : elevationMap(elevation), aspectMap(aspect), gradientMap(G), D8Map(D8)
{
    width = elevationMap.getWidth();
    height = elevationMap.getHeight();
}

// Accumulate flow based on the method (D8 or Dinf)
template <typename elevationT, typename D8T, typename DinfT>
Map<elevationT> FlowAccumulator<elevationT, D8T, DinfT>::accumulateFlow(const std::string& method) {
    Map<elevationT> flowMap(width, height);

    if (method == "d8") {
        accumulateD8(flowMap);
    }
    else if (method == "dinf") {
        accumulateDinf(flowMap);
    }
    else if (method == "mdf") {
        accumulateMDF(flowMap);
    }
    else {
        std::cerr << "Unknown method: " << method << std::endl;
        return flowMap;
    }

    return flowMap;
}

// Accumulate flow for the D8 method
template <typename elevationT, typename D8T, typename DinfT>
void FlowAccumulator<elevationT, D8T, DinfT>::accumulateD8(Map<elevationT>& flowMap) {
    // D8 directions for D8FlowAnalysis
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

    std::vector<std::tuple<elevationT, int, int>> cells;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            elevationT elevation = elevationMap.getData(x, y); // assuming you have elevationMap
            cells.emplace_back(elevation, x, y);
        }
    }

    std::sort(cells.begin(), cells.end(), [](const auto& a, const auto& b) {
        return std::get<0>(a) > std::get<0>(b); // descending elevation
    });


    for (const auto& [elevation, x, y] : cells) {
        flowMap.setData(x, y, flowMap.getData(x, y) + 1.0);
        
        D8T direction = D8Map->getData(x, y);  // Get direction from D8Map
        if (direction == -1) {
            continue; // Skip no dir
        }

        int nx = x + dx[direction];
        int ny = y + dy[direction];

        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
            flowMap.setData(nx, ny, flowMap.getData(nx, ny) + flowMap.getData(x, y));
        }
    }
}

template <typename elevationT, typename D8T, typename DinfT>
void FlowAccumulator<elevationT, D8T, DinfT>::accumulateDinf(Map<elevationT>& flowMap) {    
    // Gather cells
    std::vector<std::tuple<elevationT, int, int>> cells;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            elevationT elevation = elevationMap.getData(x, y);
            cells.emplace_back(elevation, x, y);
        }
    }

    // Sort by elevation (descending)
    std::sort(cells.begin(), cells.end(), [](const auto& a, const auto& b) {
        return std::get<0>(a) > std::get<0>(b); // Higher elevation first
    });

    // Create a temporary map to store updates
    Map<elevationT> newFlowMap = flowMap;

    // Iterate by descending order
    for (const auto& [elevation, x, y] : cells) {
        // Init cell with water
        newFlowMap.setData(x, y, newFlowMap.getData(x, y) + 1.0);
        
        // Pull aspect (angle)
        double theta = aspectMap->getData(x, y);
        if (std::isnan(theta) || theta < 0) continue;  // Ensure aspect is valid

        // Find nearest two cells
        auto [dir1, dir2, weighting1, weighting2] = getNearestTwoDirections(theta);

        // Next 2 cell coordinates
        int nx1 = x + dir1[0];
        int ny1 = y + dir1[1];
        int nx2 = x + dir2[0];
        int ny2 = y + dir2[1];

        // Out of bounds check
        if (nx1 < 0 || ny1 < 0 || nx1 >= width || ny1 >= height) weighting1 = 0.0;
        if (nx2 < 0 || ny2 < 0 || nx2 >= width || ny2 >= height) weighting2 = 0.0;

        // Normalize weights to ensure total sum is 1
        double weightSum = weighting1 + weighting2;
        if (weightSum > 0) {
            weighting1 /= weightSum;
            weighting2 /= weightSum;
        } else {
            continue;  // Skip flow update if weights are invalid
        }

        // Accumulate flow
        double flowValue = newFlowMap.getData(x, y);
        if (weighting1 > 0.0) {
            newFlowMap.setData(nx1, ny1, newFlowMap.getData(nx1, ny1) + (flowValue * weighting1));
        }
        if (weighting2 > 0.0) {
            newFlowMap.setData(nx2, ny2, newFlowMap.getData(nx2, ny2) + (flowValue * weighting2));
        }
    }

    // Copy the updated flow map back to the original
    flowMap = newFlowMap;
}

template <typename elevationT, typename D8T, typename DinfT>
std::tuple<std::array<int, 2>, std::array<int, 2>, double, double> FlowAccumulator<elevationT, D8T, DinfT>::getNearestTwoDirections(double aspect) {
    // Ensure aspect is within [0, 360)
    aspect = std::fmod(aspect, 360.0);
    if (aspect < 0 || aspect > 360) {
        std::cerr << "Incorrect aspect used. Negative." << std::endl;
    }

    // Angle to Direction lookup
    const std::array<std::pair<double, std::array<int, 2>>, 9> D8_DIRECTIONS = {{
        {0.0, {0, -1}},   // N
        {45.0, {1, -1}},  // NE
        {90.0, {1, 0}},   // E
        {135.0, {1, 1}},  // SE
        {180.0, {0, 1}},  // S
        {225.0, {-1, 1}}, // SW
        {270.0, {-1, 0}}, // W
        {315.0, {-1, -1}},//NW
        {360.0, {0, -1}}  // N
    }};


    for (int i = 1; i < 9; i++) {
        if (aspect == D8_DIRECTIONS[i].first) {
            return std::make_tuple(D8_DIRECTIONS[i].second, D8_DIRECTIONS[i-1].second, 1.0, 0.0);
        }
        if (aspect < D8_DIRECTIONS[i].first) {
            double w1 = (aspect - D8_DIRECTIONS[i-1].first) / (D8_DIRECTIONS[i].first - D8_DIRECTIONS[i-1].first);
            double w2 = 1.0 - w1;
            return std::make_tuple(D8_DIRECTIONS[i].second, D8_DIRECTIONS[i-1].second, w2, w1);
        }
    }
    std::cerr << "UH OH BAD ASPECT" << std::endl;
    return std::make_tuple(D8_DIRECTIONS[0].second, D8_DIRECTIONS[7].second, 1.0, 0.0); // This really shouldn't happen
}



template <typename elevationT, typename D8T, typename DinfT>
void FlowAccumulator<elevationT, D8T, DinfT>::accumulateMDF(Map<elevationT>& flowMap) {
    // Init Neighbour directions
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

    // Gather cells
    std::vector<std::tuple<elevationT, int, int>> cells;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            elevationT elevation = elevationMap.getData(x, y); // assuming you have elevationMap
            cells.emplace_back(elevation, x, y);
        }
    }

    // Sort by elevation
    std::sort(cells.begin(), cells.end(), [](const auto& a, const auto& b) {
        return std::get<0>(a) > std::get<0>(b); // descending
    });

    // Iterate by descending elevation
    for (const auto& [elevation, x, y] : cells) {
        // Init cell w/ water
        flowMap.setData(x, y, flowMap.getData(x, y) + 1.0);

        // Pull elevation for comparison with neighbours
        elevationT currentElevation = elevationMap.getData(x, y);
        std::vector<int> validIndexes;
        for (int i = 0; i < 8; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if (nx < 0 || nx >= elevationMap.getWidth() || ny < 0 || ny >= elevationMap.getHeight()) {
                continue;
            }
            if (elevationMap.getData(nx, ny) < currentElevation) {
                validIndexes.push_back(i);
            }
        }

        // No valid neighbours check
        if (validIndexes.size() == 0) {
            continue;
        }

        // iterate over valid neighbours for total slope
        double overallSlope = 0.0;
        for (const int& i : validIndexes) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            overallSlope += gradientMap->getData(nx, ny);
        }
        if (overallSlope == 0.0) {
            continue;
        }

        // iterate over neighbours again to distribute water
        for (const int& i : validIndexes) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            
            double magnitude = gradientMap->getData(nx, ny);
            double weight = magnitude / overallSlope;
            
            flowMap.setData(nx, ny, flowMap.getData(nx, ny) + (flowMap.getData(x, y) * weight));
            
        }
    }   
}



// Explicit template instantiation for different types (double, int, etc.)
template class FlowAccumulator<double, int, int>;
template class FlowAccumulator<float, int, int>;
template class FlowAccumulator<int, int, int>;

template class FlowAccumulator<double, int, double>;
template class FlowAccumulator<float, int, float>;

