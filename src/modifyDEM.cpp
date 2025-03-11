#include <limits>
#include <algorithm>  // For std::min and std::max
#include "Map.h"

template <typename T>
void Map<T>::fillSinks(void) {
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

    int width = getWidth();
    int height = getHeight();

    bool modified;
    do {
        modified = false;
        for (int y = 1; y < height - 1; y++) {
            for (int x = 1; x < width - 1; x++) {
                if (is_sink(x, y)) {
                    // Find the lowest neighbor elevation to fill the sink
                    T min_neighbor_value = std::numeric_limits<T>::infinity();
                    bool has_lower_neighbor = false;
                    for (int direction = 0; direction < 8; direction++) {
                        int nx = x + dx[direction];
                        int ny = y + dy[direction];
                        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                            // Only consider neighbors with non-zero values (skip lakes or low-lying areas)
                            if (mapData[ny][x] > 0) {
                                min_neighbor_value = std::min(min_neighbor_value, mapData[ny][nx]);
                                has_lower_neighbor = true;
                            }
                        }
                    }

                    // If there's a valid lower neighbor, raise the current cell to just above that neighbor
                    if (has_lower_neighbor && mapData[y][x] < min_neighbor_value) {
                        mapData[y][x] = min_neighbor_value + 1;  // Raise slightly above the lowest neighbor
                        modified = true;
                    }
                }
            }
        }
    } while (modified);
}

template <typename T>
bool Map<T>::is_sink(int x, int y) {
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};
    
    T current_value = mapData[y][x];
    int width = getWidth();
    int height = getHeight();

    for (int direction = 0; direction < 8; direction++) {
        int nx = x + dx[direction];
        int ny = y + dy[direction];
        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
            // If there is a neighboring cell with lower elevation, it's not a sink
            if (mapData[ny][nx] < current_value) {
                return false;
            }
        }
    }
    return true;
}


template class Map<int>;
template class Map<float>;
template class Map<double>;
