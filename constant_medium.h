#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "hittable.h"
#include "material.h"
#include "texture.h"

class constant_medium : public hittable {
    public:
        constant_medium(std::shared_ptr<hittable> boundary, double density, std::shared_ptr<texture> tex)
         : boundary(boundary), neg_inv_density(-1.0/density), phase_function(std::make_shared<isotropic>(tex)) {}
         
        constant_medium(std::shared_ptr<hittable> boundary, double density, const glm::vec3& albedo)
         : boundary(boundary), neg_inv_density(-1.0/density), phase_function(std::make_shared<isotropic>(albedo)) {}
        
        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // ----- 1. Determine and enforce volume bounds -----
            hit_record rec1, rec2;
            
            // Test if ray hits boundary at all
            // - Negative t allowed in case ray origin is inside volume
            if (!boundary->hit(r, interval::universe, rec1))
                return false;
            
            // Test if ray hits boundary again after the first hit
            // - Should always happen unless the ray hits an extremely narrow section of
            // the volume (e.g., edges)
            if (!boundary->hit(r, interval(rec1.t+0.0001, infinity), rec2))
                return false;
            
            // Conform (via clamping) rec1.t and rec2.t to ray_t
            if (rec1.t < ray_t.min) 
                rec1.t = ray_t.min; // For cases where the ray originates from inside the volume
            if (rec2.t > ray_t.max) 
                rec2.t = ray_t.max; // For cases where an object closer than the other end of the volume has been hit already
            
            // Exit if clamping results in nonsensical bounds
            // - if rec1.t and rec2.t are both negative such that after clamping to ray_t, rec1.t > rec2.t
            // - if ray_t.max < rec1.t
            if (rec1.t >= rec2.t) 
                return false;
            
            // Clamp rec1.t to 0 (only relevant if ray_t.min < 0, which doesn't occur in this ray tracer design)
            if (rec1.t < 0)
                rec1.t = 0;

            // ----- 2. Probabilistically scatter the ray -----
            float ray_length = glm::length(r.direction());
            double distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
            double hit_distance = neg_inv_density * std::log(random_double()); // Scatter distance
            
            // Exit if scattered ray exceeds boundary
            if (hit_distance > distance_inside_boundary)
                return false;

            rec.t = rec1.t + hit_distance / ray_length;
            rec.p = r.at(rec.t);
            rec.normal = glm::vec3(1,0,0);  // arbitrary
            rec.front_face = true;          // arbitrary
            rec.mat = phase_function;

            return true;
        }

        aabb bounding_box() const override { return boundary->bounding_box(); }

    private:
        std::shared_ptr<hittable> boundary; 
        double neg_inv_density; // Scales the probability of scattering
        std::shared_ptr<material> phase_function;

};

#endif