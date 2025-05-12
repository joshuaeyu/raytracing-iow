#include "rtweekend.h"

#include "bvh.h"
#include "camera.h"
#include "material.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

void bouncing_spheres() {
    hittable_list world;

    auto ground_material = std::make_shared<lambertian>(glm::vec3(0.5, 0.5, 0.5));
    auto checker_tex = std::make_shared<checker_texture>(0.32, glm::vec3(.2, .3, .1), glm::vec3(.9, .9, .9));
    auto checker_material = std::make_shared<lambertian>(checker_tex);
    world.add(std::make_shared<sphere>(glm::vec3(0,-1000,0), 1000, checker_material));

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

    world = hittable_list(std::make_shared<bvh_node>(world)); // bvh_node wraps current hittables, making it optional

    camera cam;

    // RENDER SETTINGS
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 30;
    cam.max_depth         = 10;

    cam.vfov     = 20;
    cam.lookfrom = glm::vec3(13,2,3);
    cam.lookat   = glm::vec3(0,0,0);
    cam.vup      = glm::vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    cam.render(world);
}

// For image_width = 400, samples_per_pixel = 50, max_depth = 10
    // Without BVH: 17.4 min
    // With BVH random axis split: 1.79 min
    // With BVH longest axis split: 1.79 min

void checkered_spheres() {
    hittable_list world;

    auto checker_tex = std::make_shared<checker_texture>(0.32, glm::vec3(.2, .3, .1), glm::vec3(.9, .9, .9));
    auto checker_material = std::make_shared<lambertian>(checker_tex);
    world.add(std::make_shared<sphere>(glm::vec3(0,-10,0), 10, checker_material));
    world.add(std::make_shared<sphere>(glm::vec3(0, 10,0), 10, checker_material));

    camera cam;
    
    // RENDER SETTINGS
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 30;
    cam.max_depth         = 10;

    cam.vfov     = 20;
    cam.lookfrom = glm::vec3(13,2,3);
    cam.lookat   = glm::vec3(0,0,0);
    cam.vup      = glm::vec3(0,1,0);

    cam.defocus_angle = 0;
    // cam.defocus_angle = 0.6;
    // cam.focus_dist    = 10.0;

    cam.render(world);
}

void earth() {
    auto earth_texture = std::make_shared<image_texture>("images/earthmap.jpg");
    auto earth_material = std::make_shared<lambertian>(earth_texture);
    
    auto globe = std::make_shared<sphere>(glm::vec3(0,0,0), 2, earth_material);

    camera cam;
    
    // RENDER SETTINGS
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 30;
    cam.max_depth         = 10;

    cam.vfov     = 20;
    cam.lookfrom = glm::vec3(13,2,3);
    cam.lookat   = glm::vec3(0,0,0);
    cam.vup      = glm::vec3(0,1,0);

    cam.defocus_angle = 0;
    // cam.defocus_angle = 0.6;
    // cam.focus_dist    = 10.0;

    cam.render(*globe);
}

void perlin_spheres() {
    hittable_list world;

    auto perlin_tex = std::make_shared<noise_texture>(4);
    auto perlin_material = std::make_shared<lambertian>(perlin_tex);
    world.add(std::make_shared<sphere>(glm::vec3(0,-1000,0), 1000, perlin_material));
    world.add(std::make_shared<sphere>(glm::vec3(0,2,0), 2, perlin_material));

    camera cam;
    
    // RENDER SETTINGS
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 30;
    cam.max_depth         = 10;

    cam.vfov     = 20;
    cam.lookfrom = glm::vec3(13,2,3);
    cam.lookat   = glm::vec3(0,0,0);
    cam.vup      = glm::vec3(0,1,0);

    cam.defocus_angle = 0;
    // cam.defocus_angle = 0.6;
    // cam.focus_dist    = 10.0;

    cam.render(world); 
}

int main() {
    switch (4) {
        case 1: bouncing_spheres(); break;
        case 2: checkered_spheres(); break;
        case 3: earth(); break;
        case 4: perlin_spheres(); break;
    }
}