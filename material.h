#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"

class hit_record;

class material {
    public:
        virtual ~material() = default;

        virtual bool scatter(const ray& r_in, const hit_record& rec, glm::vec3& attenuation, ray& scattered) const {
            return false;
        }
};

class lambertian : public material {    // Diffuse/matte material
    public:
        lambertian(const glm::vec3& albedo)
         : albedo(albedo) {}

        bool scatter(const ray& r_in, const hit_record& rec, glm::vec3& attenuation, ray& scattered) const override {
            // Return attenuation and scattered ray through corresponding args
            glm::vec3 scatter_direction = rec.normal + random_unit_vector();
            
            if (near_zero(scatter_direction)) {
                scatter_direction = rec.normal;
            }
            
            scattered = ray(rec.p, scatter_direction, r_in.time()); // Preserve time in scattered ray
            attenuation = albedo;
            return true;
        }

    private:
        glm::vec3 albedo;   // Whiteness, or fractional reflectance
};

class metal : public material {
    public:
        metal(const glm::vec3& albedo, float fuzz)
         : albedo(albedo), fuzz(fuzz) {}

        bool scatter(const ray& r_in, const hit_record& rec, glm::vec3& attenuation, ray& scattered) const override {
            // Return attenuation and scattered ray through corresponding args
            glm::vec3 reflect_direction = glm::reflect(r_in.direction(), rec.normal);
            reflect_direction = glm::normalize(reflect_direction) + fuzz*random_unit_vector();
            scattered = ray(rec.p, reflect_direction, r_in.time()); // Preserve time in scattered ray
            attenuation = albedo;
            // Return only if ray is scattered away from surface   
            return (glm::dot(scattered.direction(), rec.normal) > 0);
        }

    private:
        glm::vec3 albedo;   // Whiteness, or fractional reflectance
        float fuzz;
};

class dielectric : public material {
    public:
        dielectric(float refraction_index)
         : refraction_index(refraction_index) {}

        bool scatter(const ray& r_in, const hit_record& rec, glm::vec3& attenuation, ray& scattered) const override {
            // Set attenuation
            attenuation = glm::vec3(1); // Glass; doesn't absorb anything.

            // Set scattered as refract or reflect
            float ri = rec.front_face ? 1.0f/refraction_index : refraction_index;   // Front means air->dielectric, back means dielectric->air
            glm::vec3 unit_direction = glm::normalize(r_in.direction());
            // float cos_theta = fmin(glm::dot(-unit_direction, rec.normal), 1.0);
            float cos_theta = glm::dot(-unit_direction, rec.normal);
            float sin_theta = glm::sqrt(1.0f - cos_theta*cos_theta);
            
            glm::vec3 scatter_direction;
            // First condition: Reflect if refraction is impossible, i.e., if sin_theta must be greater than 1.0.
            // Second condition: Reflect if Shlick reflectance (a function of the angle between the ray and the surface normal) is greater than some arbitrary value.
            if (ri * sin_theta > 1.0 || reflectance(cos_theta, ri) > random_float()) {
                scatter_direction = glm::reflect(unit_direction, rec.normal);
            } else {
                scatter_direction = glm::refract(unit_direction, rec.normal, ri);
            }
            scattered = ray(rec.p, scatter_direction, r_in.time()); // Preserve time in scattered ray
            return true;
        }

    private:
        float refraction_index;

        static float reflectance(float cosine, float refraction_index) {
            // Shlick's approximation - refractive index changes depending on angle between ray and normal, causing more reflection than otherwise
            float r0 = (1.0f - refraction_index) / (1.0f + refraction_index);
            r0 = r0*r0;
            return r0 + (1.0f-r0)*glm::pow((1.0f-cosine),5);
        }
};

#endif
