/**
 * @file watershedAnalysis.h
 * @author Ollie
 * @brief Watershed delineation class for Map objects: flow maps, gradients maps, and aspect
 * @version 1.0.0
 * @date 2025-03-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef WATERSHEDANALYSIS_H
#define WATERSHEDANALYSIS_H

#include "../map_core/Map.h"
#include <vector>

/**
 * @brief watershed delineation class for Map object.
 * Allows determination of watershed across different algorithms.
 * 
 * Pour points are recognised by D8 and MDF categorisation.
 * 
 * Watersheds are determined from these pour points.
 * 
 * @tparam elevationT Numeric types: double, float, int
 * @tparam D8T Numeric types: double, float, int
 */
template<typename elevationT, typename D8T>
class watershedAnalysis {
public:
    /**
     * @brief Construct a new watershed Analysis object
     * 
     * @param elevation Reference to elevation map
     * @param D8 Pointer to D8 map. Used for D8 pour point and watershed analysis
     * @param flow Pointer to flow map. Used to store values in output map of watershed
     * @param slope Pointer to gradient map. Used for D infinity watershed.
     * @param aspect Pointer to aspect map. Used for D infinity watershed
     */
    watershedAnalysis(const Map<elevationT>& elevation,
                    const Map<D8T>* D8 = nullptr,
                    const Map<elevationT>* flow = nullptr,
                    const Map<elevationT>* slope = nullptr,
                    const Map<elevationT>* aspect = nullptr);
    
    /**
     * @brief Return vector of pour points.
     * 
     * @param nPoints Number of pour points to be returned in vector
     * @param method Method for pourpoint identification.
     * Accepts: "d8", "mdf"
     * 
     * D8 method looks for cells of the highest flow with neighbours that flow into them
     * or are at edge.
     * MDF method looks for cells of the highest flow with neighbours that are of 
     * greater elevation.
     * @bug It is known that pour points occur in regions of local low elevation / gradient
     * changes. Future development on this would look to combine pour points in similar 
     * neighbour hoods to create more realistic watersheds.
     *
     * @return std::vector<std::pair<int, int>> 
     */
    std::vector<std::pair<int, int>> getPourPoints(int nPoints, const std::string method);

    /**
     * @brief Delegation method for watershed identification.
     * 
     * @param Point Pour Point at location (x, y )as std::pair<x, y>
     * @param method Accepts: "mdf", "d8", "dinf"
     * @return Map<elevationT> 
     */
    Map<elevationT> calculateWatershed(std::pair<int, int> Point, const std::string method);

private:
    int _height, _width;
    const Map<elevationT>& _elevationMap;
    const Map<D8T>* _D8Map;
    const Map<elevationT>* _flowMap;
    const Map<elevationT>* _slopeMap;
    const Map<elevationT>* _aspectMap;

    /**
     * @brief Identification of pour points via D8 algorithm
     * 
     * @param nPoints Number of points to find
     * @return std::vector<std::pair<int, int>> 
     */
    std::vector<std::pair<int, int>> D8PourPoints(int nPoints);

    /**
     * @brief Identification of pour points via D infinity algorithm
     * @bug THIS IS NOT IMPLEMENTED
     * @param nPoints Number of points to find
     * @return std::vector<std::pair<int, int>> 
     */
    std::vector<std::pair<int, int>> DinfPourPoints(int nPoints);

    /**
     * @brief Identification of pour points via multi-directional flow algorithm
     * 
     * @param nPoints Number of points to find
     * @return std::vector<std::pair<int, int>> 
     */
    std::vector<std::pair<int, int>> MDFPourPoints(int nPoints);

    /**
     * @brief D8 Watershed delineation algorithm.
     * From an identified pour point, this algorithm will travel up every cell that has a D8
     * direction that 'points' into the current cell. This occurs recursively until no further
     * cells point into the current cell.
     * 
     * Values from _flowMap are stored in the output Map to indicate which regions in the watershed
     * experience the most flow
     * 
     * @param point std::pair<x, y> for pour point at (x, y)
     * @return Map<elevationT> 
     */
    Map<elevationT> D8watershed(std::pair<int, int> point);

    /**
     * @brief D8 Watershed delineation algorithm.
     * From an identified pour point, this algorithm will travel up every cell that has a D8
     * direction that 'points' into the current cell. This occurs recursively until no further
     * cells point into the current cell.
     * 
     * Values from _flowMap are stored in the output Map to indicate which regions in the watershed
     * experience the most flow
     * 
     * @param point std::pair<x, y> for pour point at (x, y)
     * @return Map<elevationT> 
     */
    Map<elevationT> DinfWatershed(std::pair<int, int> Point);

    /**
     * @brief D8 Watershed delineation algorithm.
     * From an identified pour point, this algorithm will travel up every cell that has a D8
     * direction that 'points' into the current cell. This occurs recursively until no further
     * cells point into the current cell.
     * 
     * Values from _flowMap are stored in the output Map to indicate which regions in the watershed
     * experience the most flow
     * 
     * @param point std::pair<x, y> for pour point at (x, y)
     * @return Map<elevationT> 
     */
    Map<elevationT> MDFwatershed(std::pair<int, int> point);

    /**
     * @brief Find nearest two directions in 3x3 grid for a given aspect.
     * Aspect = 0 indicates North, 90 -> East, etc..
     * 
     * @param aspect Aspect at cell in aspect map
     * @return std::pair<std::array<int, 2>, std::array<int, 2>> Two 1x2 directions.
     */
    std::pair<std::array<int, 2>, std::array<int, 2>> getNearestTwoDirections(double aspect);

};

#endif