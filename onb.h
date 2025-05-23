#ifndef ONB_H
#define ONB_H

#include <glm/glm.hpp>

class onb {
    public:
        onb(const glm::vec3& n) {
            axis[2] = glm::normalize(n);
            glm::vec3 a = std::fabs(axis[2].x) > 0.9 ? glm::vec3(0,1,0) : glm::vec3(1,0,0);
            axis[1] = glm::normalize(glm::cross(axis[2], a)); // left-handed?
            axis[0] = glm::cross(axis[2], axis[1]); // left-handed?
        }

        const glm::vec3& u() const { return axis[0]; }
        const glm::vec3& v() const { return axis[1]; }
        const glm::vec3& w() const { return axis[2]; }

        glm::vec3 transform(const glm::vec3& v) const {
            return v.x * axis[0] + v.y * axis[1] + v.z * axis[2];
        }

    private:
        glm::vec3 axis[3]; // This can just be a matrix
};

#endif