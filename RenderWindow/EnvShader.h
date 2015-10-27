#pragma once
#include "stdafx.h"
#include "scene.h"

namespace Scene
{

class RadMap : public EnvMap
{
public:
    RadMap(std::string  filename) :
        EnvMap(filename) {
        _textureID = nextTextureID();
        _readMap();
        _mapReady = true;
        _visible = false;
    }

    static std::string getRadMapName(int index);
};

class PrecomputeMap : public EnvMap
{
public:
    PrecomputeMap(EnvMap & envMap) : EnvMap(), _envMap(envMap), _xSkip(256), _ySkip(64)
    {
        _delayBind = true;
    };
    PrecomputeMap(EnvMap & envMap, float radius, int n, int m) :
        EnvMap(radius, n, m), _envMap(envMap), _xSkip(256), _ySkip(64)
    {
        _delayBind = true;
    };

    void useCache(std::string filename) { _cached = true; _filename = filename; }
    void disableCache() { _cached = false; }

    void setXSkip(int xSkip) { _xSkip = xSkip; }
    void setYSkip(int ySkip) { _ySkip = ySkip; }

    std::string mapType() { return "Precompute"; }

protected:
    virtual int _readMap();
    virtual void _precomputeMap() = 0;

    EnvMap & _envMap;
    bool _cached;
    int _xSkip, _ySkip;
};

class InterpolateMap : public PrecomputeMap
{
public:
    InterpolateMap(EnvMap & envMap, int w, int h) : _newWidth(w), _newHeight(h), PrecomputeMap(envMap) {};

    std::string mapType() { return "Interpolate"; }

private:
    int _newWidth;
    int _newHeight;
protected:
    void _precomputeMap();
};

class EnvShader : public Shader
{
public:
    EnvShader(EnvMap * envMap, std::string vertfile, std::string fragfile) :
        Shader(vertfile, fragfile), _envMap(envMap) { };

    void link();
    void unlink();

private:
    EnvMap * _envMap;
};

class CookTorranceIcosMap : public PrecomputeMap
{
public:
    CookTorranceIcosMap(EnvMap & envMap,
        float r1, float r2, int i, std::string filename) :
        _roughness(r1), _reflCoeff(r2), _vertexIndex(i), PrecomputeMap(envMap)
    {
        useCache(filename);
        setXSkip(1);
        setYSkip(1);
    };

    std::string mapType() { return "CookTorrance"; }

    static std::string CookTorranceIcosMap::getCtIcosMapName(int);
private:
    float _roughness;
    float _reflCoeff;
    int _vertexIndex;
protected:
    void _precomputeMap();
};


class DiffuseEnvMap : public PrecomputeMap
{
public:
    DiffuseEnvMap(EnvMap & envMap) : PrecomputeMap(envMap) {};
    DiffuseEnvMap(EnvMap & envMap, float radius, int n, int m) : PrecomputeMap(envMap, radius, n, m) {};

    std::string mapType() { return "Diffuse"; }

protected:
    void _precomputeMap();
};

class PhongEnvMap : public PrecomputeMap
{
public:
    PhongEnvMap(EnvMap & envMap) : PrecomputeMap(envMap), _s(15) {};
    PhongEnvMap(EnvMap & envMap, float radius, int n, int m) : PrecomputeMap(envMap, radius, n, m), _s(15) {};

    void setSpecCoeffecient(int s) { _s = s; }
    std::string mapType() { return "Phong"; }

protected:
    int _s;

    void _precomputeMap();
};


class CtShader : public Shader
{
public:
    CtShader(std::vector<Scene::RadMap *> & radMaps,
        std::string vertfile, std::string fragfile) :
        Shader(vertfile, fragfile),
        _radMaps(radMaps), _diffuseMap(nullptr) { };

    CtShader(std::vector<Scene::RadMap *> & radMaps,
        DiffuseEnvMap * diffuseMap,
        std::string vertfile, std::string fragfile) :
        Shader(vertfile, fragfile),
        _radMaps(radMaps), _diffuseMap(diffuseMap) { };

    void link();

private:
    std::vector<Scene::RadMap *> & _radMaps;
    DiffuseEnvMap * _diffuseMap;
};

};
