/*
 * Copyright (c) 2023 Tobias Vonier. All rights reserved.
 */

#include "pointlight.hpp"

Pointlight::Pointlight(vec3 position, vec3 color) : Light(position, color) {}

Pointlight::Pointlight(vec3 position, float intensity)
    : Light(position, intensity) {}
