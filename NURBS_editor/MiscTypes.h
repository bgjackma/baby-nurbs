#pragma once

#include <glm/vec3.hpp>

struct Ray
{
   glm::vec3 origin, direction;
   Ray(glm::vec3 origin, glm::vec3 direction)
      : origin(origin), direction(direction) {}
};