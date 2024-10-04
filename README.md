<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->
<a id="readme-top"></a>
<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Don't forget to give the project a star!
*** Thanks again! Now go create something AMAZING! :D
-->



<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![Contributors][contributors-shield]][contributors-url]
[![Creative Commons License][license-shield]][license-url]



<!-- PROJECT LOGO -->
<br />
<h3 align="center">A simple Ray Tracer</h3>

  <p align="center">
    This project was build for my Bachelorthesis at the Albert-Ludwigs-Univerty
Freiburg.
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li><a href="#features">Features</a></li>
    <li><a href="#license">License</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project
This is a implementation of a simple raytracer.
<div align="center">
    <img src="images/bistro.png" alt="Logo">
</div>

Model downloaded from [Amazon Lumberyard Bistro, Open Research Content Archive (ORCA)](http://developer.nvidia.com/orca/amazon-lumberyard-bistro).

TODO: description
### Built With
The following libraries where used to build the project:
* [![glm][glm-shield]][glm-url]
* [![cimg][cimg-shield]][cimg-url]
* [![boost][boost-shield]][boost-url]
* [![tiny_obj_loader][tiny_obj_loader-shield]][tiny_obj_loader-url]

Other tools used for the project:
* [![cclint][cclint-shield]][cclint-url] style checker
* [![cloc][cloc-shield]][cloc-url] getting code statistics
* [![compiledb][compiledb-shield]][compiledb-url] generating compile commands

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- FEATURES -->
## Features
### Supported Objects:
  - Spheres, Planes and Triangles
  - Triangle meshes (in the .obj format) 
- Textures (read from .mtl file)

### Shading:
To realistically represent materials this implementation uses the simple Phong
shading. It can handle Difusse, Specular and Mirroring Materials.

  - Diffuse specular and Mirroring materials.

### Intersection Acceleration:
To efficiently calculate ray-primitive intersections my implementation uses one
of the following methods:
- Bounding Volume Hierarchies with:
  - Surface Area Heuristic (SAH)
  - Linear Bounding Volume Hierarhies (LBVH)
  - Hierarichal Linear Bounding Volumes (HLBVH)
- Uniform Grid

<p align="right">(<a href="#readme-top">back to top</a>)</p>





<!-- LICENSE -->
## License

Distributed under the Creative Commons license [CC BY-NC 4.0][license-url].

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/Tobb2002/raytracer.svg?style=for-the-badge
[contributors-url]: https://github.com/Tobb2002/raytracer/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/Tobb2002/raytracer.svg?style=for-the-badge
[forks-url]: https://github.com/Tobb2002/raytracer/network/members
[stars-shield]: https://img.shields.io/github/stars/Tobb2002/raytracer.svg?style=for-the-badge
[stars-url]: https://github.com/Tobb2002/raytracer/stargazers
[issues-shield]: https://img.shields.io/github/issues/Tobb2002/raytracer.svg?style=for-the-badge
[issues-url]: https://github.com/Tobb2002/raytracer/issues
[license-shield]: https://img.shields.io/badge/Licence-CC_BY--NC_4.0-red?style=for-the-badge
[license-url]: https://creativecommons.org/licenses/by-nc/4.0/


<!-- USED LIBRARYS & TOOLS -->
[glm-shield]: https://img.shields.io/badge/OpenGL_Mathematics-orange.svg?
[glm-url]: https://glm.g-truc.net/0.9.9
[cimg-url]: https://cimg.eu/
[cimg-shield]: https://img.shields.io/badge/The_CImg_Library-gray.svg?
[boost-url]: https://www.boost.org/doc/libs/1_31_0/libs/lambda/doc/lambda_docs_as_one_file.html
[boost-shield]: https://img.shields.io/badge/The_Boost_Lambda_Library-green.svg?
[tiny_obj_loader-url]: https://github.com/tinyobjloader/tinyobjloader
[tiny_obj_loader-shield]: https://img.shields.io/badge/Tinyobjloader-red.svg?

[cclint-url]: https://github.com/ollix/cclint
[cclint-shield]: https://img.shields.io/badge/cclint-red.svg?
[compiledb-url]: https://pypi.org/project/compiledb/
[compiledb-shield]: https://img.shields.io/badge/compiledb-blue.svg?
[cloc-url]: https://github.com/AlDanial/cloc
[cloc-shield]: https://img.shields.io/badge/cloc-red.svg?
