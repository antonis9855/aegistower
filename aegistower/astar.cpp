#include "astar.h"
#include <cmath>
#include <algorithm>
#include <limits>
// Ypologismoi A* me weights
namespace Astar {
    float distance(Point a, Point b) {
        float dx = b.x - a.x;
        float dy = b.y - a.y;
        return std::sqrt(dx * dx + dy * dy);
    }
    float heuristic(const PathNode& from, const PathNode& to) {
        return distance(from.position, to.position);
    }
    void updateNodeWeights(std::vector<PathNode>& nodes, const std::vector<Tower*>& towers) {
        for (auto& node : nodes) {
            node.weight = 0.0f;
            for (const auto* tower : towers) {
                float dist = distance(node.position, tower->position);
                if (dist < tower->range) {
                    node.weight += (tower->range - dist) * 2.0f;
                }
            }
        }
    }
    std::vector<int> findPath(
        int startNodeId,
        int endNodeId,
        const std::vector<PathNode>& nodes,
        float randomWeight
    ) {
        if (nodes.empty()) return {};
        int startIdx = -1, endIdx = -1;
        for (size_t i = 0; i < nodes.size(); i++) {
            if (nodes[i].id == startNodeId) startIdx = static_cast<int>(i);
            if (nodes[i].id == endNodeId) endIdx = static_cast<int>(i);
        }
        if (startIdx == -1 || endIdx == -1) return {};
        std::vector<PathNode> workNodes = nodes;
        std::vector<bool> openSet(nodes.size(), false);
        std::vector<bool> closedSet(nodes.size(), false);
        workNodes[startIdx].gCost = 0;
        workNodes[startIdx].hCost = heuristic(workNodes[startIdx], workNodes[endIdx]);
        workNodes[startIdx].parentId = -1;
        openSet[startIdx] = true;
        while (true) {
            int currentIdx = -1;
            float lowestF = std::numeric_limits<float>::max();
            for (size_t i = 0; i < workNodes.size(); i++) {
                if (openSet[i] && !closedSet[i]) {
                    float f = workNodes[i].fCost() + randomWeight * (rand() % 10) / 10.0f;
                    if (f < lowestF) {
                        lowestF = f;
                        currentIdx = static_cast<int>(i);
                    }
                }
            }
            if (currentIdx == -1) return {};
            if (currentIdx == endIdx) {
                std::vector<int> path;
                int idx = endIdx;
                while (idx != -1) {
                    path.push_back(workNodes[idx].id);
                    int parentId = workNodes[idx].parentId;
                    idx = -1;
                    if (parentId != -1) {
                        for (size_t i = 0; i < workNodes.size(); i++) {
                            if (workNodes[i].id == parentId) {
                                idx = static_cast<int>(i);
                                break;
                            }
                        }
                    }
                }
                std::reverse(path.begin(), path.end());
                return path;
            }
            openSet[currentIdx] = false;
            closedSet[currentIdx] = true;
            for (int neighborId : workNodes[currentIdx].neighbors) {
                int neighborIdx = -1;
                for (size_t i = 0; i < workNodes.size(); i++) {
                    if (workNodes[i].id == neighborId) {
                        neighborIdx = static_cast<int>(i);
                        break;
                    }
                }
                if (neighborIdx == -1 || closedSet[neighborIdx]) continue;
                float tentativeG = workNodes[currentIdx].gCost +
                    distance(workNodes[currentIdx].position, workNodes[neighborIdx].position) +
                    workNodes[neighborIdx].weight;
                if (!openSet[neighborIdx]) {
                    openSet[neighborIdx] = true;
                } else if (tentativeG >= workNodes[neighborIdx].gCost) {
                    continue;
                }
                workNodes[neighborIdx].parentId = workNodes[currentIdx].id;
                workNodes[neighborIdx].gCost = tentativeG;
                workNodes[neighborIdx].hCost = heuristic(workNodes[neighborIdx], workNodes[endIdx]);
            }
        }
    }
}
