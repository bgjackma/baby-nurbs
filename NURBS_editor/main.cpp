// main.cpp : Defines the entry point for the console application.
//

#include <gl/glut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include <list>
#include <memory>
#include "Scene.h"
#include "ViewWindow.h"
#include "Camera.h"

void RenderAll();

//there's a static instance of glut, so there's a static main editor window
std::list<ViewWindow> subWindows;
int windowID;
int x, y, width, height;
Scene scene;

void SetupWindows()
{
   x = 10;
   y = 20;
   width = 1200;
   height = 800;
   glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
   glutInitWindowPosition(x, y);
   glutInitWindowSize(width, height);
   windowID = glutCreateWindow("NURBS Editor");

   subWindows.emplace_back(windowID, 0, 0, width / 2, height / 2);
   subWindows.emplace_back(windowID, width/2, 0, width / 2, height / 2);
   subWindows.emplace_back(windowID, 0, height/2, width / 2, height / 2);
   subWindows.emplace_back(windowID, width / 2, height / 2, width / 2, height / 2);
   //freeglut craps out on the last window, so here's a dummy
   subWindows.emplace_back(windowID);
   //set ortho cameras (perspective is default)
   auto win = subWindows.begin();
   win->SetCamera(std::unique_ptr<OrthoCamera>(new OrthoCamera(win->GetID(),
      3, OrthoCamera::Z)));
   win++;
   win->SetCamera(std::unique_ptr<OrthoCamera>(new OrthoCamera(win->GetID(),
      3, OrthoCamera::X)));
   win++;
   win->SetCamera(std::unique_ptr<OrthoCamera>(new OrthoCamera(win->GetID(),
      3, OrthoCamera::Y)));

   for (auto& view : subWindows)
      view.SetScene(&scene);

   glutDisplayFunc(RenderAll);
   glutIdleFunc(RenderAll);

   glLoadIdentity();

   // Set the viewport to be the entire window
   glViewport(0, 0, width, height);
}

void RenderAll()
{
   for (auto& subWindow : subWindows)
      subWindow.Render();
}

int main(int argc, char* argv[])
{
   glutInit(&argc, argv);
   SetupWindows();
   glutMainLoop();
   return 0;
}

