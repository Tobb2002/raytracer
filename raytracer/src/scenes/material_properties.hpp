#include "../scene.hpp"
#include "../objects/mesh.hpp"
#include "../objects/sphere.hpp"
#include "../objects/plane.hpp"

Scene get_scene() {
  Scene scene = Scene(vec3(0, 50, 100));

  vec3 origin = vec3(0, 0, -3);

  // --- setup scene ---
  scene.get_camera()->set_resolution(500, 500);
  scene.get_camera()->set_sensor_size(1, 1);

  scene.set_aliasing(1);
  scene.set_tonemapping_value(-1);

  // --- add lights ---
  Pointlight light1 = Pointlight(origin + vec3(-1.5, 2, 4), 250);


  scene.add_light(light1);
  

  // --- add objects ---
  std::vector<Sphere> spheres;
  //spheres.reserve(5);
  //for (int i = 0; i < 4; i++) {
  //  float spec = (float)i * 0.2;
    Sphere s = Sphere(origin + vec3(0, 0, 0) + vec3(0, 0, 0),
                      1,
                      {.color = vec3(1, 0, 1),
                       .specular = 1});
  scene.add_object(s);
  //  spheres.push_back(s);

    //scene.add_object(s);
  //}
  //scene.add_object(spheres.at(0));
  //scene.add_object(spheres.at(1));


  return scene;
}
