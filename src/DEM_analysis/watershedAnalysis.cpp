/**
 * @file watershedAnalysis.cpp
 * @author your name (you@domain.com)
 * @brief Watershed delineation class for Map objects: flow maps, gradients maps, and aspect
 * @version 1.0.0
 * @date 2025-03-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "watershedAnalysis.h"
#include <queue>
#include <stack>
#include <iostream>

/**
 * @brief Construct watershedAnalysis class for watershed delineation
 * and identification of pour points
 */
template<typename elevationT, typename D8T>
watershedAnalysis<elevationT, D8T>::watershedAnalysis(
    const Map<elevationT>  &elevation,
    const Map<D8T>* D8,
    const Map<elevationT>* flow,
    const Map<elevationT>* slope,
    const Map<elevationT>* aspect)
    : _elevationMap(elevation), _D8Map(D8), _flowMap(flow), _slopeMap(slope), _aspectMap(aspect),
    _height(elevation.getHeight()), _width(elevation.getWidth()) {
}

/**
 * @brief Delegation method for pour points identification
 */
template<typename elevationT, typename D8T>
std::vector<std::pair<int, int>> watershedAnalysis<elevationT, D8T>::getPourPoints(int nPoints, const std::string method) {
    if (method == "d8") {
        return D8PourPoints(nPoints);
    }
    else if (method == "mdf") {
        return MDFPourPoints(nPoints);
    }
    else {
        std::cerr << "Error: Unsupported method." << std::endl;
        return std::vector<std::pair<int ,int>>();
    }
}

/**
 * @brief Delegation method for watershed delineation algorithms
 */
template<typename elevationT, typename D8T>
Map<elevationT> watershedAnalysis<elevationT, D8T>::calculateWatershed(std::pair<int, int> Point, const std::string method) {
    if (method == "d8") {
        return D8watershed(Point);
    }
    else if (method == "dinf") {
        return DinfWatershed(Point);
    }
    else if (method == "mdf") {
        return MDFwatershed(Point);
    }
    else {
        std::cerr << "Error: Unsupported watershed delineation method." << std::endl;
        return Map<elevationT>(_width, _height);
    }
}

/**
 * @brief D8 Pour point identification method
Previous implementation of identifying pour points used sorting.
-> O(n log(n)) where n is the grid size.

Faster with a priortiy queue, only keeping top N values found.
-> O(n + m log(n) + nP log(nP)) where n is the grid size nxn, m is number of pour points, and nP is wanted number found.
Faster as m and nP should be typically smaller than n (grid size).
*/

// Quick struct for grouping cells from D8 map
struct PointWithFlow {
    int x, y;
    double flowValue;

    // overloaded operators as originals had unexpected behaviours
    // Do not remove
    bool operator>(const PointWithFlow& other) const {
        return flowValue > other.flowValue;
    }
    bool operator<(const PointWithFlow& other) const {
        return flowValue < other.flowValue;
    }
};

template<typename elevationT, typename D8T>
std::vector<std::pair<int, int>> watershedAnalysis<elevationT, D8T>::D8PourPoints(int nPoints) {
    // D8 directions from index (0-7)
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};
    std::vector<std::pair<int, int>> Points;

    // Create priority queue (min-heap based on flow value) of ascending flow accumulation
    std::priority_queue<PointWithFlow, std::vector<PointWithFlow>, std::greater<PointWithFlow>> minHeap;

    // iterate cells in Maps
    for (int y = 0; y < _height; y++) {
        for (int x = 0; x < _width; x++) {
            int flowDir = _D8Map->getData(x, y);
            bool isPourPoint = false;

            if (flowDir == -1) {
                isPourPoint = true;  // No forward flow direction -> pour point
            }
            else {
                int nx = x + dx[flowDir];
                int ny = y + dy[flowDir];

                // Out of bounds check
                if (nx < 0 || ny < 0 || nx >= _width || ny >= _height) {
                    isPourPoint = true;  // Flow out of map boundary -> pour point
                }
            }

            if (isPourPoint) {
                double flowValue = _flowMap->getData(x, y);
                // Push current point into queue
                minHeap.push({x, y, flowValue});

                // Keep only nPoints with the largest flow values
                if (minHeap.size() > nPoints) {
                    minHeap.pop();  // Remove the smallest flow value
                }
            }
        }
    }
    // Extract top nPoints from the heap (in terms of flow value)
    while (!minHeap.empty()) {
        PointWithFlow p = minHeap.top();
        Points.push_back({p.x, p.y});
        minHeap.pop();
    }
    return Points;
}

