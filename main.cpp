#include "rtweekend.h"

#include "bvh.h"
#include "camera.h"
#include "constant_medium.h"
#include "material.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "quad.h"

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
    cam.background        = glm::vec3(0.70, 0.80, 1.00);

    cam.vfov     = 20;
    cam.lookfrom = glm::vec3(13,2,3);
    cam.lookat   = glm::vec3(0,0,0);
    cam.vup      = glm::vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    // cam.render(world);
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
    cam.background        = glm::vec3(0.70, 0.80, 1.00);

    cam.vfov     = 20;
    cam.lookfrom = glm::vec3(13,2,3);
    cam.lookat   = glm::vec3(0,0,0);
    cam.vup      = glm::vec3(0,1,0);

    cam.defocus_angle = 0;;

    // cam.render(world);
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
    cam.background        = glm::vec3(0.70, 0.80, 1.00);

    cam.vfov     = 20;
    cam.lookfrom = glm::vec3(13,2,3);
    cam.lookat   = glm::vec3(0,0,0);
    cam.vup      = glm::vec3(0,1,0);

    cam.defocus_angle = 0;

    // cam.render(*globe);
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
    cam.background        = glm::vec3(0.70, 0.80, 1.00);

    cam.vfov     = 20;
    cam.lookfrom = glm::vec3(13,2,3);
    cam.lookat   = glm::vec3(0,0,0);
    cam.vup      = glm::vec3(0,1,0);

    cam.defocus_angle = 0;

    // cam.render(world); 
}

void quads() {
    hittable_list world;

    // Materials
    auto left_red     = std::make_shared<lambertian>(glm::vec3(1.0, 0.2, 0.2));
    auto back_green   = std::make_shared<lambertian>(glm::vec3(0.2, 1.0, 0.2));
    auto right_blue   = std::make_shared<lambertian>(glm::vec3(0.2, 0.2, 1.0));
    auto upper_orange = std::make_shared<lambertian>(glm::vec3(1.0, 0.5, 0.0));
    auto lower_teal   = std::make_shared<lambertian>(glm::vec3(0.2, 0.8, 0.8));

    // Quads
    world.add(std::make_shared<quad>(glm::vec3(-3,-2, 5), glm::vec3(0, 0,-4), glm::vec3(0, 4, 0), left_red));
    world.add(std::make_shared<quad>(glm::vec3(-2,-2, 0), glm::vec3(4, 0, 0), glm::vec3(0, 4, 0), back_green));
    world.add(std::make_shared<quad>(glm::vec3( 3,-2, 1), glm::vec3(0, 0, 4), glm::vec3(0, 4, 0), right_blue));
    world.add(std::make_shared<quad>(glm::vec3(-2, 3, 1), glm::vec3(4, 0, 0), glm::vec3(0, 0, 4), upper_orange));
    world.add(std::make_shared<quad>(glm::vec3(-2,-3, 5), glm::vec3(4, 0, 0), glm::vec3(0, 0,-4), lower_teal));

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = glm::vec3(0.70, 0.80, 1.00);

    cam.vfov     = 80;
    cam.lookfrom = glm::vec3(0,0,9);
    cam.lookat   = glm::vec3(0,0,0);
    cam.vup      = glm::vec3(0,1,0);

    cam.defocus_angle = 0;

    // cam.render(world);
}

void simple_light() {
    hittable_list world;

    auto perlin_texture = std::make_shared<noise_texture>(4);
    auto perlin_material = std::make_shared<lambertian>(perlin_texture);
    world.add(std::make_shared<sphere>(glm::vec3(0,-1000,0), 1000, perlin_material));
    world.add(std::make_shared<sphere>(glm::vec3(0,2,0), 2, perlin_material));

    auto diff_light = std::make_shared<diffuse_light>(glm::vec3(5));
    world.add(std::make_shared<sphere>(glm::vec3(2,5,2), 0.5, diff_light));
    world.add(std::make_shared<quad>(glm::vec3(3,1,-2), glm::vec3(2,0,0), glm::vec3(0,2,0), diff_light));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = glm::vec3(0,0,0);

    cam.vfov     = 20;
    cam.lookfrom = glm::vec3(26,3,6);
    cam.lookat   = glm::vec3(0,2,0);
    cam.vup      = glm::vec3(0,1,0);

    cam.defocus_angle = 0;

    // cam.render(world);
}

