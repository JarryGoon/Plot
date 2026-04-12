//
// Created by jarrygoon on 260403.
//

#include "TypeFunc.hpp"

#define SQR(x) ((x) * (x))

#include <cmath>

double distance(const ImVec2 &p1, const ImVec2 &p2) {
    return std::sqrt(SQR(p2.x - p1.x) + SQR(p2.y - p1.y));
}