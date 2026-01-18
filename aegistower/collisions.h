#pragma once
#include "types.h"
// Voithitika gia apostaseis kai overlap
namespace Collisions {
    float distance(Point a, Point b);
    bool checkOverlap(Point c1, float r1, Point c2, float r2);
    bool pointInRect(Point p, float rectX, float rectY, float width, float height);
    bool pointInCircle(Point p, Point center, float radius);
}
