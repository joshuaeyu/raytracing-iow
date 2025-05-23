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

        double pdf_value(const glm::vec3& origin, const glm::vec3& direction) const override {
            hit_record rec;
            if (!hit(ray(origin, direction), interval(0.001, infinity), rec))
                return 0;

            // Compute solid angle of the sphere that corresponds to the sampling cone
            double dist_squared = glm::distance2(center.at(0), origin);
            double cos_theta_max = std::sqrt(1 - radius*radius / dist_squared);
            double solid_angle = 2 * pi * (1 - cos_theta_max);

            // PDF is uniform over the cone
            return 1 / solid_angle;
        }

        glm::vec3 random(const glm::vec3& origin) const override {
            glm::vec3 direction = center.at(0) - origin;
            double dist_squared = glm::length2(direction);
            onb uvw(direction); // Transform such that z-axis in object space becomes direction vector in world space
            return uvw.transform(random_to_sphere(radius, dist_squared));
        }

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

        static glm::vec3 random_to_sphere(double radius, double distance_squared) {
            // In object space 
            // Assume origin at 0,0,0 and sphere center at 0,0,d
            // Return vector from origin to a random point on the side of the sphere facing the origin, i.e., the -z side of the sphere
            
            // radius and distance_squared are only used to compute theta_max!
            double cos_theta_max = std::sqrt(1 - radius*radius / distance_squared);

            double r1 = random_double();
            double r2 = random_double();

            double z = 1 + r2 * (cos_theta_max - 1);
            double phi = 2 * pi * r1;
            double x = std::cos(phi) * std::sqrt(1 - z*z);
            double y = std::sin(phi) * std::sqrt(1 - z*z);

            return glm::vec3(x, y, z);
        }
    };

#endif