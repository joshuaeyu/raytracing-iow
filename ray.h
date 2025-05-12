#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>

class ray {
    public:
        ray() {}

        ray(const glm::vec3 origin, const glm::vec3 direction, double time)
         : orig(origin), dir(direction), tm(time) {}

        ray(const glm::vec3 origin, const glm::vec3 direction)
         : ray(origin, direction, 0) {}

        const glm::vec3 origin() const    { return orig; }
        const glm::vec3 direction() const { return dir; }
        
        double time() const { return tm; }

        glm::vec3 at(float t) const {
            return orig + dir*t;    // May need double t and dvec3 later...
        }

    private:
        glm::vec3 orig;
        glm::vec3 dir;  // Doesn't need to be normalized, because we solve for t values at intersections analytically.
        double tm;
};

#endif