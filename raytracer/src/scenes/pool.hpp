#include "../scene.hpp"
#include "../objects/sphere.hpp"
#include "../objects/plane.hpp"

#include <string.h>

Scene get_scene() {
  Scene scene = Scene(vec3(0, 50, 100));

  vec3 origin = vec3(0, 0, -12);

  Plane plane = Plane(origin + vec3(0, -1,0), vec3(0, 1, 0),
      {.color = vec3(0.2 , 1, 0.2),.specular=0, .mirror = 0.0});

  scene.get_camera()->set_resolution(400, 400);
  scene.get_camera()->set_sensor_size(1, 1);

  scene.set_aliasing(1);
  scene.set_tonemapping_value(-1);

  // --- add lights ---
  Pointlight light1 = Pointlight(origin + vec3(-12, 2, 14), 250);


  scene.add_light(light1);
  

  // --- add objects ---
  std::vector<Sphere> spheres;
  //spheres.reserve(5);
  scene.add_object(plane);
  //scene.add_object(b1);
  //scene.add_object(b2);
  //scene.add_object(b3);

  uint count = 0;
  char texture[40];
  int seq[15] = {1, 9, 2, 3, 4, 10, 11, 7, 14, 4, 5, 13, 15, 6, 12};
  // textures gehen nicht: 8

  for (int i = 1; i <= 5; i++) {
    for (int j = 0; j < i; j++) {
      vec3 pos = origin + vec3(0, 0, 0) + vec3(j*2 - i , 0, -1.73 * i);
        sprintf(texture, "data/input/textures/pool/ball%i.png", seq[count]);
          Sphere b = Sphere(pos,
                        1,
                        {.color = vec3(1, 0, 1), .specular = 0.6, .mirror=0.3},
                        texture);
          // rotate ball randomly
          int rand = 359 * (double) std::rand() / RAND_MAX;
          std::cout << "rand: " << rand << "\n";
          b.rotate(vec3(rand,rand,rand),rand);

          scene.add_object(b);
      count ++;
    }

  }

  scene.get_camera()->move(vec3(0, 4, 0));
  scene.get_camera()->rotate(origin, vec3(1, 0, 0), -15);
  scene.get_camera()->rotate(origin, vec3(0, 1, 0), 30);
  scene.update_view_transform();
  return scene;
}
