// RenderWindoeMain.cpp : Creates the 3D Viewer.
//

#include "stdafx.h"
#include "GlutUI.h"

GlutUI::Manager MANAGER;

int main(int argc, char* argv[])
{
    MANAGER.init(argc, argv);

    GlutUI::Window & mainWindow = MANAGER.createWindow(640,480, "Render Window");
    GlutUI::Panel & mainPanel = MANAGER.createPanel(mainWindow, 640,480, "Render Panel");
    Scene::World world = Scene::createWorld();
    std::cout << std::string((char *) glGetString(GL_VENDOR)) << std::endl;
    std::cout << std::string((char *) glGetString(GL_RENDERER)) << std::endl;
    std::cout << "OpenGL " << std::string((char *) glGetString(GL_VERSION)) << std::endl;
    std::cout << "====================================================" << std::endl;

    Scene::Grid * gridXZ = new Scene::Grid(20,20,2.0);
    world.addObject(gridXZ);
    gridXZ->setTy(-7.5);

    Scene::Camera * cam = new Scene::Camera();
    cam->setRotz(0);
    cam->setRotx(0);
    cam->setTx(0);
    cam->setTy(0);
    cam->setTz(30);
    world.addObject(cam);

    std::string envmapfile = "grace-new.hdr";
    Scene::EnvMap * envMap = new Scene::EnvMap(envmapfile);
    world.addObject(envMap);
    Scene::Shader * envShader = new Scene::Shader("tonemap_vert.glsl", "tonemap_frag.glsl");
    world.assignShader(envMap, envShader);

    Scene::Shader * sphereShader = new Scene::Shader("sphere_vert.glsl", "sphere_frag.glsl");

    Scene::Sphere * sphere = new Scene::Sphere();
    sphere->setTx(7);
    world.addObject(sphere);
    world.assignShader(sphere, sphereShader);

    /*
    Scene::CookTorranceIcosMap * ctMap00 = new Scene::CookTorranceIcosMap(*envMapVis, 0.3, 0.8, 0);
    Scene::CookTorranceIcosMap * ctMap01 = new Scene::CookTorranceIcosMap(*envMapVis, 0.3, 0.8, 1);
    Scene::CookTorranceIcosMap * ctMap02 = new Scene::CookTorranceIcosMap(*envMapVis, 0.3, 0.8, 2);
    Scene::CookTorranceIcosMap * ctMap03 = new Scene::CookTorranceIcosMap(*envMapVis, 0.3, 0.8, 3);
    Scene::CookTorranceIcosMap * ctMap04 = new Scene::CookTorranceIcosMap(*envMapVis, 0.3, 0.8, 4);
    Scene::CookTorranceIcosMap * ctMap05 = new Scene::CookTorranceIcosMap(*envMapVis, 0.3, 0.8, 5);
    Scene::CookTorranceIcosMap * ctMap06 = new Scene::CookTorranceIcosMap(*envMapVis, 0.3, 0.8, 6);
    Scene::CookTorranceIcosMap * ctMap07 = new Scene::CookTorranceIcosMap(*envMapVis, 0.3, 0.8, 7);
    Scene::CookTorranceIcosMap * ctMap08 = new Scene::CookTorranceIcosMap(*envMapVis, 0.3, 0.8, 8);
    Scene::CookTorranceIcosMap * ctMap09 = new Scene::CookTorranceIcosMap(*envMapVis, 0.3, 0.8, 9);
    Scene::CookTorranceIcosMap * ctMap10 = new Scene::CookTorranceIcosMap(*envMapVis, 0.3, 0.8, 10);
    Scene::CookTorranceIcosMap * ctMap11 = new Scene::CookTorranceIcosMap(*envMapVis, 0.3, 0.8, 11);
    ctMap00->useCache("ctIcos00.hdr");
    ctMap01->useCache("ctIcos01.hdr");
    ctMap02->useCache("ctIcos02.hdr");
    ctMap03->useCache("ctIcos03.hdr");
    ctMap04->useCache("ctIcos04.hdr");
    ctMap05->useCache("ctIcos05.hdr");
    ctMap06->useCache("ctIcos06.hdr");
    ctMap07->useCache("ctIcos07.hdr");
    ctMap08->useCache("ctIcos08.hdr");
    ctMap09->useCache("ctIcos09.hdr");
    ctMap10->useCache("ctIcos10.hdr");
    ctMap11->useCache("ctIcos11.hdr");
    world.addObject(ctMap00);
    world.addObject(ctMap01);
    world.addObject(ctMap02);
    world.addObject(ctMap03);
    world.addObject(ctMap04);
    world.addObject(ctMap05);
    world.addObject(ctMap06);
    world.addObject(ctMap07);
    world.addObject(ctMap08);
    world.addObject(ctMap09);
    world.addObject(ctMap10);
    world.addObject(ctMap11);
    world.assignShader(ctMap, envShader);
    ctMap->setRotx(90);
    ctMap->setTx(7);
    */

    Scene::DiffuseEnvMap * diffuseMap = new Scene::DiffuseEnvMap(*envMap, 5, 50, 50);
    world.addObject(diffuseMap);
    diffuseMap->useCache("test_diffuse.hdr");
    world.assignShader(diffuseMap, envShader);
    diffuseMap->setRotx(90);
    diffuseMap->setXSkip(64);
    diffuseMap->setYSkip(8);
    diffuseMap->setVisible(false);
    world.setEnvMap(diffuseMap);

    Scene::PhongEnvMap * phongMap = new Scene::PhongEnvMap(*envMap, 5, 50, 50);
    phongMap->setSpecCoeffecient(80);
    world.addObject(phongMap);
    phongMap->useCache("test_phong.hdr");
    world.assignShader(phongMap, envShader);
    phongMap->setXSkip(64);
    phongMap->setYSkip(8);
    phongMap->setRotx(90);
    phongMap->setVisible(false);
    world.setEnvMap(phongMap);

    Scene::ObjGeometry * kevin = new Scene::ObjGeometry("kevin.obj");
    world.assignShader(kevin, sphereShader);
    world.addObject(kevin);

    //Scene::RadMap * radMap00 = new Scene::RadMap("ctIcos00.hdr");
    //Scene::RadMap * radMap01 = new Scene::RadMap("ctIcos01.hdr");
    //Scene::RadMap * radMap02 = new Scene::RadMap("ctIcos02.hdr");
    //Scene::RadMap * radMap03 = new Scene::RadMap("ctIcos03.hdr");
    //Scene::RadMap * radMap04 = new Scene::RadMap("ctIcos04.hdr");
    //Scene::RadMap * radMap05 = new Scene::RadMap("ctIcos05.hdr");
    //Scene::RadMap * radMap06 = new Scene::RadMap("ctIcos06.hdr");
    //Scene::RadMap * radMap07 = new Scene::RadMap("ctIcos07.hdr");
    //Scene::RadMap * radMap08 = new Scene::RadMap("ctIcos08.hdr");
    //Scene::RadMap * radMap09 = new Scene::RadMap("ctIcos09.hdr");
    //Scene::RadMap * radMap10 = new Scene::RadMap("ctIcos10.hdr");
    //Scene::RadMap * radMap11 = new Scene::RadMap("ctIcos11.hdr");
    //world.addObject(radMap00);
    //world.addObject(radMap01);
    //world.addObject(radMap02);
    //world.addObject(radMap03);
    //world.addObject(radMap04);
    //world.addObject(radMap05);
    //world.addObject(radMap06);
    //world.addObject(radMap07);
    //world.addObject(radMap08);
    //world.addObject(radMap09);
    //world.addObject(radMap10);
    //world.addObject(radMap11);

    //Scene::Sphere * ctSphere = new Scene::Sphere(5,100,100);
    //world.addObject(ctSphere);
    //Scene::Shader * ctSphereShader = new Scene::Shader("warp_vert.glsl", "warp_frag.glsl");

    //GLint radMapLocation00 = glGetUniformLocation(ctSphereShader->getProgram(), "radMaps[0]");
    //GLint radMapLocation01 = glGetUniformLocation(ctSphereShader->getProgram(), "radMaps[1]");
    //GLint radMapLocation02 = glGetUniformLocation(ctSphereShader->getProgram(), "radMaps[2]");
    //GLint radMapLocation03 = glGetUniformLocation(ctSphereShader->getProgram(), "radMaps[3]");
    //GLint radMapLocation04 = glGetUniformLocation(ctSphereShader->getProgram(), "radMaps[4]");
    //GLint radMapLocation05 = glGetUniformLocation(ctSphereShader->getProgram(), "radMaps[5]");
    //GLint radMapLocation06 = glGetUniformLocation(ctSphereShader->getProgram(), "radMaps[6]");
    //GLint radMapLocation07 = glGetUniformLocation(ctSphereShader->getProgram(), "radMaps[7]");
    //GLint radMapLocation08 = glGetUniformLocation(ctSphereShader->getProgram(), "radMaps[8]");
    //GLint radMapLocation09 = glGetUniformLocation(ctSphereShader->getProgram(), "radMaps[9]");
    //GLint radMapLocation10 = glGetUniformLocation(ctSphereShader->getProgram(), "radMaps[10]");
    //GLint radMapLocation11 = glGetUniformLocation(ctSphereShader->getProgram(), "radMaps[11]");
    //glUniform1i(radMapLocation00, radMap00->_getTextureID());
    //glUniform1i(radMapLocation01, radMap01->_getTextureID());
    //glUniform1i(radMapLocation02, radMap02->_getTextureID());
    //glUniform1i(radMapLocation03, radMap03->_getTextureID());
    //glUniform1i(radMapLocation04, radMap04->_getTextureID());
    //glUniform1i(radMapLocation05, radMap05->_getTextureID());
    //glUniform1i(radMapLocation06, radMap06->_getTextureID());
    //glUniform1i(radMapLocation07, radMap07->_getTextureID());
    //glUniform1i(radMapLocation08, radMap08->_getTextureID());
    //glUniform1i(radMapLocation09, radMap09->_getTextureID());
    //glUniform1i(radMapLocation10, radMap10->_getTextureID());
    //glUniform1i(radMapLocation11, radMap11->_getTextureID());
    //world.assignShader(ctSphere, ctSphereShader);
    //world.assignShader(ctSphere, sphereShader);
    //ctSphere->setTx(-7);

    mainPanel.setWorld(&world);
    mainPanel.setCamera(cam);

    GlutUI::Controls::Keyboard keyboard(&mainPanel);
    GlutUI::Controls::Mouse mouse(&mainPanel, mainPanel.getCamera());

    MANAGER.drawElements();

    return 0;
}
