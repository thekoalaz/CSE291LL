// RenderWindoeMain.cpp : Creates the 3D Viewer.
//

#include "stdafx.h"
#include "GlutUI.h"

GlutUI::Manager MANAGER;

int main(int argc, char* argv[])
{
    MANAGER.init(argc, argv);

    GlutUI::Window & mainWindow = MANAGER.createWindow(640,480, "TestWindow");
    GlutUI::Panel & mainPanel = MANAGER.createPanel(mainWindow, 640,480, "TestPanel");
    std::cout << std::string((char *) glGetString(GL_VENDOR)) << std::endl;
    std::cout << std::string((char *) glGetString(GL_RENDERER)) << std::endl;
    std::cout << "OpenGL " << std::string((char *) glGetString(GL_VERSION)) << std::endl;
    std::cout << "====================================================" << std::endl;
    Scene::World world = Scene::createWorld();
    mainPanel.setWorld(&world);

    GlutUI::Button & testButton = MANAGER.createButton(mainPanel, 40, 20, 10, 10, "TestButton");

    Scene::Grid * gridXZ = new Scene::Grid();
    world.addObject(gridXZ);
    gridXZ->setTy(-5);

    Scene::Grid * gridXY = new Scene::Grid();
    world.addObject(gridXY);
    gridXY->setRotx(90);
    gridXY->setTz(-5);

    Scene::Grid * gridYZ = new Scene::Grid();
    world.addObject(gridYZ);
    gridYZ->setRotz(90);
    gridYZ->setTx(-5);
	
	Scene::Sphere * sphere = new Scene::Sphere();
	world.addObject(sphere);
	sphere->setTx(0);
	sphere->setTy(0);
	sphere->setTz(0);
	
	Scene::Camera * cam = new Scene::Camera();
	cam->setRoty(-45);
	cam->setRotx(45);
	cam->setTx(0);
	cam->setTy(0);
	cam->setTz(20);
	
    mainPanel.setWorld(&world);
	mainPanel.setCamera(cam);
    GlutUI::Controls::Mouse(mainPanel.getCamera());

    MANAGER.drawElements();
	return 0;
}
