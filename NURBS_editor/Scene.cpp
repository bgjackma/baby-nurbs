#include "Scene.h"
#include "NURBS.h"

Scene::Scene()
:surface(new NURBS<4>(5, 5))
{

}


void Scene::DrawWireframe()
{
   surface->DrawControlMesh();
   surface->DrawSelectedPoints();
   surface->DrawSurface();
}

Scene::~Scene()
{
}

void Scene::ResetWeights()
{
   surface->ResetWeights();
}


void Scene::ResizeCurve(int u, int v)
{
   surface->SetSize(u, v, u * 0.2f, v * 0.2f);
}


void Scene::SetSelectedWeight(float weight)
{
   surface->SetSelectedWeight(weight);
}

float Scene::GetSelectedWeight() const
{
   return surface->GetSelectedWeight();
}

void Scene::SetSelectedPos(const glm::vec3& p)
{
   surface->SetSelectedPos(p);
}

glm::vec3 Scene::GetSelectedPos() const
{
   return surface->GetSelectedPos();
}

void Scene::SelectOnRay(Ray ray, float cutoff)
{
   surface->SelectOnRay(ray, cutoff);
   glutPostRedisplay();
}


void Scene::MoveSelected(glm::vec3 displacement)
{
   surface->MoveSelected(displacement);
   glutPostRedisplay();
}

void Scene::Recalculate()
{
   surface->RecalculateSurface();
}