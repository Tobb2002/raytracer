#include "../scene.hpp"
#include "../objects/mesh.hpp"
#include "../objects/sphere.hpp"
#include "../objects/plane.hpp"

Scene get_scene() {
  Scene scene = Scene(vec3(0, 50, 100));

  vec3 origin = vec3(0, 0, -10);

  // --- setup scene ---
  scene.get_camera()->set_resolution(1000, 333);
  scene.get_camera()->set_sensor_size(1, 0.333);

  scene.set_aliasing(4);
  scene.set_tonemapping_value(-1);

  // --- add lights ---
  Pointlight light1 = Pointlight(origin + vec3(-12, 2, 14), 250);


  scene.add_light(light1);
  

  // --- add objects ---
  std::vector<Sphere> spheres;
  //spheres.reserve(5);
  for (int i = 0; i < 4; i++) {
    float spec = (float)i * 0.4;
    Sphere s = Sphere(origin + vec3(-3.75,0,0) +  vec3(i * 2.5, 0, 0),
                      1,
                      {.color = vec3(1, 0, 0),
                       .specular = spec,
                      b
                       .pow_m = 14});
  scene.add_object(s);

  }

  return scene;
}
