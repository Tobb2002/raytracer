#include "../scene.hpp"
#include "../objects/mesh.hpp"
#include "../objects/sphere.hpp"
#include "../objects/plane.hpp"

Scene get_scene() {

  // // Setup Scene
  // Setup Scene
  Scene scene = Scene(vec3(0, 50, 100));

  vec3 origin = vec3(-6, 0, 0);
  vec3 origin_plane = vec3(0, -2, -18);
  //Mesh m = Mesh("data/input/skull.obj",
  //    vec3(0, 15, -20), {.color = vec3(0, 1, 0)});
  Mesh m = Mesh("data/input/bunny_smooth.obj",
      origin_plane + vec3(0, -0.8, 0), {.color = vec3(0, 1, 0), .specular=0.2});
  Sphere earth = Sphere(origin_plane + vec3(0, 0, 0), 1,{.color = vec3(1, 0, 1), .specular=0.2}, "data/input/earth_uv.png");
  //Sphere venus = Sphere(origin + vec3(4, 0, 0), 1.1,{.color = vec3(1, 0, 1), .specular=0.2}, "data/input/textures/planets/venus.jpg");
  //Sphere mars = Sphere(origin + vec3(9, 0, 0), 0.5,{.color = vec3(1, 0, 1), .specular=0.2}, "data/input/textures/planets/mars.jpg");
  //Sphere sun = Sphere(origin + vec3(-16, 0, 0), 15,{.color = vec3(1, 0, 1), .specular=0.2}, "data/input/textures/planets/sonne.jpg");
  //Mesh c1 = Mesh("data/input/cube.obj",
  //    origin_plane + vec3(0, 0, 0), {.color = vec3(1, 0, 0), .mirror = 0.3});

  Plane plane = Plane(origin_plane, vec3(0, 1, 0),
      {.color = vec3(1 , 1, 1), .mirror = 0.3},
      {.color = vec3(0.6, 0.6, 0.6), .mirror = 0.3});

  plane.set_axis(true);
  // Plane plane2 = Plane(vec3(0, 0, -50), vec3(0, 0, -1), vec3(0, 0.5, 1));
  Pointlight light = Pointlight(origin + vec3(0, 4, 0), 300);
  Pointlight light1 = Pointlight(origin_plane + vec3(-3, 5, 4), 250);
  Pointlight light2 = Pointlight(vec3(-6, 8, 1), 150);

  //scene = Scene(vec3(0, 50, 100));

  //std::cout << "Scene size:" << m.get_size() << std::endl;

 
  scene.get_camera()->set_resolution(300);
  scene.get_camera()->set_sensor_size(1, 1);

  scene.set_aliasing(1);
  scene.set_tonemapping_value(-1);

  scene.add_light(light1);

  //scene.add_object(m); 
  scene.add_object(plane);
  scene.add_object(earth);


  scene.get_camera()->move(vec3(0, 7, 0));
  scene.get_camera()->rotate(origin, vec3(1, 0, 0), -15);
  scene.update_view_transform();

  return scene;
}