/**
 * @brief MDF method for identifying pour points
 */
template<typename elevationT, typename D8T>
std::vector<std::pair<int, int>> watershedAnalysis<elevationT, D8T>::MDFPourPoints(int nPoints) {
    // All direction about central cell
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};
    std::vector<std::pair<int, int>> Points;

    // Create priority queue (min-heap based on flow value) of ascending flow accumulation
    std::priority_queue<PointWithFlow, std::vector<PointWithFlow>, std::greater<PointWithFlow>> minHeap;

    // Iterate over every cell in Map
    for (int y = 0; y < _height; y++) {
        for (int x = 0; x < _width; x++) {
            bool isPourPoint = false;
            elevationT currentElevation = _elevationMap.getData(x, y);
            // Check if there is any neighbor with greater elevation
            bool hasTallerNeighbor = false;
            for (int dir = 0; dir < 8; dir++) {
                int nx = x + dx[dir];
                int ny = y + dy[dir];

                // Out of bounds check
                if (nx >= 0 && nx < _width && ny >= 0 && ny < _height) {
                    elevationT neighborElevation = _elevationMap.getData(nx, ny);
                    if (neighborElevation > currentElevation) {
                        hasTallerNeighbor = true;
                        break;  // We found at least one taller neighbor
                    }
                }
            }

            if (hasTallerNeighbor) {
                isPourPoint = true;  // Has a taller neighbor -> pour point candidate
            }

            // If the cell is a pour point based on the criteria, add to heap
            if (isPourPoint) {
                double flowValue = _flowMap->getData(x, y);
                // Push current point into queue
                minHeap.push({x, y, flowValue});

                // Keep only nPoints with the largest flow values
                if (minHeap.size() > nPoints) {
                    minHeap.pop();  // Remove the smallest flow value
                }
            }
        }
    }
    // Extract top nPoints from the heap (in terms of flow value)
    while (!minHeap.empty()) {
        PointWithFlow p = minHeap.top();
        Points.push_back({p.x, p.y});
        minHeap.pop();
    }
    return Points;
}

/**
 * @brief D8 watershed delineation method
 */
template<typename elevationT, typename D8T>
Map<elevationT> watershedAnalysis<elevationT, D8T>::D8watershed(std::pair<int, int> Point) {
    // Init D8 directions by index (0-7)
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

    int currentX = Point.first;
    int currentY = Point.second;

    Map<elevationT> visited(_width, _height); // To store flow values of visited cells

    // Set the flow value for the starting point
    visited.setData(currentX, currentY, _flowMap->getData(currentX, currentY));

    // Helper recursive function
    std::function<void(int, int)> visitUpstream = [&](int x, int y) {
        elevationT currentFlow = _flowMap->getData(currentX, currentY); // Flow of the current cell
        double MAX_ELEVATION = -1.7976931348623157e+308;  // Min double value for checking
        bool validNeighbourFound = false;

        // Find neigbour direction index (from dx/dy) for greatest flowing neighbour
        for (int dir = 0; dir < 8; dir++) {
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            // Out of bounds check
            if (nx < 0 || nx >= _width || ny < 0 || ny >= _height) {
                continue;
            }

            // Visited check
            if (visited.getData(nx, ny) != 0) {
                continue;
            }

            // Neighbour flow direction check
            int neighborFlowDir = _D8Map->getData(nx, ny);

            
            // If the neighbor is flowing into the current cell, it's a valid candidate
            if (x == nx + dx[neighborFlowDir] && y == ny + dy[neighborFlowDir]) {
                visited.setData(nx, ny, _flowMap->getData(nx, ny)); 
                visitUpstream(nx, ny);
                validNeighbourFound = true;
            }
        }
    };

    // Start recursive process from initial pour point
    visitUpstream(currentX, currentY);
    return visited;
}

/**
 * @brief Dinf watershed delineation method
 */
