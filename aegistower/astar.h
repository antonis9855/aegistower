#pragma once
#include <vector>
#include "types.h"
// A* pathfinding sto graph
namespace Astar {
    std::vector<int> findPath(
        int startNodeId,
        int endNodeId,
        const std::vector<PathNode>& nodes,
        float randomWeight = 0.0f
    );
    float distance(Point a, Point b);
    float heuristic(const PathNode& from, const PathNode& to);
    void updateNodeWeights(
        std::vector<PathNode>& nodes,
        const std::vector<Tower*>& towers
    );
}
