#pragma once
#include <cstdint>

struct point {
    float x, y;
    int32_t id;

    point(float x2, float y2, int32_t id2); 
    point();
};

inline point middle(point const & p1, point const & p2) {
    return {(p1.x + p2.x) * 0.5f, (p1.y + p2.y) * 0.5f, -1};
}