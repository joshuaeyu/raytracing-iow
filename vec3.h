#ifndef VEC3_H
#define VEC3_H

#include "rtweekend.h"

const bool near_zero(const glm::vec3& v) {
    float s = 1e-8;
    glm::vec3 v_abs = glm::abs(v);
    return (v.x < s) && (v.y < s) && (v.z < s);
}

inline glm::vec3 random_unit_vector() {
    while (true) {
        glm::vec3 p = glm::vec3(random_float(-1,1), random_float(-1,1), random_float(-1,1));
        if (glm::length2(p) < 1)
            return glm::normalize(p);
    }
}

inline glm::vec3 random_on_hemisphere(const glm::vec3& normal) {
    glm::vec3 on_unit_sphere = random_unit_vector();
    if (glm::dot(normal, on_unit_sphere) > 0.0)
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

inline glm::vec3 random_in_unit_disk() {
    while (true) {
        glm::vec3 p = glm::vec3(random_float(-1,1), random_float(-1,1), 0);
        if (glm::length2(p) < 1)
            return p;
    }
}

#endif