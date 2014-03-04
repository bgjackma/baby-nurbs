#pragma once
#include "MiscTypes.h"
#include <memory>
#include <glm/vec3.hpp>

template <int D>
class NURBS;

//originally a class to contain any objects I might want to place in the scene,
//it has moprhed into the pimpl idiom, which is OK with me because templates.
class Scene
{
public:
   Scene();
   void DrawWireframe();
   void SelectOnRay(Ray ray, float cutoff);
   void MoveSelected(glm::vec3 displacement);
   void Recalculate();
   void ResizeCurve(int u, int v);
   void SetSelectedWeight(float weight);
   float GetSelectedWeight() const;
   void SetSelectedPos(const glm::vec3& p);
   glm::vec3 GetSelectedPos() const;

   void ResetWeights();

   ~Scene();
private:
   std::unique_ptr<NURBS<4> > surface;
};

