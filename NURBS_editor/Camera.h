#pragma once
#include <gl/freeglut.h>
#include <glm/vec3.hpp>
#include "MiscTypes.h"

class Camera
{
public:
   Camera(int window) : windowID(window) {}
   void SwitchWindow() 
   { glutSetWindow(windowID); }
   virtual void Activate() = 0;
   virtual void ReloadPosition() = 0;
   virtual void Rotate(int dx, int dy) = 0;

   virtual Ray ClickRay(int mouseX, int mouseY) const = 0;
   virtual glm::vec3 InPlaneMovement(int dx, int dy) const = 0;

   virtual glm::vec3 GetPosition() const { return position; }
   virtual float GetX() const { return position.x; }
   virtual float GetY() const { return position.y; }
   virtual float GetZ() const { return position.z; }
   virtual void SetPosition(glm::vec3 pos) { position = pos; }

   virtual ~Camera() {}
private:
   int windowID;
   glm::vec3 position;
};

class PerspectiveCamera : public Camera
{
public:
   PerspectiveCamera(int window, double theta, double phi, double r, 
      double fov);
   void Activate();
   //rotates the camera
   void Rotate(int dx, int dy);
   //does nothing
   void ReloadPosition();
   //returns a ray projected from the camera through the point (X,Y) in the
   //projection plane
   Ray ClickRay(int mouseX, int mouseY) const;
   virtual glm::vec3 InPlaneMovement(int dx, int dy) const;

   ~PerspectiveCamera() {}
private:
   double theta, phi, r;
   double fov, aspect;
};

class OrthoCamera : public Camera
{
public:
   enum Direction { X, Y, Z };
   OrthoCamera(int window, double h, Direction direct);
   void Activate();
   //does nothing
   void Rotate(int dx, int dy);
   //pans the camera
   void ReloadPosition();
   //returns a ray projected from the camera through the point (X,Y) in the
   //projection plane
   Ray ClickRay(int mouseX, int mouseY) const;
   virtual glm::vec3 InPlaneMovement(int dx, int dy) const;

private:
   float height;
   float aspect;
   Direction d;
};