#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtweekend.h"
#include "aabb.h"

class material;

class hit_record {
    public: 
        glm::vec3 p;
        glm::vec3 normal;
        std::shared_ptr<material> mat;
        double t;
        double u;
        double v;
        bool front_face;

        void set_face_normal(const ray& r, const glm::vec3& outward_normal) {
            // NOTE: Assumes outward_normal has length 1.
            front_face = (glm::dot(r.direction(), outward_normal) < 0);
            normal = front_face ? outward_normal : -outward_normal;
        }
};

class hittable {
    public:
        virtual ~hittable() = default;  

        virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;

        virtual aabb bounding_box() const = 0;

        virtual double pdf_value(const glm::vec3& origin, const glm::vec3& direction) const {
            return 0;
        }

        virtual glm::vec3 random(const glm::vec3& origin) const {
            return glm::vec3(1,0,0);
        }
};

class translate : public hittable {
    public:
        translate(std::shared_ptr<hittable> object, const glm::vec3& offset)
         : object(object), offset(offset) 
        {  
            bbox = object->bounding_box() + offset; // + operator is overloaded and acts as a displacement
        }
        
        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // Temporarily offset the ray by -offset and test intersection with the offset ray to simulate
            // a translation of the object by +offset, then undo the offset when returning
            // - This is essentially temporarily transforming the ray from world space to object space
            ray offset_r(r.origin() - offset, r.direction(), r.time());
            
            if (!object->hit(offset_r, ray_t, rec))
                return false;

            rec.p += offset;

            return true;
        }
        
        aabb bounding_box() const override { return bbox; }
    
    private:
        std::shared_ptr<hittable> object;
        glm::vec3 offset;
        aabb bbox;
};

class rotate_y : public hittable {
    public:
        rotate_y(std::shared_ptr<hittable> object, double angle)
         : object(object)
        {
            double radians = glm::radians(angle);
            cos_theta = std::cos(radians);
            sin_theta = std::sin(radians);
            bbox = object->bounding_box();

            // Compute the aabb of the rotated object by rotating each corner of the current bounding box 
            // and updating the min and max coordinates if they are exceeded
            glm::vec3 min(infinity, infinity, infinity);
            glm::vec3 max(-infinity, -infinity, -infinity);
            for (int i = 0; i < 2; i++)
                for (int j = 0; j < 2; j++)
                    for (int k = 0; k < 2; k++) {
                        float x = i*bbox.x.max + (1-i)*bbox.x.min;
                        float y = j*bbox.y.max + (1-j)*bbox.y.min;
                        float z = k*bbox.z.max + (1-k)*bbox.z.min;

                        float newx = cos_theta * x - sin_theta * z;
                        float newz = sin_theta * x + cos_theta * z;

                        glm::vec3 tester(newx, y, newz);

                        for (int c = 0; c < 3; c++) {
                            min[c] = std::fmin(min[c], tester[c]);
                            max[c] = std::fmax(max[c], tester[c]);
                        }
                    }
            
            bbox = aabb(min, max);
        }
        
        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // Temporarily rotate the ray by -angle and test intersection with the rotated ray to simulate
            // a rotation of the object by +angle, then undo the rotation when returning
            // - This is essentially temporarily transforming the ray from world space to object space
            glm::vec3 origin(
                cos_theta * r.origin().x - sin_theta * r.origin().z,
                r.origin().y,
                sin_theta * r.origin().x + cos_theta * r.origin().z
            );
            glm::vec3 direction(
                cos_theta * r.direction().x - sin_theta * r.direction().z,
                r.direction().y,
                sin_theta * r.direction().x + cos_theta * r.direction().z
            );
            ray rotated_r(origin, direction, r.time());

            if (!object->hit(rotated_r, ray_t, rec))
                return false;

            rec.p = glm::vec3(
                cos_theta * rec.p.x + sin_theta * rec.p.z,
                rec.p.y,
                -(sin_theta * rec.p.x) + cos_theta * rec.p.z
            );
            rec.normal = glm::vec3(
                cos_theta * rec.normal.x + sin_theta * rec.normal.z,
                rec.normal.y,
                -(sin_theta * rec.normal.x) + cos_theta * rec.normal.z
            );

            return true;
        }
        
        aabb bounding_box() const override { return bbox; }
    
    private:
        std::shared_ptr<hittable> object;
        double cos_theta;
        double sin_theta;
        aabb bbox;
};

#endif