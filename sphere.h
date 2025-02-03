#ifndef SPHERE_H
#define SPHERE_H

#include "rtweekend.h"

#include "hittable.h"

class sphere : public hittable {
    public:
        sphere(const glm::vec3& center, float radius, std::shared_ptr<material> mat) : center(center), radius(fmax(0,radius)), mat(mat) {}

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // Ray origin to sphere center vector
            glm::vec3 oc = center - r.origin();
            
            // Solve quadratic for values of t where sphere is hit by ray, within ray_tmin and ray_tmax.
                // float a = glm::dot(r.direction(), r.direction());
                // float b = -2.0f * glm::dot(r.direction(), oc);
                // float c = glm::dot(oc, oc) - radius*radius;
                // float discriminant = b*b - 4*a*c;
                // return (-b - glm::sqrt(discriminant)) / (2.0f * a);  
            float a = glm::length2(r.direction());
            float h = glm::dot(r.direction(), oc);
            float c = glm::length2(oc) - radius*radius;
            float discriminant = h*h - a*c;
            
            // Return early if no values of t (no intersections) exist
            if (discriminant < 0) {
                return false;
            }

            // Return early if t is not between ray_tmin and ray_tmax, exclusive
            float sqrtd = glm::sqrt(discriminant);
            float root = (h - sqrtd) / a;
            if (!ray_t.surrounds(root)) {
                root = (h + sqrtd) / a;
                if (!ray_t.surrounds(root)) {
                    return false;
                }
            }
            
            // Update rec and return true
            rec.t = root;
            rec.p = r.at(root);
            rec.mat = mat;
            rec.set_face_normal(r, (rec.p - center) / radius);
            return true;
        }

    private:
        glm::vec3 center;
        float radius;
        std::shared_ptr<material> mat;
};

#endif