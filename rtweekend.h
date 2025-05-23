#ifndef RTWEEKEND_H
#define RTWEEKEND_H

// #include <cstdlib>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <random>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/constants.hpp>

const double infinity = std::numeric_limits<double>::infinity();
const double pi = glm::pi<double>();

inline double random_double() {
    static std::default_random_engine generator(1);
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(generator);
}

inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

inline float random_float() {
    static std::default_random_engine generator(2);
    static std::uniform_real_distribution<float> distribution(0.0, 1.0);
    return distribution(generator);
}

inline float random_float(float min, float max) {
    return min + (max - min) * random_float();
}

inline float random_int(int min, int max) {
    static std::default_random_engine generator(4);
    static std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

inline glm::vec3 random_cosine_direction() {
    // Cosine sample a hemisphere
    // - Generates a vector symmetric about the z-axis and according to a cosine distribution of the 
    // angle against the z-axis in the positive hemisphere
    double r1 = random_double();
    double r2 = random_double();

    double phi = 2 * pi * r1;
    double x = std::cos(phi) * std::sqrt(r2);
    double y = std::sin(phi) * std::sqrt(r2);
    double z = std::sqrt(1 - r2);

    return glm::vec3(x, y, z);
}

#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "interval.h"

#endif