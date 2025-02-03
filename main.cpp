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
                    world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
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
    cam.image_width       = 1200;
    cam.samples_per_pixel = 10;
    cam.max_depth         = 10;

    cam.vfov     = 20;
    cam.lookfrom = glm::vec3(13,2,3);
    cam.lookat   = glm::vec3(0,0,0);
    cam.vup      = glm::vec3(0,1,0);

    cam.defocus_angle = 0.65;
    cam.focus_dist    = 10.0;

    cam.render(world);
}

int main1() {
            
    // Materials
    std::shared_ptr<material> material_ground   = std::make_shared<lambertian>(glm::vec3(0.8, 0.8, 0.0));
    std::shared_ptr<material> material_center   = std::make_shared<lambertian>(glm::vec3(0.1, 0.2, 0.5));
    std::shared_ptr<material> material_top      = std::make_shared<dielectric>(2.0);
    std::shared_ptr<material> material_left     = std::make_shared<metal>(glm::vec3(0.8, 0.6, 0.2), 0.4);
    std::shared_ptr<material> material_right    = std::make_shared<dielectric>(1.33);
    // std::shared_ptr<material> material_right  = std::make_shared<metal>(glm::vec3(0.8, 0.6, 0.2), 1.0);
    std::shared_ptr<material> material_bubble   = std::make_shared<dielectric>(1.00/1.33);

    // World
    hittable_list world;
    world.add(std::make_shared<sphere>(glm::vec3(0,0,-1),       0.3f,   material_center));
    world.add(std::make_shared<sphere>(glm::vec3(0,-20.0,-3.3), 20.f,   material_ground));
    world.add(std::make_shared<sphere>(glm::vec3(0,0.9,-1),     0.3f,   material_top));
    world.add(std::make_shared<sphere>(glm::vec3(-0.7,0.5,-1),  0.3f,   material_left));
    world.add(std::make_shared<sphere>(glm::vec3(0.7,0.3,-1),   0.4f,   material_right));
    world.add(std::make_shared<sphere>(glm::vec3(0.7,0.3,-1),   0.3f,   material_bubble));

    // Camera
    camera cam;

    // cam.aspect_ratio        = 16.0/9.0;
    cam.aspect_ratio        = 1.0;
    cam.image_width         = 400;

    cam.samples_per_pixel   = 200;
    cam.max_depth           = 50;
    cam.acne_bound          = 0.001;
    
    cam.vfov                = 43;
    cam.lookfrom            = glm::vec3(-1.5,0.2,0.75);
    cam.lookat              = glm::vec3(0,0,-1);
    cam.vup                 = glm::vec3(0,1,0);

    cam.defocus_angle       = 5;
    cam.focus_dist          = glm::length(cam.lookfrom - cam.lookat);

    cam.render(world);
    return 0;
}