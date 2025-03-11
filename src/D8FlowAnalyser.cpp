#include "D8FlowAnalyser.h"
#include <iostream>

// Constructor from Map object
template <typename T>
D8FlowAnalyser<T>::D8FlowAnalyser(Map<T>& map) : elevationData(map){
    if (map.getHeight() == 0 || map.getWidth() == 0) {
        std::cerr << "Map cannot be empty." << std::endl;
    }
    flowDirections = Map<int>(map.getWidth(), map.getHeight());
}

template <typename T>
void D8FlowAnalyser<T>::analyseFlow(void) {
    int width = elevationData.getWidth();
    int height = elevationData.getHeight();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            analyseFlowAt(x, y);
        }
    }
}

template <typename T>
Map<int> D8FlowAnalyser<T>::getMap(void) {
    return flowDirections;
}

template <typename T>
void D8FlowAnalyser<T>::analyseFlowAt(int x, int y) {
    T currentValue = elevationData.getData(x, y);

    // 8 Directions about cell
    int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

    // Initial variables for search
    int bestX = x, bestY = y;
    T lowestValue = currentValue;
    int bestDirection = -1;

    for (int dir = 0; dir < 8; dir++) {
        int nx = x + dx[dir];
        int ny = y + dy[dir];

        // Inbounds check
        if (nx >= 0  && nx < elevationData.getWidth() && ny >= 0  && ny < elevationData.getHeight()) {
           T neighbourValue = elevationData.getData(nx, ny);
           
            if (neighbourValue < lowestValue) {
                lowestValue = neighbourValue;
                bestX = nx;
                bestY = ny;
                bestDirection = dir;
            }
            else if (neighbourValue == lowestValue) {
                
                if (rand() % 2 == 0) {
                    bestX = nx;
                    bestY = ny;
                    bestDirection = dir;
                }
            }
        }

    }
    if (bestDirection != -1) {
        flowDirections.setData(x, y, bestDirection);
    }
    else {
        flowDirections.setData(x, y, -1);
    }
    
    
}

template class D8FlowAnalyser<int>;
template class D8FlowAnalyser<float>;
template class D8FlowAnalyser<double>;