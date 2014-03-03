#pragma once
#include "NURBS.h"
#include "MiscTypes.h"

class Scene
{
public:
   Scene();
   void DrawWireframe();
   void SelectOnRay(Ray ray, float cutoff);
   void MoveSelected(glm::vec3 displacement);
   void Recalculate();

   ~Scene();
private:
   NURBS<4> surface;
};

