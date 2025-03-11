#include "watershedAnalysis.h"
#include <queue>
#include <stack>
#include <iostream>

template<typename elevationT, typename D8T>
watershedAnalysis<elevationT, D8T>::watershedAnalysis(
    const Map<elevationT>  &elevation,
    const Map<D8T>* D8,
    const Map<elevationT>* flow,
    const Map<elevationT>* slope,
    const Map<elevationT>* aspect)
    : elevationMap(elevation), D8Map(D8), fMap(flow), slopeMap(slope), aspectMap(aspect) {
    width = elevationMap.getWidth();
    height = elevationMap.getHeight();
}


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
        std::cerr << "Error: Unsupported method." << std::endl;
        return Map<elevationT>(width, height);
    }
}


/*
Previous implementation of identifying pour points used sorting.
-> O(n log(n)) where n is the grid size.

Faster with a priortiy queue, only keeping top N values found.
-> O(n + m log(n) + nP log(nP)) where n is the grid size nxn, m is number of pour points, and nP is wanted number found.
Faster as m and nP should be typically smaller than n (grid size).
*/
struct PointWithFlow {
    int x, y;
    double flowValue;

    bool operator>(const PointWithFlow& other) const {
        return flowValue > other.flowValue;
    }
    bool operator<(const PointWithFlow& other) const {
        return flowValue < other.flowValue;
    }
};

template<typename elevationT, typename D8T>
std::vector<std::pair<int, int>> watershedAnalysis<elevationT, D8T>::D8PourPoints(int nPoints) {
    
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};
    std::vector<std::pair<int, int>> Points;

    // Create priority queue (min-heap based on flow value) of ascending flow accumulation
    std::priority_queue<PointWithFlow, std::vector<PointWithFlow>, std::greater<PointWithFlow>> minHeap;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int flowDir = D8Map->getData(x, y);
            bool isPourPoint = false;

            if (flowDir == -1) {
                isPourPoint = true;  // No forward flow direction -> pour point
            }
            else {
                // Check edge flow case
                int nx = x + dx[flowDir];
                int ny = y + dy[flowDir];

                if (nx < 0 || ny < 0 || nx >= width || ny >= height) {
                    isPourPoint = true;  // Flow out of map boundary -> pour point
                }
            }

            if (isPourPoint) {
                double flowValue = fMap->getData(x, y);
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

template<typename elevationT, typename D8T>
std::vector<std::pair<int, int>> watershedAnalysis<elevationT, D8T>::MDFPourPoints(int nPoints) {
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};
    std::vector<std::pair<int, int>> Points;

    // Create priority queue (min-heap based on flow value) of ascending flow accumulation
    std::priority_queue<PointWithFlow, std::vector<PointWithFlow>, std::greater<PointWithFlow>> minHeap;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            bool isPourPoint = false;
            elevationT currentElevation = fMap->getData(x, y);
            // Check if there is any neighbor with greater elevation (taller cell)
            bool hasTallerNeighbor = false;
            for (int dir = 0; dir < 8; dir++) {
                int nx = x + dx[dir];
                int ny = y + dy[dir];

                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    elevationT neighborElevation = fMap->getData(nx, ny);
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
                double flowValue = fMap->getData(x, y);
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


template<typename elevationT, typename D8T>
Map<elevationT> watershedAnalysis<elevationT, D8T>::D8watershed(std::pair<int, int> Point) {
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

    int currentX = Point.first;
    int currentY = Point.second;

    Map<elevationT> visited(width, height); // To store flow values of visited cells

    // Set the flow value for the starting point
    visited.setData(currentX, currentY, fMap->getData(currentX, currentY));
    std::cout << "Pour point: " << currentX << ", "  << currentY << std::endl;
    std::cout << "Pour point value: " << elevationMap.getData(currentX, currentY) << std::endl;

    // Helper recursive function
    std::function<void(int, int)> visitUpstream = [&](int x, int y) {
        std::cout << "Current cell: " << x << ", " << y << std::endl;
        elevationT currentFlow = fMap->getData(currentX, currentY); // Flow of the current cell
        double MAX_ELEVATION = -1.7976931348623157e+308;  // Min double value for checking
        bool validNeighbourFound = false;

        // Find neigbour direction index (from dx/dy) for greatest flowing neighbour
        for (int dir = 0; dir < 8; dir++) {
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            // Out of bounds check
            if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
                continue;
            }

            // Visited check
            if (visited.getData(nx, ny) != 0) {
                continue;
            }

            // Neighbour flow direction check
            int neighborFlowDir = D8Map->getData(nx, ny);

            
            // If the neighbor is flowing into the current cell, it's a valid candidate
            if (x == nx + dx[neighborFlowDir] && y == ny + dy[neighborFlowDir]) {
                visited.setData(nx, ny, fMap->getData(nx, ny)); 
                std::cout << "Moving to cell: " << nx << ", " << ny << std::endl;
                visitUpstream(nx, ny);
                validNeighbourFound = true;
            }
        }
        if (!validNeighbourFound) {
            std::cout << "No valid neighbors to move to!" << std::endl;
        }
    };

    // Start recursive process from initial pour point
    visitUpstream(currentX, currentY);
    return visited;
}


template<typename elevationT, typename D8T>
Map<elevationT> watershedAnalysis<elevationT, D8T>::DinfWatershed(std::pair<int, int> Point) {
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};
    int currentX = Point.first;
    int currentY = Point.second;

    Map<elevationT> visited(width, height); // To store flow values of visited cells

    // Set the flow value for the starting point
    visited.setData(currentX, currentY, 1);
    std::cout << "Pour point: " << currentX << ", "  << currentY << std::endl;
    std::cout << "Pour point value: " << elevationMap.getData(currentX, currentY) << std::endl;

    // Helper recursive function
    std::function<void(int, int)> visitUpstream = [&](int x, int y) {
        std::cout << "Current cell: " << x << ", " << y << std::endl;
        elevationT currentFlow = fMap->getData(currentX, currentY); // Flow of the current cell
        double MAX_ELEVATION = -1.7976931348623157e+308;  // Min double value for checking
        bool validNeighbourFound = false;

        elevationT currentAspect = aspectMap->getData(x, y);
        elevationT currentSlope = slopeMap->getData(x, y);

        // Find neigbour direction index (from dx/dy) for greatest flowing neighbour
        for (int dir = 0; dir < 8; dir++) {
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            elevationT neighbourAspect = aspectMap->getData(nx, ny);
            elevationT neighbourSlope = slopeMap->getData(nx, ny);

            // Out of bounds check
            if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
                continue;
            }

            // Visited check
            if (visited.getData(nx, ny) != 0) {
                continue;
            }
            
            // Check flow into current
            std::pair<std::array<int, 2>, std::array<int, 2>> directions = getNearestTwoDirections(neighbourAspect);
            std::array<int, 2> dir1 = directions.first;
            std::array<int, 2> dir2 = directions.second;
            int expectedX1 = nx + dir1[0];
            int expectedY1 = ny + dir1[1];
            int expectedX2 = nx + dir2[0];
            int expectedY2 = ny + dir2[1];
            if ((expectedX1 == x && expectedY1 == y) || (expectedX2 == x && expectedY2 == y)) {
                visited.setData(nx, ny, fMap->getData(nx, ny));
                std::cout << "Moving to cell: " << nx << ", " << ny << std::endl;
                visitUpstream(nx, ny);
                validNeighbourFound = true;
            }
        }
        if (!validNeighbourFound) {
            std::cout << "No valid neighbors to move to!" << std::endl;
        }
    };

    // Start recursive process from initial pour point
    visitUpstream(currentX, currentY);
    return visited;
}

