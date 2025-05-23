#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"
#include "hittable_list.h"

class quad : public hittable {
    public:
        quad(const glm::vec3& Q, const glm::vec3& u, const glm::vec3& v, std::shared_ptr<material> mat)
         : Q(Q), u(u), v(v), mat(mat)
        {
            glm::vec3 n = glm::cross(u, v);
            normal = glm::normalize(n);
            D = glm::dot(normal, Q);
            w = n / glm::dot(n, n);
            
            area = glm::length(n);

            set_bounding_box();
        }

        virtual void set_bounding_box() {
            aabb bbox_diagonal1 = aabb(Q, Q + u + v);
            aabb bbox_diagonal2 = aabb(Q + u, Q + v);
            bbox = aabb(bbox_diagonal1, bbox_diagonal2);
        }
        
        aabb bounding_box() const override { return bbox; }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // Ray is parallel to plane
            double denom = glm::dot(normal, r.direction());
            if (std::fabs(denom) < 1e-8)
                return false;

            // Intersection is outside ray interval of interest
            double t = (D - glm::dot(normal, r.origin())) / denom;
            if (!ray_t.contains(t))
                return false;

            glm::vec3 intersection = r.at(t);
            glm::vec3 planar_hitpt_vector = intersection - Q; // "p"
            double alpha = glm::dot(w, glm::cross(planar_hitpt_vector, v));
            double beta = glm::dot(w, glm::cross(u, planar_hitpt_vector));

            // Intersection is not within the quad
            if (!is_interior(alpha, beta, rec))
                return false;

            rec.t = t;
            rec.p = intersection;
            rec.set_face_normal(r, normal);
            rec.mat = mat;
            
            return true;
        }

        // Use this function to define other 2D primitives (e.g., triangles, ellipses, annuli)
        virtual bool is_interior(double a, double b, hit_record& rec) const {
            interval unit_interval(0, 1);
            if (!unit_interval.contains(a) || !unit_interval.contains(b))
                return false;
            
            // uv aren't necessarily perpendicular; currently parallel to quad edges
            rec.u = a;
            rec.v = b;

            return true;
        }

        double pdf_value(const glm::vec3& origin, const glm::vec3& direction) const override {
            // Get intersection of ray with quad
            hit_record rec;
            if (!hit(ray(origin, direction), interval(0.001, infinity), rec))
                return 0;
            
            // Compute PDF value
            double dist_squared = glm::distance2(origin, rec.p);
            // double dist_squared = rec.t * rec.t * glm::length2(direction);
            double cosine = std::fabs(glm::dot(direction, normal)) / glm::length(direction);
            
            return dist_squared / (cosine * area);
        }

        glm::vec3 random(const glm::vec3& origin) const override {
            // Vector from origin to random point on quad
            glm::vec3 p = Q + (random_float() * u) + (random_float() * v);
            return p - origin;
        }

    private:
        glm::vec3 Q;
        glm::vec3 u, v;
        glm::vec3 w;
        std::shared_ptr<material> mat;
        aabb bbox;
        glm::vec3 normal;
        double D;
        double area;
};

inline std::shared_ptr<hittable_list> box(const glm::vec3& a, const glm::vec3& b, std::shared_ptr<material> mat) {
    std::shared_ptr<hittable_list> sides = std::make_shared<hittable_list>();

    glm::vec3 min(std::fmin(a.x, b.x), std::fmin(a.y, b.y), std::fmin(a.z, b.z));
    glm::vec3 max(std::fmax(a.x, b.x), std::fmax(a.y, b.y), std::fmax(a.z, b.z));

    glm::vec3 dx(max.x - min.x, 0, 0);
    glm::vec3 dy(0, max.y - min.y, 0);
    glm::vec3 dz(0, 0, max.z - min.z);

    sides->add(std::make_shared<quad>(glm::vec3(min.x, min.y, max.z), dx, dy, mat)); // front
    sides->add(std::make_shared<quad>(glm::vec3(max.x, min.y, max.z),-dz, dy, mat)); // right
    sides->add(std::make_shared<quad>(glm::vec3(max.x, min.y, min.z),-dx, dy, mat)); // back
    sides->add(std::make_shared<quad>(glm::vec3(min.x, min.y, min.z), dz, dy, mat)); // left
    sides->add(std::make_shared<quad>(glm::vec3(min.x, max.y, max.z), dx,-dz, mat)); // top
    sides->add(std::make_shared<quad>(glm::vec3(min.x, min.y, min.z), dx, dz, mat)); // bottom

    return sides;
}

#endif