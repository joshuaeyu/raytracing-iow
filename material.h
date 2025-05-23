#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"
#include "onb.h"
#include "pdf.h"
#include "texture.h"

class scatter_record {
    public:
        glm::vec3 attenuation;
        std::shared_ptr<pdf> pdf_ptr;
        bool skip_pdf;
        ray skip_pdf_ray;
};

class material {
    public:
        virtual ~material() = default;

        virtual glm::vec3 emitted(const ray& r_in, const hit_record& rec, double u, double v, const glm::vec3& p) const {
            return glm::vec3(0);
        }

        virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const {
            return false;
        }

        virtual double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) {
            // Note on PDFs and Monte Carlo integration: 
            // - PDFs are used to more efficiently compute integrals. You generate some value
            // then divide based on the PDF value of the value generated. This effectively 
            // weights (normalizes) the value based on how much or how little it is sampled.
            // - Values that are sampled less frequently are weighted higher. Values that are sampled
            // frequently are weighted lower.
            // - The PDF p used is independent of the integrand f, but the closer that p approximates
            // f, the faster the computation will converge. (The intuition is that we want to
            // steer samples to values which more strongly contribute to the integral.)
            // - Perfect importance sampling is only possible when f can be integrated analytically. 
            // Then if p = f, the CDF obtained through integration and the corresponding ICD are "perfect"
            // matches for f.
            // - The CDF is obtained by analytically integrating the PDF, and the inverse of the CDF 
            // is the ICD, which takes a uniform input and outputs a value according to the distribution
            // defined by the CDF (and thus the PDF).
            return 0;
        }
};

class lambertian : public material {    // Diffuse/matte material
    public:
        lambertian(const glm::vec3& albedo)
         : tex(std::make_shared<solid_color>(albedo)) {}
        
        lambertian(std::shared_ptr<texture> tex)
         : tex(tex) {}

        bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
            srec.attenuation = tex->value(rec.u, rec.v, rec.p);
            srec.pdf_ptr = std::make_shared<cosine_pdf>(rec.normal); // Cosine sampling
            srec.skip_pdf = false;
            return true;
        }

        double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) override {
            double cos_theta = glm::dot(rec.normal, glm::normalize(scattered.direction()));
            return cos_theta >= 0 ? cos_theta/pi : 0;
        }

    private:
        // glm::vec3 albedo;   // Whiteness, or fractional reflectance
        std::shared_ptr<texture> tex;   // Whiteness, or fractional reflectance
};

class metal : public material {
    public:
        metal(const glm::vec3& albedo, float fuzz)
         : albedo(albedo), fuzz(fuzz) {}

        bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
            // Return attenuation and scattered ray through corresponding args
            glm::vec3 reflect_direction = glm::reflect(r_in.direction(), rec.normal);
            reflect_direction = glm::normalize(reflect_direction) + fuzz * random_unit_vector();

            srec.attenuation = albedo;
            srec.pdf_ptr = nullptr;
            srec.skip_pdf = true;
            srec.skip_pdf_ray = ray(rec.p, reflect_direction, r_in.time());

            return true;
            // Return only if ray is scattered away from surface   
            // return (glm::dot(scattered.direction(), rec.normal) > 0);
        }

    private:
        glm::vec3 albedo;   // Whiteness, or fractional reflectance
        float fuzz;
};

class dielectric : public material {
    public:
        dielectric(float refraction_index)
         : refraction_index(refraction_index) {}

        bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
            // Set attenuation
            srec.attenuation = glm::vec3(1, 1, 1); // Glass; doesn't absorb anything.
            srec.pdf_ptr = nullptr;
            srec.skip_pdf = true;

            // Set scattered as refract or reflect
            float ri = rec.front_face ? 1.0f/refraction_index : refraction_index;   // Front means air->dielectric, back means dielectric->air
            glm::vec3 unit_direction = glm::normalize(r_in.direction());
            // float cos_theta = fmin(glm::dot(-unit_direction, rec.normal), 1.0);
            float cos_theta = glm::dot(-unit_direction, rec.normal);
            float sin_theta = glm::sqrt(1.0f - cos_theta*cos_theta);
            
            // First condition: Reflect if refraction is impossible, i.e., if sin_theta must be greater than 1.0.
            // Second condition: Reflect if Shlick reflectance (a function of the angle between the ray and the surface normal) is greater than some arbitrary value.
            glm::vec3 scatter_direction;
            if (ri * sin_theta > 1.0 || reflectance(cos_theta, ri) > random_float()) {
                scatter_direction = glm::reflect(unit_direction, rec.normal);
            } else {
                scatter_direction = glm::refract(unit_direction, rec.normal, ri);
            }
            srec.skip_pdf_ray = ray(rec.p, scatter_direction, r_in.time());
            
            return true;
        }

    private:
        float refraction_index;

        static float reflectance(float cosine, float refraction_index) {
            // Shlick's approximation - refractive index changes depending on angle between ray and normal, causing more reflection than otherwise
            float r0 = (1.0f - refraction_index) / (1.0f + refraction_index);
            r0 = r0*r0;
            return r0 + (1.0f-r0) * std::pow((1.0f-cosine), 5);
        }
};

class diffuse_light : public material {
    public:
        diffuse_light(std::shared_ptr<texture> tex) 
         : tex(tex) {}
        
        diffuse_light(const glm::vec3& emit)
         : tex(std::make_shared<solid_color>(emit)) {}

        glm::vec3 emitted(const ray& r_in, const hit_record& rec, double u, double v, const glm::vec3& p) const override {
            // Only emit light from the front face
            if (!rec.front_face)
                return glm::vec3(0);
            return tex->value(u, v, p);
        }

    private:
        std::shared_ptr<texture> tex;
};

class isotropic : public material {
    public:
        isotropic(const glm::vec3& albedo)
         : tex(std::make_shared<solid_color>(albedo)) {}
        
        isotropic(std::shared_ptr<texture> tex)
         : tex(tex) {}

        bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
            srec.attenuation = tex->value(rec.u, rec.v, rec.p);
            srec.pdf_ptr = std::make_shared<sphere_pdf>(); // Uniform sphere sampling
            srec.skip_pdf = false;
            return true;
        }

        double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) override {
            return 1 / (4 * pi);
        }
    
    private:
        std::shared_ptr<texture> tex;
};

#endif
