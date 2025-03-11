#ifndef D8FLOWANALYSER_H
#define D8FLOWANALYSER_H

#include "Map.h"

template <typename T>
class D8FlowAnalyser {
public:
    // Constructor    
    D8FlowAnalyser(Map<T>& map);

    // Methods
    void analyseFlow(void);
    Map<int> getMap(void);

private:
    Map<T> elevationData;
    Map<int> flowDirections;
    
    void analyseFlowAt(int x, int y);

    
};

#endif