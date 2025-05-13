#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

#include "hittable.h"
#include "hittable_list.h"
#include "material.h"

class camera {

    public:
        float aspect_ratio    = 1.0;
        int image_width       = 100;
        int samples_per_pixel = 10;     // For antialiasing
        int max_depth         = 10;     // Ray bounce limit
        glm::vec3 background;           // Scene background color
        float acne_bound      = 0.001;  // For shadow acne fixing

        float vfov = 90;    // Vertical field of view, entire span

        glm::vec3 lookfrom = glm::vec3(0,0,0);
        glm::vec3 lookat   = glm::vec3(0,0,-1);
        glm::vec3 vup      = glm::vec3(0,1,0);

        float defocus_angle = 10.0;    // Cone angle, entire span
        float focus_dist    = 3.4;

        void render(const hittable& world) {
            auto t0 = std::chrono::steady_clock::now();
            
            initialize();
            // Render
            std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
            for (int j = 0; j < image_height; j++) {
                std::clog << "\rScanlines remaining: " << image_height-j-1 << ' ' << std::flush;
                for (int i = 0; i < image_width; i++) {
                    glm::vec3 pixel_color = glm::vec3(0);
                    // Antialiasing
                    for (int sample = 0; sample < samples_per_pixel; sample++) {
                        ray r = get_ray(i,j);  // aims at viewport
                        pixel_color += ray_color(r, max_depth, world);
                    }
                    // glm::vec3 pixel_center = pixel00_loc + (float(i) * pixel_delta_u) + (float(j) * pixel_delta_v);
                    // glm::vec3 ray_direction = pixel_center - camera_center;
                    // ray r(camera_center, ray_direction);
                    write_color(std::cout, pixel_color * pixel_samples_scale);
                }
            }
            std::clog << "\nDone!\n";

            auto t1 = std::chrono::steady_clock::now();
            std::chrono::duration<double, std::chrono::minutes::period> dur = t1 - t0;
            std::clog << "Total render time: " << std::fixed << std::setprecision(3) << dur.count() << " min" << std::endl;
        }

    private:
        int       image_height;
        float     pixel_samples_scale;  // For antialiasing
        glm::vec3 camera_center;
        glm::vec3 pixel00_loc;
        glm::vec3 pixel_delta_u;
        glm::vec3 pixel_delta_v;
        
        glm::vec3 u, v, w;  // u = right, v = up, w = backwards (-w = forward)
        
        glm::vec3 defocus_disk_u;
        glm::vec3 defocus_disk_v;

        void initialize() {
            // Image dimensions
            image_height = image_width / aspect_ratio;
            image_height = (image_height < 1) ? 1 : image_height;

            pixel_samples_scale = 1.0f / samples_per_pixel;

            camera_center = lookfrom;

            // Viewport dimensions
            // float focal_length = glm::length(lookat - lookfrom);
            float theta = glm::radians(vfov); // vfov in radians
            float h = glm::tan(theta/2); // vfov in units
            float viewport_height = 2 * h * focus_dist; // vfov scaled by focus_dist
            float viewport_width = viewport_height * (float(image_width)/float(image_height));

            // Orthonormal basis vectors
            w = glm::normalize(lookfrom - lookat);
            u = glm::normalize(glm::cross(-w, vup));
            v = glm::cross(u, -w);

            // V_u and V_v vectors (since y is inverted in 3D space vs when drawing image)
            // Viewport is in the 3D space, whereas the image is the final image itself
            glm::vec3 viewport_u = viewport_width * u;
            glm::vec3 viewport_v = viewport_height * -v;
            
            // viewport pixel count = image pixel count
            pixel_delta_u = viewport_u / float(image_width);
            pixel_delta_v = viewport_v / float(image_height);

            // Upper left pixel location (in 3D space)
            glm::vec3 viewport_upper_left = camera_center - focus_dist*w - viewport_u/2.0f - viewport_v/2.0f;
            pixel00_loc = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);

            // Camera defocus disk basis vectors
            float defocus_radius = glm::tan(glm::radians(defocus_angle/2)) * focus_dist;
            defocus_disk_u = u * defocus_radius;
            defocus_disk_v = v * defocus_radius;
        }
        
        ray get_ray(int i, int j) const {
            // Ray 
            // - From: Defocus disk surrounding camera_center
            // - To: Random point near pixel (i, j) (within (i,j)±(0.5,0.5))
            glm::vec3 offset = sample_square();
            glm::vec3 pixel_sample = pixel00_loc + ((float(i) + offset.x) * pixel_delta_u) + ((float(j) + offset.y) * pixel_delta_v);
            
            glm::vec3 ray_origin = (defocus_angle <= 0) ? camera_center : defocus_disk_sample();
            glm::vec3 ray_direction = pixel_sample - ray_origin;
            double ray_time = random_double();

            return ray(ray_origin, ray_direction, ray_time);
        }

        glm::vec3 sample_square() const {
            // Random point within [-0.5,-0.5] and [0.5,0.5]
            return glm::vec3(random_float() - 0.5f, random_float() - 0.5f, 0);
        }

        glm::vec3 sample_disk() const {
            float x = glm::cos( glm::two_pi<float>() * random_float() ) - 0.5f;
            float y = glm::sin( glm::two_pi<float>() * random_float() ) - 0.5f;
            return glm::vec3(x, y, 0);
        }

        glm::vec3 defocus_disk_sample() const {
            glm::vec3 p = random_in_unit_disk();
            return camera_center + p.x*defocus_disk_u + p.y*defocus_disk_v;
        }
        
        glm::vec3 ray_color(const ray& r, int depth, const hittable& world) const {
            if (depth <= 0)
                return glm::vec3(0,0,0);
            
            // Get closest hit record
            hit_record rec;
            if (!world.hit(r, interval(acne_bound,infinity), rec)) {
                return background;
                // // Gradient
                // glm::vec3 unit_direction = glm::normalize(r.direction());
                // float a = 0.5f*(unit_direction.y + 1.0f);   // take [-1,1] to [0,1]
                // return (1-a)*glm::vec3(1,1,1) + a*glm::vec3(0.5,0.7,1);
            }

            // Return emission if material doesn't scatter this ray
            ray scattered;
            glm::vec3 attenuation;
            glm::vec3 color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);
            if (!rec.mat->scatter(r, rec, attenuation, scattered))
                return color_from_emission;
                
            // Get color of incoming ray which we are tracing backward, and attenuate it based on scatter properties
            glm::vec3 color_from_scatter = attenuation * ray_color(scattered, depth-1, world);
            
            return color_from_emission + color_from_scatter;
        }
};

#endif
