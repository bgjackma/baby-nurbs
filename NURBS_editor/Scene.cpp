#include "Scene.h"

Scene::Scene()
:surface(5, 5)
{

}


void Scene::DrawWireframe()
{
   surface.DrawControlMesh();
   surface.DrawSelectedPoints();
   surface.DrawSurface();
}

Scene::~Scene()
{
}

void Scene::SelectOnRay(Ray ray, float cutoff)
{
   surface.SelectOnRay(ray, cutoff);
   glutPostRedisplay();
}


void Scene::MoveSelected(glm::vec3 displacement)
{
   surface.MoveSelected(displacement);
   glutPostRedisplay();
}

void Scene::Recalculate()
{
   surface.RecalculateSurface();
}