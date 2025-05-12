#ifndef BVH_H
#define BVH_H

#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"

#include <algorithm>    

class bvh_node : public hittable {
    public:
        bvh_node(hittable_list list) : bvh_node(list.objects, 0, list.objects.size()) {}

        bvh_node(std::vector<std::shared_ptr<hittable>>& objects, size_t start, size_t end) 
        {
            // BVH is most effective when the objects are divided into two lists well,
            // i.e., such that the two lists (children) have smaller bounding boxes than
            // the current BVH (parent)

            // Naive method: Split on random axis
            // int axis = random_int(0, 2);

            // Slightly better method: Split on longest axis
            bbox = aabb::empty;
            for (size_t object_index = start; object_index < end; object_index++)
                bbox = aabb(bbox, objects[object_index]->bounding_box());
            int axis = bbox.longest_axis();

            auto comparator = (axis = 0) ? box_x_compare
                            : (axis = 1) ? box_y_compare
                                         : box_z_compare;

            size_t object_span = end - start;

            if (object_span == 1) {
                left = right = objects[start];
            } else if (object_span == 2) {
                left = objects[start];
                right = objects[start + 1];
            } else {
                // Sort by ascending min axis interval bound
                std::sort(objects.begin() + start, objects.begin() + end, comparator);

                size_t mid = start + object_span/2;
                left = std::make_shared<bvh_node>(objects, start, mid);
                right = std::make_shared<bvh_node>(objects, mid, end);
            }

            bbox = aabb(left->bounding_box(), right->bounding_box()); // Only if splitting on random axis
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            if (!bbox.hit(r, ray_t))
                return false;

            // Call hit() on hittable instead of aabb because we need to return hit info in rec
            bool hit_left = left->hit(r, ray_t, rec);
            // Slight optimization for second hit check if a hit occurred in hit_left
            // by reducing max of interval to check
            bool hit_right = right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec); 

            return hit_left || hit_right;
        }

        aabb bounding_box() const override { return bbox; }

    private:
        std::shared_ptr<hittable> left;
        std::shared_ptr<hittable> right;
        aabb bbox;

        static bool box_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b, int axis_index) {
            // Compare based on interval min bound
            interval a_axis_interval = a->bounding_box().axis_interval(axis_index);
            interval b_axis_interval = b->bounding_box().axis_interval(axis_index);
            return a_axis_interval.min < b_axis_interval.min;
        }

        static bool box_x_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b) {
            return box_compare(a, b, 0);
        }

        static bool box_y_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b) {
            return box_compare(a, b, 1);
        }

        static bool box_z_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b) {
            return box_compare(a, b, 2);
        }
};

#endif