#ifndef WATERSHEDANALYSIS_H
#define WATERSHEDANALYSIS_H

#include "Map.h"
#include <vector>

template<typename elevationT, typename D8T>
class watershedAnalysis {
public:
    watershedAnalysis(const Map<elevationT>& elevation,
                    const Map<D8T>* D8 = nullptr,
                    const Map<elevationT>* flow = nullptr,
                    const Map<elevationT>* slope = nullptr,
                    const Map<elevationT>* aspect = nullptr);
    
    
    std::vector<std::pair<int, int>> getPourPoints(int nPoints, const std::string method);
    Map<elevationT> calculateWatershed(std::pair<int, int> Point, const std::string method);

private:
    int height, width;
    const Map<elevationT>& elevationMap;
    const Map<D8T>* D8Map;
    const Map<elevationT>* fMap;
    const Map<elevationT>* slopeMap;
    const Map<elevationT>* aspectMap;
    std::vector<std::pair<int, int>> D8PourPoints(int nPoints);
    std::vector<std::pair<int, int>> DinfPourPoints(int nPoints);
    std::vector<std::pair<int, int>> MDFPourPoints(int nPoints);


    Map<elevationT> D8watershed(std::pair<int, int> point);
    Map<elevationT> DinfWatershed(std::pair<int, int> Point);
    Map<elevationT> MDFwatershed(std::pair<int, int> point);
    std::pair<std::array<int, 2>, std::array<int, 2>> getNearestTwoDirections(double aspect);

};

#endif