void cornell_box() {
    hittable_list world;

    auto red   = std::make_shared<lambertian>(glm::vec3(.65, .05, .05));
    auto white = std::make_shared<lambertian>(glm::vec3(.73, .73, .73));
    auto green = std::make_shared<lambertian>(glm::vec3(.12, .45, .15));
    auto light = std::make_shared<diffuse_light>(glm::vec3(15, 15, 15));

    world.add(std::make_shared<quad>(glm::vec3(555,0,0), glm::vec3(0,555,0), glm::vec3(0,0,555), green));
    world.add(std::make_shared<quad>(glm::vec3(0,0,0), glm::vec3(0,555,0), glm::vec3(0,0,555), red));
    world.add(std::make_shared<quad>(glm::vec3(343, 554, 332), glm::vec3(-130,0,0), glm::vec3(0,0,-105), light));
    world.add(std::make_shared<quad>(glm::vec3(0,0,0), glm::vec3(555,0,0), glm::vec3(0,0,555), white));
    world.add(std::make_shared<quad>(glm::vec3(555,555,555), glm::vec3(-555,0,0), glm::vec3(0,0,-555), white));
    world.add(std::make_shared<quad>(glm::vec3(0,0,555), glm::vec3(555,0,0), glm::vec3(0,555,0), white));

    // std::shared_ptr<material> aluminum = std::make_shared<metal>(glm::vec3(0.8, 0.85, 0.88), 0.0);
    std::shared_ptr<hittable> box1 = box(glm::vec3(0,0,0), glm::vec3(165,330,165), white);
    box1 = std::make_shared<rotate_y>(box1, 15);
    box1 = std::make_shared<translate>(box1, glm::vec3(265,0,295));
    world.add(box1);

    std::shared_ptr<dielectric> glass = std::make_shared<dielectric>(1.5);
    std::shared_ptr<hittable> glass_sphere = std::make_shared<sphere>(glm::vec3(190,90,190), 90, glass);
    world.add(glass_sphere);

    // std::shared_ptr<hittable> box2 = box(glm::vec3(0,0,0), glm::vec3(165,165,165), white);
    // box2 = std::make_shared<rotate_y>(box2, -18);
    // box2 = std::make_shared<translate>(box2, glm::vec3(130,0,65));
    // world.add(box2);

    // Note that this is ONLY used to steer samples toward objects we deem important
    auto empty_material = std::make_shared<material>();
    hittable_list lights;
    // Ceiling light
    lights.add(std::make_shared<quad>(glm::vec3(343,554,332), glm::vec3(-130,0,0), glm::vec3(0,0,-105), empty_material));
    // Glass sphere
    lights.add(std::make_shared<sphere>(glm::vec3(190,90,190), 90, empty_material));

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 800;
    cam.samples_per_pixel = 1000;
    cam.max_depth         = 250;
    cam.background        = glm::vec3(0,0,0);

    cam.vfov     = 40;
    cam.lookfrom = glm::vec3(278, 278, -800);
    cam.lookat   = glm::vec3(278, 278, 0);
    cam.vup      = glm::vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world, lights);
}

void cornell_smoke() {
    hittable_list world;

    auto red   = std::make_shared<lambertian>(glm::vec3(.65, .05, .05));
    auto white = std::make_shared<lambertian>(glm::vec3(.73, .73, .73));
    auto green = std::make_shared<lambertian>(glm::vec3(.12, .45, .15));
    auto light = std::make_shared<diffuse_light>(glm::vec3(7, 7, 7));

    world.add(std::make_shared<quad>(glm::vec3(555,0,0), glm::vec3(0,555,0), glm::vec3(0,0,555), green));
    world.add(std::make_shared<quad>(glm::vec3(0,0,0), glm::vec3(0,555,0), glm::vec3(0,0,555), red));
    world.add(std::make_shared<quad>(glm::vec3(113,554,127), glm::vec3(330,0,0), glm::vec3(0,0,305), light));
    world.add(std::make_shared<quad>(glm::vec3(0,555,0), glm::vec3(555,0,0), glm::vec3(0,0,555), white));
    world.add(std::make_shared<quad>(glm::vec3(0,0,0), glm::vec3(555,0,0), glm::vec3(0,0,555), white));
    world.add(std::make_shared<quad>(glm::vec3(0,0,555), glm::vec3(555,0,0), glm::vec3(0,555,0), white));

    std::shared_ptr<hittable> box1 = box(glm::vec3(0,0,0), glm::vec3(165,330,165), white);
    box1 = std::make_shared<rotate_y>(box1, 15);
    box1 = std::make_shared<translate>(box1, glm::vec3(265,0,295));

    std::shared_ptr<hittable> box2 = box(glm::vec3(0,0,0), glm::vec3(165,165,165), white);
    box2 = std::make_shared<rotate_y>(box2, -18);
    box2 = std::make_shared<translate>(box2, glm::vec3(130,0,65));

    world.add(std::make_shared<constant_medium>(box1, 0.01, glm::vec3(0,0,0)));
    world.add(std::make_shared<constant_medium>(box2, 0.01, glm::vec3(1,1,1)));

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 600;
    cam.samples_per_pixel = 50;
    cam.max_depth         = 20;
    cam.background        = glm::vec3(0,0,0);

    cam.vfov     = 40;
    cam.lookfrom = glm::vec3(278, 278, -800);
    cam.lookat   = glm::vec3(278, 278, 0);
    cam.vup      = glm::vec3(0,1,0);

    cam.defocus_angle = 0;

    // cam.render(world);
}

