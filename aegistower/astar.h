#pragma once
#include <sgg/graphics.h>
#include <vector>
#include "types.h"

namespace Astar {

	std::vector<int> findPath(
		int startNodeId,
		int endNodeId,
		const std::vector<PathNode>& nodes,
		const std::vector<Tower>& towers
	);



	float distance(Point a, Point b);

	float getTowerWeight(
		
		const PathNode& from,
		const PathNode& to ,
		const std::vector<Tower>& towers);


}

