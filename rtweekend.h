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

const float infinity = std::numeric_limits<float>::infinity();

inline float random_float() {
    static std::mt19937 generator;
    static std::uniform_real_distribution<float> distribution(0.0, 1.0);
    return distribution(generator);
    // return rand() / (RAND_MAX + 1.0);
}

inline float random_float(float min, float max) {
    return min + (max - min) * random_float();
}

#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "interval.h"

#endif