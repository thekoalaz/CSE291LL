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

    Scene::Grid * gridXZ = new Scene::Grid(20,20,2.0);
    world.addObject(gridXZ);
    gridXZ->setTy(-7.5);

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

    Scene::Camera * cam = new Scene::Camera();
    /*
    cam->setRoty(-45);
    cam->setRotx(45);
    cam->setTx(0);
    cam->setTy(0);
    cam->setTz(40);
    */
    cam->setRotz(0);
    cam->setRoty(0);
    cam->setRotx(90);
    cam->setTx(0);
    cam->setTy(0);
    cam->setTz(40);
    world.addObject(cam);

    Scene::Sphere * sphere = new Scene::Sphere();
    world.addObject(sphere);
    Scene::Shader * sphereShader = new Scene::Shader("sphere.vert", "sphere.frag");
    world.assignShader(sphere, sphereShader);
    sphere->setTx(7.5);

    Scene::Sphere * specSphere = new Scene::Sphere();
    world.addObject(specSphere);
    Scene::Shader * sphereSpecShader = new Scene::Shader("specSphere.vert", "specSphere.frag");
    world.assignShader(specSphere, sphereSpecShader);
    specSphere->setTx(-7.5);

    Scene::EnvMap * envMap = new Scene::EnvMap();
    world.addObject(envMap);
    envMap->setRotx(90);
    envMap->setRotz(180);
    Scene::Shader * envShader = new Scene::Shader("tonemap.vert", "tonemap.frag");
    world.assignShader(envMap, envShader);

    mainPanel.setWorld(&world);
    mainPanel.setCamera(cam);
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
