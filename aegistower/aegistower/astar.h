#pragma once
#include <vector>
#include "types.h"

namespace Astar {
    std::vector<int> findPath(int startNodeId, int endNodeId, const std::vector<PathNode>& nodes, float randomWeight);
    float distance(Point a, Point b);
    float heuristic(const PathNode& from, const PathNode& to);
    void updateNodeWeights(std::vector<PathNode>& nodes, const std::vector<Tower*>& towers);
}
