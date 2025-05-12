#ifndef SPHERE_H
#define SPHERE_H

#include "rtweekend.h"

#include "aabb.h"
#include "hittable.h"

class sphere : public hittable {
    public:
        // Stationary
        sphere(const glm::vec3& static_center, float radius, std::shared_ptr<material> mat)
         : center(static_center, glm::vec3(0)), radius(std::fmax(0,radius)), mat(mat) 
        {
            // glm::vec3 rvec(radius);
            // bbox = aabb(static_center - rvec, static_center + rvec);
            bbox = aabb(static_center - radius, static_center + radius);
        }
        // Moving
        sphere(const glm::vec3& center1, const glm::vec3& center2, float radius, std::shared_ptr<material> mat)
         : center(center1, center2 - center1), radius(std::fmax(0,radius)), mat(mat) 
        {
            aabb box1(center1 - radius, center1 + radius);
            aabb box2(center2 - radius, center2 + radius);
            bbox = aabb(box1, box2);
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // Ray origin to current sphere center
            glm::vec3 current_center = center.at(r.time());
            glm::vec3 oc = current_center - r.origin();
            
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
            glm::vec3 outward_normal = (rec.p - current_center) / radius;
            rec.set_face_normal(r, outward_normal);
            get_sphere_uv(outward_normal, rec.u, rec.v);
            rec.mat = mat;
            
            return true;
        }

        aabb bounding_box() const override { return bbox; }

    private:
        ray center;
        float radius;
        std::shared_ptr<material> mat;
        aabb bbox;

        static void get_sphere_uv(const glm::vec3& p, double& u, double& v) {
            // Converts a point p on the unit sphere to uv coordinates

            double phi = std::atan2(-p.z, p.x) + pi;
            double theta = std::acos(-p.y);

            u = phi / (2 * pi);
            v = theta / pi;
        }
    };

#endif