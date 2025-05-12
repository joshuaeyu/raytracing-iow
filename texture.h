#ifndef TEXTURE_H
#define TEXTURE_H

#include "perlin.h"
#include "rtw_stb_image.h"

#include <glm/glm.hpp>

class texture {
    public:
        virtual ~texture() = default;

        virtual glm::vec3 value(double u, double v, const glm::vec3& p) const = 0;
};

class solid_color : public texture {
    public:
        solid_color(const glm::vec3& albedo)
         : albedo(albedo) {}
        
        solid_color(double red, double green, double blue)
         : solid_color(glm::vec3(red, green, blue)) {}
        
        glm::vec3 value(double u, double v, const glm::vec3& p) const override {
            // Value is independent of u, v, and p
            return albedo; 
        }

    private:
        glm::vec3 albedo;
};

class checker_texture : public texture {
    public:
        checker_texture(double scale, std::shared_ptr<texture> even, std::shared_ptr<texture> odd)
         : inv_scale(1.0 / scale), even(even), odd(odd) {}

        checker_texture(double scale, const glm::vec3& color1, const glm::vec3& color2)
         : checker_texture(scale, std::make_shared<solid_color>(color1), std::make_shared<solid_color>(color2)) {}
        
        glm::vec3 value(double u, double v, const glm::vec3& p) const override {
            // A type of solid (spatial) texture, where value is only dependent on p

            int x_int = int(std::floor(inv_scale * p.x));
            int y_int = int(std::floor(inv_scale * p.y));
            int z_int = int(std::floor(inv_scale * p.z));

            bool is_even = (x_int + y_int + z_int) % 2 == 0;

            return is_even ? even->value(u, v, p) : odd->value(u, v, p);
        }

    private:
        double inv_scale; // inverse of scale = frequency
        std::shared_ptr<texture> even;
        std::shared_ptr<texture> odd;
};

class image_texture : public texture {
    public:
        image_texture(const char* filename)
         : image(filename) {}
        
        glm::vec3 value(double u, double v, const glm::vec3& p) const override {
            if (image.height() <= 0) 
                return glm::vec3(0,1,1); // Cyan for debugging

            // Clamp uv to u = [0,1] and v = [1,0]
            u = interval(0,1).clamp(u);
            v = 1.0 - interval(0,1).clamp(v);

            int i = int(u * image.width());
            int j = int(v * image.height());
            const unsigned char* pixel = image.pixel_data(i, j);

            return glm::vec3(pixel[0], pixel[1], pixel[2]) / 255.0f;
        }

    private:
        rtw_image image;
};

class noise_texture : public texture {
    public:
        noise_texture(double scale) 
         : scale(scale) {}
        
        glm::vec3 value(double u, double v, const glm::vec3& p) const override {
            // Direct turbulence
            // double noise_val = noise.turb(scale * glm::dvec3(p), 7);
            
            // Phased turbulence (marbling)
            // - Color is proportional to a sine function
            //   - Frequency: Scale * p.z
            //   - Phase: 10 * noise.turb
            double noise_val = 0.5 * (1.0 + std::sin(scale * p.z + 10 * noise.turb(p, 7)));
            
            return glm::vec3(noise_val);
        }

    private:
        perlin noise;
        double scale;
};

#endif