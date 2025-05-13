#ifndef QUAD_H
#define QUAD_H

#include "aabb.h"
#include "hittable.h"

class quad : public hittable {
    public:
        quad(const glm::vec3& Q, const glm::vec3& u, const glm::vec3& v, std::shared_ptr<material> mat)
         : Q(Q), u(u), v(v), mat(mat)
        {
            glm::vec3 n = glm::cross(u, v);
            normal = glm::normalize(n);
            D = glm::dot(normal, Q);
            w = n / glm::dot(n, n);
            
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
        
    private:
        glm::vec3 Q;
        glm::vec3 u, v;
        glm::vec3 w;
        std::shared_ptr<material> mat;
        aabb bbox;
        glm::vec3 normal;
        double D;
};

#endif