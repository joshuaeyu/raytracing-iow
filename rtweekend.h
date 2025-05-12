#ifndef RTWEEKEND_H
#define RTWEEKEND_H

// #include <cstdlib>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <random>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/constants.hpp>

const double infinity = std::numeric_limits<double>::infinity();

inline double random_double() {
    static std::mt19937 generator;
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(generator);
}

inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

inline float random_float() {
    static std::mt19937 generator;
    static std::uniform_real_distribution<float> distribution(0.0, 1.0);
    return distribution(generator);
}

inline float random_float(float min, float max) {
    return min + (max - min) * random_float();
}

#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "interval.h"

#endif