/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "./ray.h"
#include "./triangle.h"
#include "./camera.h"
#include "image.h"
#include "mesh.h"


using glm::vec3;

void trace_triagle(void);
void test_obj(void);


int main(void) {
  
  // test obj loader
  //test_obj();


  std::string input_file = "input/bunny.obj";

	// Load .obj File
  Mesh m = Mesh(input_file);

  m.print_triangles();


  return 0;
}

//void test_obj(void) {
//  // Initialize Loader
//	objl::Loader Loader;
//
//  std::string input_file = "input/bunny.obj";
//
//	// Load .obj File
//	bool loadout = Loader.LoadFile(input_file);
//
//	// Check to see if it loaded
//
//	// If so continue
//	if (loadout)
//	{
//		// Go through each loaded mesh and out its contents
//		for (int i = 0; i < Loader.LoadedMeshes.size(); i++)
//		{
//			// Copy one of the loaded meshes to be our current mesh
//			objl::Mesh curMesh = Loader.LoadedMeshes[i];
//
//      // Generate all Triangles
//      int num_triangles = curMesh.Vertices.size()/3;
//      Triangle *triangle_list = new Triangle[num_triangles];
//
//      std::cout << "Generating Triangles." << std::endl;
//      int id = 0;
//      for (int j = 0; j < curMesh.Vertices.size(); j+=3) {
//          // get points of Triangle
//          vec3 p1 = vec3(curMesh.Vertices[j].Position.X, 
//                         curMesh.Vertices[j].Position.Y,
//                         curMesh.Vertices[j].Position.Z);
//          vec3 p2 = vec3(curMesh.Vertices[j + 1].Position.X, 
//                         curMesh.Vertices[j + 1].Position.Y,
//                         curMesh.Vertices[j + 1].Position.Z);
//          vec3 p3 = vec3(curMesh.Vertices[j + 2].Position.X, 
//                         curMesh.Vertices[j + 2].Position.Y,
//                         curMesh.Vertices[j + 2].Position.Z);
//          
//          std::cout << glm::to_string(p1) << std::endl;
//          
//          vec3 points[] = {p1, p2, p3};
//          triangle_list[id] = Triangle(points, vec3(200, 0, 0));
//          id++;
//      }
//
//			// Print Triangles
//      std::cout << "Triangles:" << std::endl;
//
//      delete[] triangle_list;
//		}
//	}
//	// If not output an error
//	else
//	{
//    throw std::runtime_error("Could not read file:" + input_file);
//	}

//}

void trace_triagle(void) {
  // setup scene
  vec3 points[] = {vec3(-4, -3, -1), vec3(4, -3, -1), vec3(0, 4, -1) };
  Triangle t = Triangle(points, vec3(200, 0, 0));

  int resolution[2] = {200, 200};
  Camera camera = Camera(resolution[0], resolution[1]);

  Image image = Image(resolution[0], resolution[1]);

  // iterate throug image and calulate intersection
  // between camera rays and Triangle
  for (int x = 0; x < resolution[0]; x++) {
    for (int y = 0; y < resolution[1]; y++) {
      Ray ray = camera.get_ray(vec2(x, y));

      if (t.intersect_bool(ray)) {
        image.set_pixel({x, y}, t.get_color());
      }
    }
  }

  image.write_to_file("output/triangle.ppm");

}