void final_scene(int image_width, int samples_per_pixel, int max_depth) {
    hittable_list world;
    
    // Mint green cubes at varying heights
    hittable_list boxes1;
    auto ground = std::make_shared<lambertian>(glm::vec3(0.48, 0.83, 0.53));
    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1,101);
            auto z1 = z0 + w;

            boxes1.add(box(glm::vec3(x0,y0,z0), glm::vec3(x1,y1,z1), ground));
        }
    }
    world.add(std::make_shared<bvh_node>(boxes1));

    // Diffuse white light quad
    auto light = std::make_shared<diffuse_light>(glm::vec3(7, 7, 7));
    world.add(std::make_shared<quad>(glm::vec3(123,554,147), glm::vec3(300,0,0), glm::vec3(0,0,265), light));

    // Orange sphere with motion blur
    auto center1 = glm::vec3(400, 400, 200);
    auto center2 = center1 + glm::vec3(30,0,0);
    auto sphere_material = std::make_shared<lambertian>(glm::vec3(0.7, 0.3, 0.1));
    world.add(std::make_shared<sphere>(center1, center2, 50, sphere_material));

    // Glass sphere
    world.add(std::make_shared<sphere>(glm::vec3(260, 150, 45), 50, std::make_shared<dielectric>(1.5)));
    
    // Metal sphere
    world.add(std::make_shared<sphere>(
        glm::vec3(0, 150, 145), 50, std::make_shared<metal>(glm::vec3(0.8, 0.8, 0.9), 1.0)
    ));

    // Blue subsurface reflection sphere
    auto boundary = std::make_shared<sphere>(glm::vec3(360,150,145), 70, std::make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(std::make_shared<constant_medium>(boundary, 0.2, glm::vec3(0.2, 0.4, 0.9)));
    boundary = std::make_shared<sphere>(glm::vec3(0,0,0), 5000, std::make_shared<dielectric>(1.5));
    world.add(std::make_shared<constant_medium>(boundary, .0001, glm::vec3(1,1,1)));

    // Globe
    auto emat = std::make_shared<lambertian>(std::make_shared<image_texture>("images/earthmap.jpg"));
    world.add(std::make_shared<sphere>(glm::vec3(400,200,400), 100, emat));
    auto pertext = std::make_shared<noise_texture>(0.2);
    world.add(std::make_shared<sphere>(glm::vec3(220,280,300), 80, std::make_shared<lambertian>(pertext)));

    // Random white spheres enclosed in an (imaginary) rotated cube
    hittable_list boxes2;
    auto white = std::make_shared<lambertian>(glm::vec3(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(std::make_shared<sphere>(random_vector(0,165), 10, white));
    }
    world.add(std::make_shared<translate>(
        std::make_shared<rotate_y>(
            std::make_shared<bvh_node>(boxes2), 15),
            glm::vec3(-100,270,395)
        )
    );

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.background        = glm::vec3(0,0,0);

    cam.vfov     = 40;
    cam.lookfrom = glm::vec3(478, 278, -600);
    cam.lookat   = glm::vec3(278, 278, 0);
    cam.vup      = glm::vec3(0,1,0);

    cam.defocus_angle = 0;

    // cam.render(world);
}

int main() {
    switch (7) {
        case 1:  bouncing_spheres();          break;
        case 2:  checkered_spheres();         break;
        case 3:  earth();                     break;
        case 4:  perlin_spheres();            break;
        case 5:  quads();                     break;
        case 6:  simple_light();              break;
        case 7:  cornell_box();               break;
        case 8:  cornell_smoke();             break;
        case 9:  final_scene(800, 10000, 40); break;
        case 10: final_scene(800,  1000, 40); break;
    }
}