template<typename elevationT, typename D8T>
Map<elevationT> watershedAnalysis<elevationT, D8T>::DinfWatershed(std::pair<int, int> Point) {
    // Directions about cell in 3x3
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};
    int currentX = Point.first;
    int currentY = Point.second;

    Map<elevationT> visited(_width, _height); // To store flow values of visited cells

    // Set the flow value for the starting point
    visited.setData(currentX, currentY, 1);
    
    // Helper recursive function
    std::function<void(int, int)> visitUpstream = [&](int x, int y) {
        elevationT currentFlow = _flowMap->getData(currentX, currentY); // Flow of the current cell
        double MAX_ELEVATION = -1.7976931348623157e+308;  // Min double value for checking
        bool validNeighbourFound = false;

        elevationT currentAspect = _aspectMap->getData(x, y);
        elevationT currentSlope = _slopeMap->getData(x, y);

        // Find neigbour direction index (from dx/dy) for greatest flowing neighbour
        for (int dir = 0; dir < 8; dir++) {
            int nx = x + dx[dir];
            int ny = y + dy[dir];
            // Out of bounds check
            if (nx < 0 || nx >= _width || ny < 0 || ny >= _height) {
                continue;
            }

            // Visited check
            if (visited.getData(nx, ny) != 0) {
                continue;
            }
            elevationT neighbourAspect = _aspectMap->getData(nx, ny);
            elevationT neighbourSlope = _slopeMap->getData(nx, ny);

            
            
            // Check flow into current
            std::pair<std::array<int, 2>, std::array<int, 2>> directions = getNearestTwoDirections(neighbourAspect);
            std::array<int, 2> dir1 = directions.first;
            std::array<int, 2> dir2 = directions.second;
            int expectedX1 = nx + dir1[0];
            int expectedY1 = ny + dir1[1];
            int expectedX2 = nx + dir2[0];
            int expectedY2 = ny + dir2[1];
            if ((expectedX1 == x && expectedY1 == y) || (expectedX2 == x && expectedY2 == y)) {
                visited.setData(nx, ny, _flowMap->getData(nx, ny));
                visitUpstream(nx, ny);
                validNeighbourFound = true;
            }
        }
    };

    // Start recursive process from initial pour point
    visitUpstream(currentX, currentY);
    return visited;
}

/**
 * @brief Find two 'cardinal' directions for a given aspect as well as their weightings
 */
template <typename elevationT, typename D8T>
std::pair<std::array<int, 2>, std::array<int, 2>> watershedAnalysis<elevationT, D8T>::getNearestTwoDirections(double aspect) {
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
        return std::make_pair(D8_DIRECTIONS[0].second, D8_DIRECTIONS[7].second);
    }

    // Iterate over other directions
    for (size_t i = 1; i < D8_DIRECTIONS.size(); ++i) {
        if (std::fabs(aspect - D8_DIRECTIONS[i].first) < 1e-6) {
            // Exact match to a cardinal direction
            return std::make_pair(D8_DIRECTIONS[i].second, D8_DIRECTIONS[i].second);
        }
        if (aspect < D8_DIRECTIONS[i].first) {
            return std::make_pair(D8_DIRECTIONS[i].second, D8_DIRECTIONS[i-1].second);
        }
    }

    // This should never be reached now
    std::cerr << "Unexpected aspect value encountered: " << aspect << std::endl;
    return std::make_pair(D8_DIRECTIONS[0].second, D8_DIRECTIONS[7].second);
}

/**
 * @brief MDF watershed delineation algorithm
 */
template<typename elevationT, typename D8T>
Map<elevationT> watershedAnalysis<elevationT, D8T>::MDFwatershed(std::pair<int, int> Point) {
    // Directions about cell in a 3x3
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

    int currentX = Point.first;
    int currentY = Point.second;

    Map<elevationT> visited(_width, _height); // To store flow values of visited cells

    // Set the flow value for the starting point
    visited.setData(currentX, currentY, _flowMap->getData(currentX, currentY));

    // Helper recursive function to explore all valid neighboring cells with greater elevation
    std::function<void(int, int)> visitUpstream = [&](int x, int y) {
        elevationT currentElevation = _elevationMap.getData(x, y); // Elevation of the current cell
        bool validNeighbourFound = false;

        // Look at all 8 neighbors
        for (int dir = 0; dir < 8; dir++) {
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            // Out of bounds check
            if (nx < 0 || nx >= _width || ny < 0 || ny >= _height) {
                continue;
            }

            // Visited check
            if (visited.getData(nx, ny) != 0) {
                continue;
            }

            // Elevation check: Only proceed if the neighboring cell has a greater elevation
            elevationT neighborElevation = _elevationMap.getData(nx, ny);
            if (neighborElevation > currentElevation) {
                visited.setData(nx, ny, _flowMap->getData(nx, ny)); // Mark neighbor as visited
                visitUpstream(nx, ny); // Recursively visit this neighbor
                validNeighbourFound = true;
            }
        }
    };

    // Start recursive process from initial pour point
    visitUpstream(currentX, currentY);
    return visited;
}

// Instantiate class
template class watershedAnalysis<double, int>;
template class watershedAnalysis<float, int>;
template class watershedAnalysis<float, double>;
template class watershedAnalysis<double, double>;