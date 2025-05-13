#ifndef AABB_H
#define AABB_H

#include "interval.h"
#include "ray.h"

class aabb {
    public:
        interval x, y, z;

        aabb() {}

        aabb(const interval& x, const interval& y, const interval& z)
         : x(x), y(y), z(z) 
        {
            pad_to_minimums();
        }

        aabb(const glm::vec3& a, const glm::vec3& b) {
            // a and b are extrema (i.e., corners) of the bounding box
            x = (a.x <= b.x) ? interval(a.x, b.x) : interval(b.x, a.x);
            y = (a.y <= b.y) ? interval(a.y, b.y) : interval(b.y, a.y);
            z = (a.z <= b.z) ? interval(a.z, b.z) : interval(b.z, a.z);

            pad_to_minimums();
        }

        aabb(const aabb& box0, const aabb& box1) {
            x = interval(box0.x, box1.x);
            y = interval(box0.y, box1.y);
            z = interval(box0.z, box1.z);
        }
        
        const interval& axis_interval(int n) const {
            if (n == 1) return y;
            if (n == 2) return z;
            return x;
        }

        bool hit(const ray& r, interval ray_t) const {
            const glm::vec3& ray_orig = r.origin();
            const glm::vec3& ray_dir = r.direction();

            for (int axis = 0; axis < 3; axis++) {
                const interval& ax = axis_interval(axis);
                const float adinv = 1.0 / ray_dir[axis]; // inverse of axis magnitude, or 1/d

                // Solve for ray-aabb intersection for this axis's interval
                float t0 = (ax.min - ray_orig[axis]) * adinv;    
                float t1 = (ax.max - ray_orig[axis]) * adinv;

                // Reuse the same ray_t for all axes to store the current min and max
                if (t0 < t1) {
                    if (t0 > ray_t.min) ray_t.min = t0;
                    if (t1 < ray_t.max) ray_t.max = t1;
                } else {
                    if (t0 < ray_t.max) ray_t.max = t0;
                    if (t1 > ray_t.min) ray_t.min = t1;
                }

                // If updating ray_t for this axis's intersections causes min > max, that means
                // this axis's intersection interval doesn't overlap with the others
                if (ray_t.max <= ray_t.min)
                    return false;
            }
            
            return true;
        }

        int longest_axis() const {
            if (x.size() > y.size())
                return x.size() > z.size() ? 0 : 2;
            else
                return y.size() > z.size() ? 1 : 2;
        }

        static const aabb empty, universe;
    
    private:
        void pad_to_minimums() {
            double delta = 0.0001;
            if (x.size() < delta) x = x.expand(delta);
            if (y.size() < delta) y = y.expand(delta);
            if (z.size() < delta) z = z.expand(delta);
        }
};

const aabb aabb::empty = aabb(interval::empty, interval::empty, interval::empty);
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);
    
aabb operator+(const aabb& bbox, const glm::vec3& offset) {
    return aabb(bbox.x + offset.x, bbox.y + offset.y, bbox.z + offset.z);
}

aabb operator+(const glm::vec3& offset, const aabb& bbox) {
    return bbox + offset;
}

#endif