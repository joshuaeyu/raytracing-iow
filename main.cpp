#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"

int main() {
    hittable_list world;

    auto ground_material = std::make_shared<lambertian>(glm::vec3(0.5, 0.5, 0.5));
    world.add(std::make_shared<sphere>(glm::vec3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_float();
            glm::vec3 center(a + 0.9*random_float(), 0.2, b + 0.9*random_float());

            if ((center - glm::vec3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = glm::vec3(random_float(), random_float(), random_float());
                    sphere_material = std::make_shared<lambertian>(albedo);
                    auto center2 = center + glm::vec3(0, random_float(0,0.5), 0); // Motion blur!
                    world.add(std::make_shared<sphere>(center, center2, 0.2, sphere_material));
                    // world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = glm::vec3(random_float(0.5, 1), random_float(0.5, 1), random_float(0.5, 1));
                    auto fuzz = random_float(0, 0.5);
                    sphere_material = std::make_shared<metal>(albedo, fuzz);
                    world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = std::make_shared<dielectric>(1.5);
                    world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<dielectric>(1.5);
    world.add(std::make_shared<sphere>(glm::vec3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<lambertian>(glm::vec3(0.4, 0.2, 0.1));
    world.add(std::make_shared<sphere>(glm::vec3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<metal>(glm::vec3(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<sphere>(glm::vec3(4, 1, 0), 1.0, material3));

    camera cam;

    // RENDER SETTINGS
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 500;
    cam.samples_per_pixel = 5;
    cam.max_depth         = 5;

    cam.vfov     = 20;
    cam.lookfrom = glm::vec3(13,2,3);
    cam.lookat   = glm::vec3(0,0,0);
    cam.vup      = glm::vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    cam.render(world);
}