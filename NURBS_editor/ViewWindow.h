#pragma once
#include <memory>

class Camera;
class Scene;

class ViewWindow
{
public:
   ViewWindow(int mainWindow, int x, int y, int width, int height);
   //construct tiny inactive window
   explicit ViewWindow(int mainWindow);
   ~ViewWindow();
   //copying would invalidate pointers held inside the corresponding glutWindow
   //could reset that pointer, but I'd rather not have another ctor to update
   ViewWindow(const ViewWindow&) = delete;
   int GetID() { return windowID; }

   void SetScene(Scene* s) { scene = s; }
   void SetCamera(std::unique_ptr<Camera> c);

   //non-static version
   void Render();

   //static update functions, should only be passed to glut as callbacks
   static void StaticRender();
   static void MouseClick(int button, int state, int x, int y);
   static void MouseMove(int x, int y);

private:
   int windowID;
   int x, y, width, height;
   std::unique_ptr<Camera> activeCamera;
   Scene* scene;
   enum MouseAction { NONE, ROTATE, PAN, MOVE };
   MouseAction mouseAct;
   int mouseX, mouseY;
};


