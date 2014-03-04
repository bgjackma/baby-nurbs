#include "Camera.h"
#include <cmath>
#include <glm/glm.hpp>
#include <iostream>

Ray PerspectiveCamera::ClickRay(int mouseX, int mouseY) const
{
   using namespace glm;
   //get coordinates in clipping plane
   float width = (float)glutGet(GLUT_WINDOW_WIDTH);
   float height = (float)glutGet(GLUT_WINDOW_HEIGHT);
   vec4 clipCoords = vec4(2 * mouseX / width - 1.0f,
      1.0f - 2 * mouseY / height, 0, 1);

   mat4x4 pMatrix;
   //get coordinates in view space
   glGetFloatv(GL_PROJECTION_MATRIX, &pMatrix[0][0]);
   vec4 viewCoords = inverse(pMatrix) * clipCoords;
   viewCoords /= viewCoords.w;
   mat4x4 mMatrix;
   glGetFloatv(GL_MODELVIEW_MATRIX, &mMatrix[0][0]);
   vec4 worldCoords = inverse(mMatrix) * viewCoords;
   vec3 direction(worldCoords.x, worldCoords.y, worldCoords.z);
   direction = normalize(direction - GetPosition());
   return Ray(GetPosition(), direction);
}


PerspectiveCamera::PerspectiveCamera(int window, double theta, double phi, double r, double fov)
: Camera(window), theta(theta), phi(phi), r(r), fov(fov)
{
   SetPosition(glm::vec3(r * sin(phi) * cos(theta), r * cos(phi), r * sin(phi) * sin(theta)));
   int prevWindow = glutGetWindow();
   glutSetWindow(window);
   aspect = ((float)glutGet(GLUT_WINDOW_WIDTH)) / glutGet(GLUT_WINDOW_HEIGHT);
   Activate();
   glutSetWindow(prevWindow);
}

void PerspectiveCamera::Activate()
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(fov, aspect, 0.1, 1000);
   gluLookAt(GetX(), GetY(), GetZ(), 0, 0, 0, 0, 1, 0);
   glMatrixMode(GL_MODELVIEW);
}


void PerspectiveCamera::Rotate(int dx, int dy) 
{ 	
   theta += dx * 0.003f;
   phi += dy * 0.003f;
   ReloadPosition();
}

void PerspectiveCamera::ReloadPosition()
{

   SetPosition(glm::vec3(r * sin(phi) * cos(theta), r * cos(phi), r * sin(phi) * sin(theta)));
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(fov, aspect, 0.1, 1000);
   gluLookAt(GetX(), GetY(), GetZ(), 0, 0, 0, 0, 1, 0);
   glMatrixMode(GL_MODELVIEW);
   glutPostRedisplay();
}

glm::vec3 PerspectiveCamera::InPlaneMovement(int, int) const
{ return glm::vec3(0, 0, 0); }



OrthoCamera::OrthoCamera(int window, double h, Direction direct)
: Camera(window), height(h), d(direct)
{
   int prevWindow = glutGetWindow();
   glutSetWindow(window);
   aspect = ((float)glutGet(GLUT_WINDOW_WIDTH)) / glutGet(GLUT_WINDOW_HEIGHT);
   Activate();
   glutSetWindow(prevWindow);
}
void OrthoCamera::Activate()
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   float distance = 10;
   glOrtho(-aspect * height / 2, aspect * height / 2, -height / 2, height / 2, -100, 100);
   switch (d)
   {
   case X:
      glRotated(90, 0, 1, 0);
      glTranslated(distance, 0, 0);
      SetPosition(glm::vec3(distance, 0, 0));
      break;
   case Y:
      glRotated(90, 1, 0, 0);
      glTranslated(0, distance, 0);
      SetPosition(glm::vec3(0, distance, 0));
      break;
   case Z:
      glTranslated(0, 0, distance);
      SetPosition(glm::vec3(0, 0, distance));
      break;
   }
   glMatrixMode(GL_MODELVIEW);
}


void OrthoCamera::Rotate(int dx, int dy)
{

}
void OrthoCamera::ReloadPosition()
{

}

Ray OrthoCamera::ClickRay(int mouseX, int mouseY) const
{
   using namespace glm;
   //get coordinates in view space
   float winWidth = (float)glutGet(GLUT_WINDOW_WIDTH);
   float winHeight = (float)glutGet(GLUT_WINDOW_HEIGHT);
   vec3 viewCoords = vec3(mouseX / winWidth - 0.5f,
      0.5f - mouseY / winHeight, 0);
   viewCoords.x *= height * aspect;
   viewCoords.y *= height;
   //ortho matrices are weird, just gonna hack this
   vec3 origin(0,0,0);
   vec3 direction(0, 0, 0);
   switch (d)
   {
   case Direction::X:
      direction.x = -1;
      origin.x = GetX();
      origin.y = viewCoords.y;
      origin.z = viewCoords.x;
      break;
   case Direction::Y:
      direction.y = -1;
      origin.x = viewCoords.x;
      origin.y = GetY();
      origin.z = viewCoords.y;
      break;
   case Direction::Z:
      origin.x = viewCoords.x;
      origin.y = viewCoords.y;
      origin.z = GetZ();
      direction.z = -1;
      break;
   default:
   }
   return Ray(origin, direction);
}


glm::vec3 OrthoCamera::InPlaneMovement(int dx, int dy) const
{
   using namespace glm;
   //get coordinates in view space
   float winWidth = (float)glutGet(GLUT_WINDOW_WIDTH);
   float winHeight = (float)glutGet(GLUT_WINDOW_HEIGHT);
   float deltaX = (dx / winWidth) * height * aspect;
   float deltaY = (-dy / winHeight) * height;
   switch (d)
   {
   case Direction::X:
      return vec3(0, deltaY, deltaX);
   case Direction::Y:
      return vec3(deltaX, 0, -deltaY);
   case Direction::Z:
      return vec3(deltaX, deltaY, 0);
   default:
      return vec3(0, 0, 0);
   }

}