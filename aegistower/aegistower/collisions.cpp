#include "collisions.h"
#include <cmath>

namespace Collisions {

    float distance(Point a, Point b) {
        float dx = b.x - a.x;
        float dy = b.y - a.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    bool checkOverlap(Point c1, float r1, Point c2, float r2) {
        float dist = distance(c1, c2);
        return dist < (r1 + r2);
    }
}
