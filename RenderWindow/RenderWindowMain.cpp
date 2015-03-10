// RenderWindoeMain.cpp : Creates the 3D Viewer.
//

#include "stdafx.h"
#include "GlutUI.h"

GlutUI::Manager MANAGER;

int main(int argc, char* argv[])
{
    MANAGER.init(argc, argv);

    GlutUI::Window & mainWindow = MANAGER.createWindow(640, 480, "Render Window");
    GlutUI::Panel & mainPanel = MANAGER.createPanel(mainWindow, 640, 480, "Render Panel");
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
    world.addObject(envMap);
    world.assignShader(envMap, envShader);

    Scene::EnvShader * sphereShader = new Scene::EnvShader(envMap, "sphere_vert.glsl", "sphere_frag.glsl");
    Scene::Sphere * sphere = new Scene::Sphere();
    sphere->setTx(10);
    world.addObject(sphere);
    world.assignShader(sphere, sphereShader);

    Scene::DiffuseEnvMap * diffuseMap = new Scene::DiffuseEnvMap(*envMap, 5, 50, 50);
    diffuseMap->useCache("test_diffuse.hdr");
    world.assignShader(diffuseMap, envShader);
    diffuseMap->setRotx(90);
    diffuseMap->setXSkip(64);
    diffuseMap->setYSkip(8);
    diffuseMap->setVisible(false);
    world.addObject(diffuseMap);

    Scene::PhongEnvMap * phongMap = new Scene::PhongEnvMap(*envMap, 5, 50, 50);
    phongMap->setSpecCoeffecient(80);
    phongMap->useCache("test_phong.hdr");
    world.assignShader(phongMap, envShader);
    phongMap->setXSkip(64);
    phongMap->setYSkip(8);
    phongMap->setRotx(90);
    phongMap->setVisible(false);
    world.addObject(phongMap);

    Scene::ObjGeometry * kevin = new Scene::ObjGeometry("kevin.obj");
    Scene::EnvShader * mirrorShader = new Scene::EnvShader(phongMap, "sphere_vert.glsl", "sphere_frag.glsl");
    Scene::EnvShader * diffuseShader = new Scene::EnvShader(diffuseMap, "tonemap_vert.glsl", "tonemap_frag.glsl");
    Scene::EnvShader * phongShader = new Scene::EnvShader(phongMap, "sphere_vert.glsl", "sphere_frag.glsl");
    world.assignShader(kevin, mirrorShader);
    world.assignShader(sphere, mirrorShader);
    world.addObject(kevin);

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


    /* Cook Torrance Generation */
    //std::vector<Scene::CookTorranceIcosMap *> ctMaps;
    //std::ostringstream ctName;
    //for (int index = 0; index < 12; index++)
    //{
    //    ctName << "ctIcos" << std::setfill('0') << std::setw(2) << index;
    //    std::cout << ctName.str() << std::endl;
    //    Scene::CookTorranceIcosMap * ctMap = new Scene::CookTorranceIcosMap(*envMap, 0.3, 0.8, 0);
    //    ctMaps.push_back(ctMap);
    //    ctMap->useCache(ctName.str() + ".hdr");
    //    world.addObject(ctMap);
    //    ctName.str("");
    //    ctName.clear();
    //}
    //std::setfill(' ');


    /* Cook Torrance Render */
    //std::vector<Scene::RadMap *> radMaps;
    //std::ostringstream radName;
    //Scene::Shader * ctSphereShader = new Scene::Shader("warp_vert.glsl", "warp_frag.glsl");
    //for (int index = 0; index < 12; index++)
    //{
    //    radName << "ctIcos" << std::setfill('0') << std::setw(2) << index;
    //    std::cout << radName.str() << std::endl;
    //    Scene::RadMap * radMap = new Scene::RadMap(radName.str() + ".hdr");
    //    radMaps.push_back(radMap);
    //    world.addObject(radMap);
    //    GLint radMapLocation = glGetUniformLocation(ctSphereShader->getProgram(), radName.str().c_str());
    //    glUniform1i(radMapLocation, radMap->_getTextureID());
    //    radName.str("");
    //    radName.clear();
    //}
    //std::setfill(' ');

    //Scene::Sphere * ctSphere = new Scene::Sphere();
    //world.assignShader(ctSphere, ctSphereShader);
    //ctSphere->setTx(-5);
    //world.addObject(ctSphere);

    //Scene::ObjGeometry * kevin = new Scene::ObjGeometry("kevin.obj");
    //world.assignShader(kevin, ctSphereShader);
    //kevin->setTx(5);
    //world.addObject(kevin);


    //auto clambda = [&]() { world.assignShader(kevin, ctSphereShader); world.setEnvMap(envMap); };
    //keyboard.register_hotkey('c', clambda);

    MANAGER.drawElements();

    return 0;
}
