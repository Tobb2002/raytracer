
  Sphere earth = Sphere(origin + vec3(7, 0, 0), 1,{.color = vec3(1, 0, 1), .specular=0.2}, "data/input/earth_uv.png");
  Sphere venus = Sphere(origin + vec3(4, 0, 0), 1.1,{.color = vec3(1, 0, 1), .specular=0.2}, "data/input/textures/planets/venus.jpg");
  Sphere mars = Sphere(origin + vec3(9, 0, 0), 0.5,{.color = vec3(1, 0, 1), .specular=0.2}, "data/input/textures/planets/mars.jpg");
  Sphere sun = Sphere(origin + vec3(-16, 0, 0), 15,{.color = vec3(1, 0, 1), .specular=0.2}, "data/input/textures/planets/sonne.jpg");
