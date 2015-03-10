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
    /*
    std::string envmapfile = "grace-mini.hdr";
    Scene::EnvMap * envMap = new Scene::EnvMap(envmapfile);
    Scene::EnvShader * envShader = new Scene::EnvShader(envMap, "tonemap_vert.glsl", "tonemap_frag.glsl");
    world.addObject(envMap);
    world.assignShader(envMap, envShader);
    */
    //Scene::EnvShader * sphereShader = new Scene::EnvShader(envMap, "sphere_vert.glsl", "sphere_frag.glsl");
    //Scene::Sphere * sphere = new Scene::Sphere();
    //sphere->setTx(10);
    //world.addObject(sphere);
    //world.assignShader(sphere, sphereShader);

    /*
    Scene::CookTorranceIcosMap * ctMap00 = new Scene::CookTorranceIcosMap(*envMap, 0.3, 0.8, 0);
    Scene::CookTorranceIcosMap * ctMap01 = new Scene::CookTorranceIcosMap(*envMap, 0.3, 0.8, 1);
    Scene::CookTorranceIcosMap * ctMap02 = new Scene::CookTorranceIcosMap(*envMap, 0.3, 0.8, 2);
    Scene::CookTorranceIcosMap * ctMap03 = new Scene::CookTorranceIcosMap(*envMap, 0.3, 0.8, 3);
    Scene::CookTorranceIcosMap * ctMap04 = new Scene::CookTorranceIcosMap(*envMap, 0.3, 0.8, 4);
    Scene::CookTorranceIcosMap * ctMap05 = new Scene::CookTorranceIcosMap(*envMap, 0.3, 0.8, 5);
    Scene::CookTorranceIcosMap * ctMap06 = new Scene::CookTorranceIcosMap(*envMap, 0.3, 0.8, 6);
    Scene::CookTorranceIcosMap * ctMap07 = new Scene::CookTorranceIcosMap(*envMap, 0.3, 0.8, 7);
    Scene::CookTorranceIcosMap * ctMap08 = new Scene::CookTorranceIcosMap(*envMap, 0.3, 0.8, 8);
    Scene::CookTorranceIcosMap * ctMap09 = new Scene::CookTorranceIcosMap(*envMap, 0.3, 0.8, 9);
    Scene::CookTorranceIcosMap * ctMap10 = new Scene::CookTorranceIcosMap(*envMap, 0.3, 0.8, 10);
    Scene::CookTorranceIcosMap * ctMap11 = new Scene::CookTorranceIcosMap(*envMap, 0.3, 0.8, 11);
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
    
    ctMap00->setXSkip(64);
    ctMap01->setXSkip(64);
    ctMap02->setXSkip(64);
    ctMap03->setXSkip(64);
    ctMap04->setXSkip(64);
    ctMap05->setXSkip(64);
    ctMap06->setXSkip(64);
    ctMap07->setXSkip(64);
    ctMap08->setXSkip(64);
    ctMap09->setXSkip(64);
    ctMap10->setXSkip(64);
    ctMap11->setXSkip(64);
    ctMap00->setYSkip(8);
    ctMap01->setYSkip(8);
    ctMap02->setYSkip(8);
    ctMap03->setYSkip(8);
    ctMap04->setYSkip(8);
    ctMap05->setYSkip(8);
    ctMap06->setYSkip(8);
    ctMap07->setYSkip(8);
    ctMap08->setYSkip(8);
    ctMap09->setYSkip(8);
    ctMap10->setYSkip(8);
    ctMap11->setYSkip(8);
    
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
    */

    //Scene::DiffuseEnvMap * diffuseMap = new Scene::DiffuseEnvMap(*envMap, 5, 50, 50);
    //diffuseMap->useCache("test_diffuse.hdr");
    //world.assignShader(diffuseMap, envShader);
    //diffuseMap->setRotx(90);
    //diffuseMap->setXSkip(64);
    //diffuseMap->setYSkip(8);
    //diffuseMap->setVisible(false);
    //world.addObject(diffuseMap);

    //Scene::PhongEnvMap * phongMap = new Scene::PhongEnvMap(*envMap, 5, 50, 50);
    //phongMap->setSpecCoeffecient(80);
    //phongMap->useCache("test_phong.hdr");
    //world.assignShader(phongMap, envShader);
    //phongMap->setXSkip(64);
    //phongMap->setYSkip(8);
    //phongMap->setRotx(90);
    //phongMap->setVisible(false);
    //world.addObject(phongMap);
    
    Scene::RadMap * diffMap = new Scene::RadMap("test_diffuse.hdr");
    Scene::RadMap * radMap00 = new Scene::RadMap("ctIcos00.hdr");
    Scene::RadMap * radMap01 = new Scene::RadMap("ctIcos01.hdr");
    Scene::RadMap * radMap02 = new Scene::RadMap("ctIcos02.hdr");
    Scene::RadMap * radMap03 = new Scene::RadMap("ctIcos03.hdr");
    Scene::RadMap * radMap04 = new Scene::RadMap("ctIcos04.hdr");
    Scene::RadMap * radMap05 = new Scene::RadMap("ctIcos05.hdr");
    Scene::RadMap * radMap06 = new Scene::RadMap("ctIcos06.hdr");
    Scene::RadMap * radMap07 = new Scene::RadMap("ctIcos07.hdr");
    Scene::RadMap * radMap08 = new Scene::RadMap("ctIcos08.hdr");
    Scene::RadMap * radMap09 = new Scene::RadMap("ctIcos09.hdr");
    Scene::RadMap * radMap10 = new Scene::RadMap("ctIcos10.hdr");
    Scene::RadMap * radMap11 = new Scene::RadMap("ctIcos11.hdr");
    world.addObject(diffMap);
    world.addObject(radMap00);
    world.addObject(radMap01);
    world.addObject(radMap02);
    world.addObject(radMap03);
    world.addObject(radMap04);
    world.addObject(radMap05);
    world.addObject(radMap06);
    world.addObject(radMap07);
    world.addObject(radMap08);
    world.addObject(radMap09);
    world.addObject(radMap10);
    world.addObject(radMap11);
    Scene::Shader * ctSphereShader = new Scene::Shader("warp_vert.glsl", "warp_frag.glsl");
    GLint diffMapLocation = glGetUniformLocation(ctSphereShader->getProgram(), "diffMap");
    GLint radMapLocation00 = glGetUniformLocation(ctSphereShader->getProgram(), "radMap00");
    GLint radMapLocation01 = glGetUniformLocation(ctSphereShader->getProgram(), "radMap01");
    GLint radMapLocation02 = glGetUniformLocation(ctSphereShader->getProgram(), "radMap02");
    GLint radMapLocation03 = glGetUniformLocation(ctSphereShader->getProgram(), "radMap03");
    GLint radMapLocation04 = glGetUniformLocation(ctSphereShader->getProgram(), "radMap04");
    GLint radMapLocation05 = glGetUniformLocation(ctSphereShader->getProgram(), "radMap05");
    GLint radMapLocation06 = glGetUniformLocation(ctSphereShader->getProgram(), "radMap06");
    GLint radMapLocation07 = glGetUniformLocation(ctSphereShader->getProgram(), "radMap07");
    GLint radMapLocation08 = glGetUniformLocation(ctSphereShader->getProgram(), "radMap08");
    GLint radMapLocation09 = glGetUniformLocation(ctSphereShader->getProgram(), "radMap09");
    GLint radMapLocation10 = glGetUniformLocation(ctSphereShader->getProgram(), "radMap10");
    GLint radMapLocation11 = glGetUniformLocation(ctSphereShader->getProgram(), "radMap11");
    glUniform1i(diffMapLocation, diffMap->_getTextureID());
    glUniform1i(radMapLocation00, radMap00->_getTextureID());
    glUniform1i(radMapLocation01, radMap01->_getTextureID());
    glUniform1i(radMapLocation02, radMap02->_getTextureID());
    glUniform1i(radMapLocation03, radMap03->_getTextureID());
    glUniform1i(radMapLocation04, radMap04->_getTextureID());
    glUniform1i(radMapLocation05, radMap05->_getTextureID());
    glUniform1i(radMapLocation06, radMap06->_getTextureID());
    glUniform1i(radMapLocation07, radMap07->_getTextureID());
    glUniform1i(radMapLocation08, radMap08->_getTextureID());
    glUniform1i(radMapLocation09, radMap09->_getTextureID());
    glUniform1i(radMapLocation10, radMap10->_getTextureID());
    glUniform1i(radMapLocation11, radMap11->_getTextureID());
    Scene::Sphere * ctSphere = new Scene::Sphere();
    world.assignShader(ctSphere, ctSphereShader);
    world.addObject(ctSphere);
    ctSphere->setTx(0);

    //Scene::ObjGeometry * kevin = new Scene::ObjGeometry("kevin.obj");
    //Scene::EnvShader * mirrorShader = new Scene::EnvShader(phongMap, "sphere_vert.glsl", "sphere_frag.glsl");
    //Scene::EnvShader * diffuseShader = new Scene::EnvShader(diffuseMap, "tonemap_vert.glsl", "tonemap_frag.glsl");
    //Scene::EnvShader * phongShader = new Scene::EnvShader(phongMap, "sphere_vert.glsl", "sphere_frag.glsl");
    //world.assignShader(kevin, ctSphereShader);
    //world.addObject(kevin);

    mainPanel.setWorld(&world);
    mainPanel.setCamera(cam);

    GlutUI::Controls::Keyboard keyboard(&mainPanel);
    GlutUI::Controls::Mouse mouse(&mainPanel, mainPanel.getCamera());

    //auto mlambda = [&]() { world.assignShader(kevin, mirrorShader); world.setEnvMap(envMap); };
    //keyboard.register_hotkey('m', mlambda);
    //auto dlambda = [&]() { world.assignShader(kevin, diffuseShader); world.setEnvMap(diffuseMap); };
    //keyboard.register_hotkey('d', dlambda);
    //auto plambda = [&]() { world.assignShader(kevin, phongShader); world.setEnvMap(phongMap); };
    //keyboard.register_hotkey('p', plambda);
    //auto clambda = [&]() { world.assignShader(kevin, ctSphereShader); world.setEnvMap(envMap); };
    //keyboard.register_hotkey('c', clambda);

    MANAGER.drawElements();

    return 0;
}
