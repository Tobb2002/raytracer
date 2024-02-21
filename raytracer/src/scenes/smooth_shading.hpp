#include "../scene.hpp"
#include "../objects/mesh.hpp"
#include "../objects/sphere.hpp"
#include "../objects/plane.hpp"

Scene get_scene() {
  Scene scene = Scene(vec3(0, 50, 100));

  vec3 origin_plane = vec3(0, -2, -18);

  Mesh m = Mesh("data/input/bunny_smooth.obj",
                origin_plane + vec3(0, -0.8, 0),
                {.color = vec3(0, 1, 0), .specular = 0.2});
  Sphere earth = Sphere(origin_plane + vec3(0, 0, 0),
                        1,
                        {.color = vec3(1, 0, 1), .specular = 0.2},
                        "data/input/earth_uv.png");

  Plane plane = Plane(origin_plane, vec3(0, 1, 0),
      {.color = vec3(1 , 1, 1), .mirror = 0.3},
      {.color = vec3(0.6, 0.6, 0.6), .mirror = 0.3});

  plane.set_axis(true);
  Pointlight light1 = Pointlight(origin_plane + vec3(-3, 5, 4), 250);

  scene.get_camera()->set_resolution(300);
  scene.get_camera()->set_sensor_size(1, 1);

  scene.set_aliasing(1);
  scene.set_tonemapping_value(-1);

  scene.add_light(light1);

  scene.add_object(m);
  scene.add_object(plane);


  scene.get_camera()->move(vec3(0, 7, 0));
  scene.get_camera()->rotate(origin_plane, vec3(1, 0, 0), -15);
  scene.update_view_transform();

  return scene;
}

