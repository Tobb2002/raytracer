#include "../scene.hpp"
#include "../objects/mesh.hpp"
#include "../objects/sphere.hpp"
#include "../objects/plane.hpp"

Scene get_scene() {
  Scene scene = Scene(vec3(0, 50, 100));

  vec3 origin = vec3(0, 0, -11);

  Mesh c = Mesh("data/input/cube.obj",
                origin + vec3(1.5, 1, 0),
                {.color = vec3(0, 1, 0), .specular = 0.2});
  Sphere s = Sphere(origin + vec3(-1.5,1,0),
                    1,
                    {.color = vec3(1, 0, 1),
                     .specular = 0,
                     .pow_m = 10});

  Plane plane = Plane(origin, vec3(0, 1, 0),
      {.color = vec3(1 , 1, 1),.specular=0, .mirror = 0.0},
      {.color = vec3(0.6, 0.6, 0.6),.specular=0, .mirror = 0.0}, vec2(100,50));

  Pointlight light1 = Pointlight(origin + vec3(-3, 5, 4), 250);

  scene.get_camera()->set_resolution(600);
  scene.get_camera()->set_sensor_size(1, 1);

  scene.set_aliasing(2);
  scene.set_tonemapping_value(-1);

  // square light
  uint amount = 5;
  float ssize = 0.3;
  float strength = 250;
  vec3 orig = origin + vec3(-3, 4, 5);

  for (int x = 0; x < amount ; x++) {
    for (int y = 0; y < amount; y++) {
      Pointlight l = Pointlight(orig + vec3(x * ssize, y * ssize, 0), strength / (amount * amount));
      scene.add_light(l);
    }
  }


  scene.add_object(c);
  scene.add_object(s);
  scene.add_object(plane);


  scene.get_camera()->move(vec3(0, 4, 0));
  scene.get_camera()->rotate(origin, vec3(1, 0, 0), -15);
  scene.update_view_transform();

  return scene;
}


