#pragma once
#include "types.h"

namespace Collisions {
    float distance(Point a, Point b);
    bool checkOverlap(Point c1, float r1, Point c2, float r2);
}
