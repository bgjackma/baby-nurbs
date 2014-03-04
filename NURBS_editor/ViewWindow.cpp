#include "ViewWindow.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/freeglut.h>
#include <iostream>
#include "Camera.h"
#include "Scene.h"


ViewWindow::ViewWindow(int mainWindow, int x, int y, int width, int height)
:x(x), y(y), width(width), height(height), mouseAct(MouseAction::NONE)
{
   windowID = glutCreateSubWindow(mainWindow, x, y, width, height);
   glutSetWindowData(static_cast<void*>(this));
   glPointSize(5);
   glEnable(GL_POINT_SMOOTH);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_LIGHT0);
   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat mat_shininess[] = { 50.0 };
   GLfloat light_position[] = { 1.0, 1.0, -1.0, 0.0 };
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_SMOOTH);
   glEnable(GL_LIGHTING);

   //glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   //glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   glViewport(0, 0, width, height);

   //register callbacks
   glutDisplayFunc(StaticRender);
   glutMouseFunc(MouseClick);
   glutMotionFunc(MouseMove);

   //set default camera
   activeCamera.reset(new PerspectiveCamera(windowID, M_PI / 4, M_PI / 4, 3.0, 45.0));
   activeCamera->Activate();
}

void ViewWindow::SetCamera(std::unique_ptr<Camera> c)
{
   activeCamera = std::move(c);
}

ViewWindow::ViewWindow(int mainWindow)
:x(0), y(0), width(1), height(1)
{

   windowID = glutCreateSubWindow(mainWindow, 0, 0, 1, 1);
   //do nothing
   glutDisplayFunc([](){});
}


void ViewWindow::StaticRender()
{
   ViewWindow& self = *static_cast<ViewWindow*>(glutGetWindowData());
   self.Render();
}

void ViewWindow::Render()
{
   glutSetWindow(windowID);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();
   glClearColor(0, 0, 0, 0);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   scene->DrawWireframe();
   glutPostRedisplay();
   glutSwapBuffers();
}

void ViewWindow::MouseClick(int button, int state, int x, int y)
{
   ViewWindow& self = *static_cast<ViewWindow*>(glutGetWindowData());
   if (state == GLUT_DOWN)
   {
      self.mouseX = x;
      self.mouseY = y;
      switch (button)
      {
      case GLUT_RIGHT_BUTTON:
         if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)
         {
            self.mouseAct = MouseAction::PAN;
         }
         else
         {
            self.mouseAct = MouseAction::ROTATE;
         }
         break;
      case GLUT_LEFT_BUTTON:
         if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)
         {
            self.mouseAct = MouseAction::MOVE;
         }
         else
         {
            self.scene->SelectOnRay(self.activeCamera->ClickRay(x, y), 0.04f);
         }
         break;
      }
   }
   else //on mouse release
   {
      if (self.mouseAct == MouseAction::MOVE)
         self.scene->Recalculate();
      self.mouseAct = MouseAction::NONE;
   }
}
void ViewWindow::MouseMove(int x, int y)
{
   ViewWindow& self = *static_cast<ViewWindow*>(glutGetWindowData());
   switch (self.mouseAct)
   {
   case ROTATE:
      self.activeCamera->Rotate(x - self.mouseX, y - self.mouseY);
      break;
   case MOVE:
      self.scene->MoveSelected(self.activeCamera->InPlaneMovement(x - self.mouseX, y - self.mouseY));
      break;
   }
   self.mouseX = x;
   self.mouseY = y;
}

ViewWindow::~ViewWindow()
{
}
