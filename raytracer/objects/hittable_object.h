/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#ifndef HITABLE_OBJECT_H_
#define HITABLE_OBJECT_H_

struct Intersection {
  bool found;
  vec3 point;
  vec3 normal;
  vec3 color;
};

class hitable_object {
 public:
  virtual Intersection intersect(void);

};
#endif  // HITABLE_OBJECT_H_
