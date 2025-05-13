#ifndef COLOR_H
#define COLOR_H

#include "interval.h"

#include <glm/glm.hpp>

inline float linear_to_gamma(float linear_component) {
    // Map linear space to gamma space (which is what most image viewers are assuming) using gamma 2 approximation
    if (linear_component > 0)
        return glm::sqrt(linear_component);

    return 0;
}

void write_color(std::ostream& out, const glm::vec3& pixel_color) {
    // Should be [0,1]
    auto r = pixel_color.r;
    auto g = pixel_color.g;
    auto b = pixel_color.b;

    // Gamma correction
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Map [0,1] to [0,255]
    static const interval intensity(0.000f, 0.999f);
    int rbyte = 256 * intensity.clamp(r);
    int gbyte = 256 * intensity.clamp(g);
    int bbyte = 256 * intensity.clamp(b);

    out << rbyte << ' ' << gbyte << ' ' << bbyte << std::endl;
}

#endif