#ifndef FLOWACCUMULATOR_H
#define FLOWACCUMULATOR_H

#include "Map.h"
#include "SobelAnalysis.h"
#include <cmath>
#include <vector>

template <typename elevationT, typename D8T, typename DinfT>
class FlowAccumulator {
public:
    FlowAccumulator(const Map<elevationT>& elevation, 
                    const Map<DinfT>* aspect = nullptr, 
                    const Map<DinfT>* G = nullptr, 
                    const Map<D8T>* D8 = nullptr);

    Map<elevationT> accumulateFlow(const std::string& method);

private:
    const Map<elevationT>& elevationMap;
    const Map<DinfT>* aspectMap;  
    const Map<DinfT>* gradientMap;      
    const Map<D8T>* D8Map;

    int width, height;

    void accumulateD8(Map<elevationT>& flowMap);
    void accumulateDinf(Map<elevationT>& flowMap);
    void accumulateMDF(Map<elevationT>& flowMap);
    std::tuple<std::array<int, 2>, std::array<int, 2>, double, double> getNearestTwoDirections(double theta);
};

#endif
