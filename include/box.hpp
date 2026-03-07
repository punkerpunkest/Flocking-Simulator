#pragma once
#include <limits>
#include "point.hpp"    
#include <algorithm>
static constexpr float inf = std::numeric_limits<float>::infinity();

struct box {
    point min {inf, inf, -1};
    point max {-inf, -inf, -1};
    box& operator |= (point const& p);
};


template <typename Iterator>
box bbox(Iterator begin, Iterator end) {
    box result; //could not return copy here ?
    for (auto it = begin; it != end; ++it)
        result |= *it;
    return result;
}