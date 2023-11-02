/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#ifndef HITABLE_OBJECT_H_
#define HITABLE_OBJECT_H_

class hitable_object {
 public:
  virtual void intersect(void);
}
#endif  // HITABLE_OBJECT_H_
