#include "point.hpp"


point::point(float x2, float y2, int32_t id2) {
    x = x2;
    y = y2;
    id = id2;
}

point::point() {
    x = 0.0f;
    y = 0.0f;
    id = -1;
}
