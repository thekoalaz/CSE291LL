// RenderWindoeMain.cpp : Creates the 3D Viewer.
//

#include "stdafx.h"
#include "GlutUI.h"

GlutUI::Manager MANAGER;

int main(int argc, char* argv[])
{
    MANAGER.init(argc, argv);

    int windowWidth = 960;
    int windowHeight = 540;
    GlutUI::Window & mainWindow = MANAGER.createWindow(windowWidth, windowHeight, "Render Window");
    GlutUI::Panel & mainPanel = MANAGER.createPanel(mainWindow, windowWidth, windowHeight, "Render Panel");
    Scene::World world = Scene::createWorld();
    std::cout << std::string((char *)glGetString(GL_VENDOR)) << std::endl;
    std::cout << std::string((char *)glGetString(GL_RENDERER)) << std::endl;
    std::cout << "OpenGL " << std::string((char *)glGetString(GL_VERSION)) << std::endl;
    std::cout << "====================================================" << std::endl;

    Scene::Grid * gridXZ = new Scene::Grid(20, 20, 2.0);
    world.addObject(gridXZ);
    gridXZ->setTy(-7.5);

    Scene::Camera * cam = new Scene::Camera();
    cam->setRotz(0);
    cam->setRotx(0);
    cam->setTx(0);
    cam->setTy(0);
    cam->setTz(30);
    world.addObject(cam);

    mainPanel.setWorld(&world);
    mainPanel.setCamera(cam);

    GlutUI::Controls::Keyboard keyboard(&mainPanel);
    GlutUI::Controls::Mouse mouse(&mainPanel, mainPanel.getCamera());

    std::string envmapfile = "grace-new.hdr";
    Scene::EnvMap * envMap = new Scene::EnvMap(envmapfile);
    Scene::EnvShader * envShader = new Scene::EnvShader(envMap, "tonemap_vert.glsl", "tonemap_frag.glsl");
    envMap->bind();
    world.addObject(envMap);
    world.assignShader(envMap, envShader);

    Scene::EnvShader * sphereShader = new Scene::EnvShader(envMap, "sphere_vert.glsl", "sphere_frag.glsl");
    Scene::Sphere * sphere = new Scene::Sphere();
    sphere->setTx(7.5);
    world.addObject(sphere);
    world.assignShader(sphere, sphereShader);

    Scene::DiffuseEnvMap * diffuseMap = new Scene::DiffuseEnvMap(*envMap, 5, 50, 50);
    diffuseMap->useCache("test_diffuse.hdr");
    world.assignShader(diffuseMap, envShader);
    diffuseMap->setRotx(90);
    diffuseMap->setXSkip(32);
    diffuseMap->setYSkip(4);
    diffuseMap->setVisible(false);
    world.addObject(diffuseMap);
    
    Scene::PhongEnvMap * phongMap = new Scene::PhongEnvMap(*envMap, 5, 50, 50);
    phongMap->setSpecCoeffecient(80);
    phongMap->useCache("test_phong.hdr");
    world.assignShader(phongMap, envShader);
    phongMap->setXSkip(32);
    phongMap->setYSkip(4);
    phongMap->setVisible(false);
    world.addObject(phongMap);
    
    Scene::ObjGeometry * kevin = new Scene::ObjGeometry("kevin.obj");
    Scene::EnvShader * mirrorShader = new Scene::EnvShader(envMap, "sphere_vert.glsl", "sphere_frag.glsl");
    Scene::EnvShader * diffuseShader = new Scene::EnvShader(diffuseMap, "tonemap_vert.glsl", "tonemap_frag.glsl");
    Scene::EnvShader * phongShader = new Scene::EnvShader(phongMap, "sphere_vert.glsl", "sphere_frag.glsl");
    world.assignShader(kevin, mirrorShader);
    world.assignShader(sphere, mirrorShader);
    world.addObject(kevin);
    kevin->setTx(-2.5);

    /* Cook Torrance Generation */
    envmapfile = "grace-mini.hdr";
    Scene::EnvMap * envMapSmall = new Scene::EnvMap(envmapfile);
    world.addObject(envMapSmall);
    world.assignShader(envMapSmall, envShader);

    for (int index = 0; index < 12; index++)
    {
        std::string ctName = Scene::CookTorranceIcosMap::getCtIcosMapName(index);
        Scene::CookTorranceIcosMap * ctMap = new Scene::CookTorranceIcosMap(*envMapSmall, 0.1f, 0.8f, index, ctName + ".hdr");
        world.addObject(ctMap);
        delete ctMap;
    }
    std::setfill(' ');

    /* Cook Torrance Render */
    std::vector<Scene::RadMap *> radMaps;
    for (int index = 0; index < 12; index++)
    {
        std::string ctName = Scene::CookTorranceIcosMap::getCtIcosMapName(index);
        Scene::RadMap * radMap = new Scene::RadMap(ctName + ".hdr");
        radMaps.push_back(radMap);
        world.addObject(radMap);
    }
    //Scene::CtShader * ctSphereShader = new Scene::CtShader(radMaps, "warp_vert.glsl", "warp_frag.glsl");
    Scene::CtShader * ctSphereShader = new Scene::CtShader(radMaps, diffuseMap, "warp_vert.glsl", "warp_frag.glsl");

//    Scene::Sphere * ctSphere = new Scene::Sphere();
//    world.assignShader(ctSphere, ctSphereShader);
//    ctSphere->setTx(-10);
//    ctSphere->setRotz(90);
//    //ctSphere->setRoty(90);
//    world.addObject(ctSphere);


    /* Keyboard hotkey assignments */
    auto mlambda = [&]()
    {
        world.setEnvMap(envMap);
        world.assignShader(kevin, mirrorShader);
        world.assignShader(sphere, mirrorShader);
    };
    keyboard.register_hotkey('m', mlambda);
    auto dlambda = [&]() 
    {
        world.setEnvMap(diffuseMap);
        world.assignShader(kevin, diffuseShader);
        world.assignShader(sphere, diffuseShader);
    };
    keyboard.register_hotkey('d', dlambda);
    auto plambda = [&]() 
    {
        world.setEnvMap(phongMap);
        world.assignShader(kevin, phongShader);
        world.assignShader(sphere, phongShader);
    };
    keyboard.register_hotkey('p', plambda);
    auto clambda = [&]()
    {
        world.setEnvMap(envMap);
        world.assignShader(kevin, ctSphereShader);
        world.assignShader(sphere, ctSphereShader);
    };
    keyboard.register_hotkey('c', clambda);

    MANAGER.drawElements();

    return 0;
}
