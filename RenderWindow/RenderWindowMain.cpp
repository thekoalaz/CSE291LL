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
    Scene::World world = Scene::createWorld();
    std::cout << std::string((char *) glGetString(GL_VENDOR)) << std::endl;
    std::cout << std::string((char *) glGetString(GL_RENDERER)) << std::endl;
    std::cout << "OpenGL " << std::string((char *) glGetString(GL_VERSION)) << std::endl;
    std::cout << "====================================================" << std::endl;

    GlutUI::Button & testButton = MANAGER.createButton(mainPanel, 40, 20, 10, 10, "TestButton");

    Scene::Grid * gridXZ = new Scene::Grid();
    world.addObject(gridXZ);
    gridXZ->setTy(-5);

    /* Only draw XZ grid.
    Scene::Grid * gridXY = new Scene::Grid();
    world.addObject(gridXY);
    gridXY->setRotx(90);
    gridXY->setTz(-5);

    Scene::Grid * gridYZ = new Scene::Grid();
    world.addObject(gridYZ);
    gridYZ->setRotz(90);
    gridYZ->setTx(-5);
    */

    Scene::EnvMap * envMap = new Scene::EnvMap();
    std::tuple<float, float, float> color;
    color = envMap->getColor(0.0, 0.0);
    std::cout << "0.0, 0.0:" << std::endl;
    std::cout << std::get<0>(color) << ", ";
    std::cout << std::get<1>(color) << ", ";
    std::cout << std::get<2>(color) << std::endl;
    std::cout << "====================================================" << std::endl;
    color = envMap->getColor(854.0, 424.0);
    std::cout << "854, 424:" << std::endl;
    std::cout << std::get<0>(color) << ", ";
    std::cout << std::get<1>(color) << ", ";
    std::cout << std::get<2>(color) << std::endl;
    std::cout << "====================================================" << std::endl;
    color = envMap->getColor(2246.0, 254.0);
    std::cout << "2246, 254:" << std::endl;
    std::cout << std::get<0>(color) << ", ";
    std::cout << std::get<1>(color) << ", ";
    std::cout << std::get<2>(color) << std::endl;
    std::cout << "====================================================" << std::endl;


    mainPanel.setWorld(&world);
    mainPanel.setCamera(new Scene::Camera());
    GlutUI::Controls::Mouse(mainPanel.getCamera());

    /* New window test*/
    //GlutUI::Window & renderWindow = MANAGER.createWindow(320,480, "RenderWindow");
    //GlutUI::Panel & renderPanel = MANAGER.createPanel(renderWindow, 320,480, "RenderPanel");
    //Scene::World renderWorld = Scene::createWorld();
    //renderPanel.setWorld(&renderWorld);
    //renderPanel.setCamera(new Scene::Camera());

    //GlutUI::Button & renderButton = MANAGER.createButton(renderPanel, 80, 20, 10, 10, "RenderButton");

    MANAGER.drawElements();
	return 0;
}
