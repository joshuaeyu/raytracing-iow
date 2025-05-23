#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include <vector>

#include "rtweekend.h"
#include "hittable.h"

class hittable_list : public hittable {
    public:
        std::vector<std::shared_ptr<hittable>> objects;

        hittable_list() {}
        hittable_list(std::shared_ptr<hittable> object) { add(object); }

        void clear(){
            objects.clear();
        }

        void add(std::shared_ptr<hittable> object) {
            objects.push_back(object);
            bbox = aabb(bbox, object->bounding_box()); // Recompute bounding box
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            hit_record temp_rec;
            bool hit_anything = false;
            float closest_so_far = ray_t.max;

            // Check hit for each object, decreasing tmax of the interval such that only objects
            // that are closer than closest_so_far are hit
            for (const auto& object : objects) {
                if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
                    // If object was hit (i.e., it was closer than closest_so_far), update closest_so_far and save its hit record
                    // hit_record contains vec3 p, vec4 normal, material mat, float t, bool front_face
                    hit_anything = true;
                    closest_so_far = temp_rec.t;
                    rec = temp_rec;
                }
            }

            return hit_anything;
        }

        aabb bounding_box() const override { return bbox; }

        double pdf_value(const glm::vec3& origin, const glm::vec3& direction) const override {
            double weight = 1.0 / objects.size();
            double sum = 0.0;

            for (const auto& object : objects)
                sum += object->pdf_value(origin, direction);
            
            return sum * weight;
        }

        glm::vec3 random(const glm::vec3& origin) const override {
            int size = objects.size();
            return objects[random_int(0, size-1)]->random(origin);
        }
    private:
        aabb bbox;
};

#endif
