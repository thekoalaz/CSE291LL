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

    std::string envmapfile = "grace-new.hdr";
    Scene::EnvMap * envMap = new Scene::EnvMap(envmapfile);
    world.addObject(envMap);
    Scene::Shader * envShader = new Scene::Shader("tonemap.vert", "tonemap.frag");
    world.assignShader(envMap, envShader);

    Scene::Sphere * sphere = new Scene::Sphere();
    world.addObject(sphere);
    Scene::Shader * sphereShader = new Scene::Shader("sphere.vert", "sphere.frag");
    world.assignShader(sphere, sphereShader);
    sphere->setTx(7);

    /*
    Scene::EnvMap * envMapVis = new Scene::EnvMap(envmapfile, 5,20,20);
    world.addObject(envMapVis);
    world.assignShader(envMapVis, envShader);
    envMapVis->setRotx(90);
    envMapVis->setTx(-7);
    */

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
    //world.assignShader(ctMap, envShader);
    //ctMap->setRotx(90);
    //ctMap->setTx(7);
    */

    //Scene::DiffuseEnvMap * diffuseMap = new Scene::DiffuseEnvMap(*envMap, 5, 50, 50);
    //world.addObject(diffuseMap);
    //diffuseMap->useCache("test_diffuse.hdr");
    //world.assignShader(diffuseMap, envShader);
    //diffuseMap->setRotx(90);
    //diffuseMap->setTx(7);
    //diffuseMap->setVisible(false);
    //world.setEnvMap(diffuseMap);

    //Scene::PhongEnvMap * phongMap = new Scene::PhongEnvMap(*envMap, 5, 50, 50);
    //phongMap->setSpecCoeffecient(80);
    //world.addObject(phongMap);
    //phongMap->useCache("test_phong.hdr");
    //world.assignShader(phongMap, envShader);
    //phongMap->setRotx(90);
    //phongMap->setTx(7);
    //phongMap->setVisible(false);
    //world.setEnvMap(phongMap);

    Scene::Shader * kevinShader = new Scene::Shader("diffuse.vert", "diffuse.frag");

    Scene::ObjGeometry * kevin = new Scene::ObjGeometry("kevin.obj");
    world.assignShader(kevin, sphereShader);
    world.addObject(kevin);

    /*
    Scene::EnvMap * radMap00 = new Scene::EnvMap("ctIcos00.hdr");
    Scene::EnvMap * radMap01 = new Scene::EnvMap("ctIcos01.hdr");
    Scene::EnvMap * radMap02 = new Scene::EnvMap("ctIcos02.hdr");
    Scene::EnvMap * radMap03 = new Scene::EnvMap("ctIcos03.hdr");
    Scene::EnvMap * radMap04 = new Scene::EnvMap("ctIcos04.hdr");
    Scene::EnvMap * radMap05 = new Scene::EnvMap("ctIcos05.hdr");
    Scene::EnvMap * radMap06 = new Scene::EnvMap("ctIcos06.hdr");
    Scene::EnvMap * radMap07 = new Scene::EnvMap("ctIcos07.hdr");
    Scene::EnvMap * radMap08 = new Scene::EnvMap("ctIcos08.hdr");
    Scene::EnvMap * radMap09 = new Scene::EnvMap("ctIcos09.hdr");
    Scene::EnvMap * radMap10 = new Scene::EnvMap("ctIcos10.hdr");
    Scene::EnvMap * radMap11 = new Scene::EnvMap("ctIcos11.hdr");
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
    */
    Scene::Sphere * ctSphere = new Scene::Sphere(5,100,100);
    world.addObject(ctSphere);
    Scene::Shader * ctSphereShader = new Scene::Shader("warp.vert", "warp.frag");
    
    //GLint texLoc = glGetUniformLocation(ctSphereShader->getProgram(), "radMaps");
    //int texInd[12] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
    //glUniform1iv(texLoc, 12, texInd);
    world.assignShader(ctSphere, ctSphereShader);
    ctSphere->setTx(-7);
    

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
