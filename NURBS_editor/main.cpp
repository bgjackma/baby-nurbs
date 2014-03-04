// NURBS_editor.cpp : Defines the entry point for the console application.
//

#include <gl/glut.h>
#include <gl/freeglut_ext.h>
#include "glui.h"
#include <glm/vec3.hpp>
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
GLUI* menu;

//GLUI live variables
float weight;
glm::vec3 pos;
int u, v;

void ChangedWeight(int)
{
   scene.SetSelectedWeight(weight);
   scene.Recalculate();
}

void ChangedPos(int)
{
   scene.SetSelectedPos(pos);
}

void ResetWeights(int)
{
   scene.ResetWeights();
}

void ResizeCurve(int)
{
   scene.ResizeCurve(u, v);
}

void SetupWindows()
{
   x = 10;
   y = 20;
   width = 1200;
   height = 800;
   u = v = 5;
   glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
   glutInitWindowPosition(x, y);
   glutInitWindowSize(width, height);
   windowID = glutCreateWindow("NURBS Editor");

   subWindows.emplace_back(windowID, 0, 0, width / 2, height / 2);
   subWindows.emplace_back(windowID, width / 2, 0, width / 2, height / 2);
   subWindows.emplace_back(windowID, 0, height / 2, width / 2, height / 2);
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
}

void SetupMenu()
{
   menu = GLUI_Master.create_glui("Controls", 0, x + width + 10, y);
   menu->add_edittext("Control Point Weight", 
      GLUI_EDITTEXT_FLOAT, &weight, 0, ChangedWeight);
   menu->add_button("Reset Weights", 0, ResetWeights);
   menu->add_edittext("X", GLUI_EDITTEXT_FLOAT ,&pos.x, 0, ChangedPos);
   menu->add_edittext("Y", GLUI_EDITTEXT_FLOAT ,&pos.y, 0, ChangedPos);
   menu->add_edittext("Z", GLUI_EDITTEXT_FLOAT ,&pos.z, 0, ChangedPos);
   menu->add_separator();
   GLUI_Panel* sizePanel = menu->add_panel("# Control Points");
   GLUI_EditText* uBox = menu->add_edittext_to_panel(sizePanel, "U", GLUI_EDITTEXT_INT, &u);
   uBox->set_int_limits(4, 20);
   GLUI_EditText* vBox = menu->add_edittext_to_panel(sizePanel, "V", GLUI_EDITTEXT_INT, &v);
   vBox->set_int_limits(4, 20);
   menu->add_button_to_panel(sizePanel, "Reload With New Size", 0, ResizeCurve);


   menu->set_main_gfx_window(windowID);
}

void SetupCallbacks()
{
   glutDisplayFunc(RenderAll);
   glutIdleFunc(RenderAll);
   GLUI_Master.set_glutIdleFunc(RenderAll);

   glLoadIdentity();

   // Set the viewport to be the entire window
   glViewport(0, 0, width, height);
}

void SyncLiveVars()
{
   pos = scene.GetSelectedPos();
   weight = scene.GetSelectedWeight();
   menu->sync_live();
}

void RenderAll()
{
   SyncLiveVars();
   for (auto& subWindow : subWindows)
      subWindow.Render();
}

int main(int argc, char* argv[])
{
   glutInit(&argc, argv);
   SetupWindows();
   SetupCallbacks();
   SetupMenu();
   glutMainLoop();
   return 0;
}

