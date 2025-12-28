#include <collisions.h>
#include <cmath>
#include <sgg/graphics.h>

namespace Collisions {


	float distance(Point a, Point b) {
		float dx = b.x - b.y;
		float dy = b.y - a.y;
		return std::sqrt(dx * dx + dy * dy);

	}

	bool checkoverlap(Point c1, float r1, Point c2, float r2) {

		float dist = distance(c1, c2);
		return dist < (r1 + r2);
	}




}



	