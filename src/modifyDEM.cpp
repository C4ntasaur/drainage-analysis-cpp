/**
 * @file modifyDEM.cpp
 * @author Ollie
 * @brief Methods to clean DEM data
 * @version 1.0.0
 * @date 2025-03-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <limits>
#include <algorithm>
#include "Map.h"

/**
 * @brief Method to remove sinks from DEM data
 */
template <typename T>
void Map<T>::fillSinks(void) {
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

    bool modified;
    do {
        modified = false;
        for (int y = 1; y < _height - 1; y++) {
            for (int x = 1; x < _width - 1; x++) {
                if (is_sink(x, y)) {
                    // Find the lowest neighbour elevation to fill the sink
                    T min_neighbor_value = std::numeric_limits<T>::infinity();
                    bool has_lower_neighbor = false;
                    for (int direction = 0; direction < 8; direction++) {
                        int nx = x + dx[direction];
                        int ny = y + dy[direction];
                        if (nx >= 0 && nx < _width && ny >= 0 && ny < _height) {
                            // Consider values greater than 0
                            if (_mapData[ny][x] > 0) {
                                min_neighbor_value = std::min(min_neighbor_value, _mapData[ny][nx]);
                                has_lower_neighbor = true;
                            }
                        }
                    }

                    // Raise sink cell to slightly greater than lowest neighbour
                    if (has_lower_neighbor && _mapData[y][x] < min_neighbor_value) {
                        _mapData[y][x] = min_neighbor_value + 1;
                        modified = true;
                    }
                }
            }
        }
    } while (modified);
}


/**
 * @brief Bool check for if cell at (x, y) is a sink.
 * Sink = all neighbours are greater
 */
template <typename T>
bool Map<T>::is_sink(int x, int y) {
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};
    
    T current_value = _mapData[y][x];
    
    for (int direction = 0; direction < 8; direction++) {
        int nx = x + dx[direction];
        int ny = y + dy[direction];
        if (nx >= 0 && nx < _width && ny >= 0 && ny < _height) {
            // If there is a neighboring cell with lower elevation, it's not a sink
            if (_mapData[ny][nx] < current_value) {
                return false;
            }
        }
    }
    return true;
}


template class Map<int>;
template class Map<float>;
template class Map<double>;
