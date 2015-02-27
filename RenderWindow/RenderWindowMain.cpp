// RenderWindoeMain.cpp : Creates the 3D Viewer.
//

#include "stdafx.h"
#include "GlutUI.h"

GlutUI::Manager MANAGER;

int main(int argc, char* argv[])
{
    MANAGER.init(argc, argv);

    GlutUI::Window & mainWindow = MANAGER.createWindow(640,480, "Render Window");
    GlutUI::Panel & mainPanel = MANAGER.createPanel(mainWindow, 640,480, "TestPanel");
    Scene::World world = Scene::createWorld();
    std::cout << std::string((char *) glGetString(GL_VENDOR)) << std::endl;
    std::cout << std::string((char *) glGetString(GL_RENDERER)) << std::endl;
    std::cout << "OpenGL " << std::string((char *) glGetString(GL_VERSION)) << std::endl;
    std::cout << "====================================================" << std::endl;

    //GlutUI::Button & testButton = MANAGER.createButton(mainPanel, 40, 20, 10, 10, "TestButton");

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
    //cam->setRoty(-90);
    cam->setRotx(0);
    cam->setTx(0);
    cam->setTy(0);
    cam->setTz(30);
    world.addObject(cam);

    Scene::Sphere * sphere = new Scene::Sphere();
    world.addObject(sphere);
    Scene::Shader * sphereShader = new Scene::Shader("sphere.vert", "sphere.frag");
    world.assignShader(sphere, sphereShader);
    
    //Scene::EnvMap * envMap = new Scene::EnvMap("half.hdr");
    //world.addObject(envMap);
    Scene::Shader * envShader = new Scene::Shader("tonemap.vert", "tonemap.frag");
    //world.assignShader(envMap, envShader);

    Scene::EnvMap * envMapVis = new Scene::EnvMap("quarter.hdr", 5,20,20);
    world.addObject(envMapVis);
    world.assignShader(envMapVis, envShader);
    envMapVis->setTx(-15);

    Scene::DiffuseEnvMap * diffuseMap = new Scene::DiffuseEnvMap(*envMapVis, 5, 20, 20);
    world.addObject(diffuseMap);
    diffuseMap->setTx(15);
    //diffuseMap->useCache("test.hdr");
    world.assignShader(diffuseMap, envShader);

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