template <typename elevationT, typename D8T>
std::pair<std::array<int, 2>, std::array<int, 2>> watershedAnalysis<elevationT, D8T>::getNearestTwoDirections(double aspect) {
    // Ensure aspect is within [0, 360)
    aspect = std::fmod(aspect, 360.0);
    if (aspect < 0) {
        aspect += 360.0;
    }

    // D8 direction lookup table (angles in degrees, corresponding dx, dy)
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

    // Find the two closest D8 directions
    int nearest1 = -1, nearest2 = -1;
    double min_diff1 = 360.0, min_diff2 = 360.0;

    for (int i = 0; i < D8_DIRECTIONS.size(); i++) {
        double diff = std::fabs(D8_DIRECTIONS[i].first - aspect);
        if (diff > 180.0) diff = 360.0 - diff; // Handle wraparound case

        if (diff < min_diff1) {
            min_diff2 = min_diff1;
            nearest2 = nearest1;

            min_diff1 = diff;
            nearest1 = i;
        } else if (diff < min_diff2) {
            min_diff2 = diff;
            nearest2 = i;
        }
    }

    // Return the two closest D8 directions as (dx, dy) pairs
    return {D8_DIRECTIONS[nearest1].second, D8_DIRECTIONS[nearest2].second};
}

template<typename elevationT, typename D8T>
Map<elevationT> watershedAnalysis<elevationT, D8T>::MDFwatershed(std::pair<int, int> Point) {
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

    int currentX = Point.first;
    int currentY = Point.second;

    Map<elevationT> visited(width, height); // To store flow values of visited cells
    std::cerr << "Map size: (" << visited.getWidth() <<  ", " << visited.getHeight() << ")" << std::endl;

    // Set the flow value for the starting point
    visited.setData(currentX, currentY, fMap->getData(currentX, currentY));
    std::cout << "Pour point: " << currentX << ", " << currentY << std::endl;
    std::cout << "Pour point value: " << elevationMap.getData(currentX, currentY) << std::endl;

    // Helper recursive function to explore all valid neighboring cells with greater elevation
    std::function<void(int, int)> visitUpstream = [&](int x, int y) {
        std::cout << "Current cell: " << x << ", " << y << std::endl;
        elevationT currentElevation = elevationMap.getData(x, y); // Elevation of the current cell
        bool validNeighbourFound = false;

        // Look at all 8 neighbors
        for (int dir = 0; dir < 8; dir++) {
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            // Out of bounds check
            if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
                continue;
            }

            // Visited check
            if (visited.getData(nx, ny) != 0) {
                continue;
            }

            // Elevation check: Only proceed if the neighboring cell has a greater elevation
            elevationT neighborElevation = elevationMap.getData(nx, ny);
            if (neighborElevation > currentElevation) {
                visited.setData(nx, ny, fMap->getData(nx, ny)); // Mark neighbor as visited
                std::cout << "Moving to higher elevation cell: " << nx << ", " << ny << " (Elevation: " << neighborElevation << ")" << std::endl;
                visitUpstream(nx, ny); // Recursively visit this neighbor
                validNeighbourFound = true;
            }
        }
        
        if (!validNeighbourFound) {
            std::cout << "No valid neighbors with greater elevation to move to!" << std::endl;
        }
    };

    // Start recursive process from initial pour point
    visitUpstream(currentX, currentY);
    return visited;
}



template class watershedAnalysis<double, int>;
template class watershedAnalysis<float, int>;
template class watershedAnalysis<float, double>;
template class watershedAnalysis<double, double>;