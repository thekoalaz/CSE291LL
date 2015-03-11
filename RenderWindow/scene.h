#include "stdafx.h"
#include "GlutDraw.h"
extern "C" {
    #include "rgbe.h"
}

#define DEFAULT_ENV_MAP "./grace-new.hdr"

namespace Scene
{
/** Global variables **/
const float GOLDEN_RATIO = (1.0f + sqrt(5.0f)) / 2.0f;
const float ICOSAHEDRON_VERTS[][3] = {
    {  0,  1.0f,  GOLDEN_RATIO },
    {  0, -1.0f,  GOLDEN_RATIO },
    {  0,  1.0f, -GOLDEN_RATIO },
    {  0, -1.0f, -GOLDEN_RATIO },
    {  1.0f,  GOLDEN_RATIO,  0 },
    { -1.0f,  GOLDEN_RATIO,  0 },
    {  1.0f, -GOLDEN_RATIO,  0 },
    { -1.0f, -GOLDEN_RATIO,  0 },
    {  GOLDEN_RATIO,  0,  1.0f },
    { -GOLDEN_RATIO,  0,  1.0f },
    {  GOLDEN_RATIO,  0, -1.0f },
    { -GOLDEN_RATIO,  0, -1.0f }
};

const glm::vec3 ICOS_ZAXES[] = {
    glm::normalize(glm::vec3(0, 1.0f, GOLDEN_RATIO)),
    glm::normalize(glm::vec3(0, -1.0f, GOLDEN_RATIO)),
    glm::normalize(glm::vec3(0, 1.0f, -GOLDEN_RATIO)),
    glm::normalize(glm::vec3(0, -1.0f, -GOLDEN_RATIO)),
    glm::normalize(glm::vec3(1.0f, GOLDEN_RATIO, 0)),
    glm::normalize(glm::vec3(-1.0f, GOLDEN_RATIO, 0)),
    glm::normalize(glm::vec3(1.0f, -GOLDEN_RATIO, 0)),
    glm::normalize(glm::vec3(-1.0f, -GOLDEN_RATIO, 0)),
    glm::normalize(glm::vec3(1.0f, 0, GOLDEN_RATIO)),
    glm::normalize(glm::vec3(-1.0f, 0, GOLDEN_RATIO)),
    glm::normalize(glm::vec3(1.0f, 0, -GOLDEN_RATIO)),
    glm::normalize(glm::vec3(-1.0f, 0, -GOLDEN_RATIO))
};
const glm::vec3 ICOS_YAXES[] = {
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(-1.0f, 0.0f, 0.0f),
    glm::vec3(-1.0f, 0.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, -1.0f, 0.0f),
    glm::vec3(0.0f, -1.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
};
const glm::vec3 ICOS_XAXES[] = {
    glm::cross(ICOS_YAXES[0], ICOS_ZAXES[0]),
    glm::cross(ICOS_YAXES[1], ICOS_ZAXES[1]),
    glm::cross(ICOS_YAXES[2], ICOS_ZAXES[2]),
    glm::cross(ICOS_YAXES[3], ICOS_ZAXES[3]),
    glm::cross(ICOS_YAXES[4], ICOS_ZAXES[4]),
    glm::cross(ICOS_YAXES[5], ICOS_ZAXES[5]),
    glm::cross(ICOS_YAXES[6], ICOS_ZAXES[6]),
    glm::cross(ICOS_YAXES[7], ICOS_ZAXES[7]),
    glm::cross(ICOS_YAXES[8], ICOS_ZAXES[8]),
    glm::cross(ICOS_YAXES[9], ICOS_ZAXES[9]),
    glm::cross(ICOS_YAXES[10], ICOS_ZAXES[10]),
    glm::cross(ICOS_YAXES[11], ICOS_ZAXES[11])
};


class World;

class Shader
    /* Base class for vert/frag shader. */
{
public:
/* Constructors */
    Shader() : _vertfile(), _fragfile(), _shaderReady(false) { };
    Shader(std::string vertfile, std::string fragfile)
        : _vertfile(vertfile), _fragfile(fragfile), _shaderReady(false)
        { _initShaders(); };

    virtual void link();
    virtual void unlink();
    GLuint getProgram() { return _program; };

/* Destructors */
    ~Shader() { glDeleteProgram(_program); }

private:
    std::string _vertfile, _fragfile;
    GLuint _program;
    GLuint _vertex;
    GLuint _frag;
    bool _shaderReady;

    void _initShaders();
    bool _checkShaderError(GLuint);
};

class Object
{
public:
/* Constructors */
    Object() : _tx(0), _ty(0), _tz(0), _rotx(0), _roty(0), _rotz(0), _visible(true)
        { _objectID = nextID(); }
    Object(float tx, float ty, float tz, float rotx, float roty, float rotz) : _tx(tx), _ty(ty), _tz(tz),
        _rotx(rotx), _roty(roty), _rotz(rotz), _visible(true)
        { _objectID = nextID(); }
    void draw();
    void draw(Shader *);
    virtual void doDraw() = 0;

    /* getters */
    float getTx() { return _tx; } const
    float getTy() { return _ty; } const
    float getTz() { return _tz; } const
    float getRotx() { return _rotx; } const
    float getRoty() { return _roty; } const
    float getRotz() { return _rotz; } const
    bool getVisible() { return _visible; } const
    World* getWorld() { return _world; } const
    int getID() { return _objectID; } const

    /* setters */
    void setTx(float tx) { _tx = tx; }
    void setTy(float ty) { _ty = ty; }
    void setTz(float tz) { _tz = tz; }
    void setRotx(float rotx) { _rotx = rotx; }
    void setRoty(float roty) { _roty = roty; }
    void setRotz(float rotz) { _rotz = rotz; }
    void setVisible(bool visible) { _visible = visible; }
    void setWorld(World * world) { _world = world; }

    /* Single line functions */
    int nextID() { return NEXTID++; }

protected:
    World * _world;
    int _objectID;
    float _tx, _ty, _tz;
    float _rotx, _roty, _rotz;
    bool _visible;

private:
    static int NEXTID;
};

class Camera : public Object
{
public:
/* Constructors */
    Camera() : Object() { }

    void doDraw();

private:
};

class Grid : public Object
{
public:
/* Constructors */
    Grid() : Object(), _rows(10), _cols(10), _gap(1.0f) { }
    Grid(int rows, int cols, float gap) : Object(),
        _rows(rows), _cols(cols), _gap(gap) { }

    void doDraw();

private:
    int _rows, _cols;
    float _gap;
};

class Sphere : public Object
{
public:
    /* Constructors */
    Sphere() : Object(), _r(5), _n(100), _m(100) { }
    Sphere(float radius, int n, int m) : Object(), _r(radius), _n(n), _m(m) { }

    void doDraw();

protected:
    int _n, _m; // number of theta and phi subdivisions respectively
    float _r;
};

class EnvMap : public Sphere
{
public:
/* Constructors */
    EnvMap() :
        Sphere(1000.0f, 20, 20), _filename(DEFAULT_ENV_MAP), _mapReady(false) {
        //_textureID = nextTextureID();
    }
    EnvMap(std::string  filename) :
        Sphere(1000.0f, 20, 20), _filename(filename), _mapReady(false) {
        //_textureID = nextTextureID();
    }
    EnvMap(float radius, int n, int m) :
        Sphere(radius, n, m), _filename(DEFAULT_ENV_MAP), _mapReady(false) {
        //_textureID = nextTextureID();
    }
    EnvMap(std::string  filename, float radius, int n, int m) :
        Sphere(radius, n, m), _filename(filename), _mapReady(false) {
        //_textureID = nextTextureID();
    }

    virtual void doDraw();
    std::tuple<float, float, float> map(const float, const float);
    std::tuple<float, float, float> getColor(const float, const float);
    void bind();
    void unbind();

    virtual std::string mapType() { return "Env"; }

    const float _getPixelR(int x, int y) { return _data[(x + y * _width)*3 + 0]; };
    const float _getPixelG(int x, int y) { return _data[(x + y * _width)*3 + 1]; };
    const float _getPixelB(int x, int y) { return _data[(x + y * _width)*3 + 2]; };
    const float _getPixelR(float x, float y) { return _bilinearInterpolate(&_data[0], x, y); };
    const float _getPixelG(float x, float y) { return _bilinearInterpolate(&_data[1], x, y); };
    const float _getPixelB(float x, float y) { return _bilinearInterpolate(&_data[2], x, y); };
    const int _getWidth() { return _width; };
    const int _getHeight() { return _height; };
    const int _getTextureID() { return _textureID; };
    int nextTextureID() { return NEXTTEXTUREID++; };

/* Destructors */
    ~EnvMap() { if(_data != nullptr) delete _data; }

protected:
    bool _mapReady;
    virtual int _readMap();
    int _writeMap();
    int _writeMap(std::string filename);
    void _setPixelR(int x, int y, float c) { _data[(x + y * _width)*3 + 0] = c; };
    void _setPixelG(int x, int y, float c) { _data[(x + y * _width)*3 + 1] = c; };
    void _setPixelB(int x, int y, float c) { _data[(x + y * _width)*3 + 2] = c; };

    float * _data;
    int _width, _height;
    GLuint _textureID;
    std::string _filename;
    
private:
    float _bilinearInterpolate(const float * _colors, const float x, const float y);
    static int NEXTTEXTUREID;
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

class CtShader : public Shader
{
public:
    CtShader(std::vector<Scene::RadMap *> & radMaps, std::string vertfile, std::string fragfile) :
        Shader(vertfile, fragfile), _radMaps(radMaps) { };

    void link();

private:
    std::vector<Scene::RadMap *> & _radMaps;
};


class PrecomputeMap : public EnvMap
{
public:
    PrecomputeMap(EnvMap & envMap) : EnvMap(), _envMap(envMap), _xSkip(256), _ySkip(64) {};
    PrecomputeMap(EnvMap & envMap, float radius, int n, int m) :
        EnvMap(radius, n, m), _envMap(envMap), _xSkip(256), _ySkip(64) {};

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

class CookTorranceMap : public PrecomputeMap
{
public:
    CookTorranceMap(EnvMap & envMap, float r1, float r2, glm::vec3 v) :
        _roughness(r1), _reflCoeff(r2), _zAxis(v), PrecomputeMap(envMap) {};
    CookTorranceMap(EnvMap & envMap, float radius, int n, int m) : _roughness(0.3f), _reflCoeff(0.8f),
        _zAxis(glm::vec3(Scene::ICOSAHEDRON_VERTS[1][0], Scene::ICOSAHEDRON_VERTS[1][1], Scene::ICOSAHEDRON_VERTS[1][2])),
        PrecomputeMap(envMap, radius, n, m){};

private:
    float _roughness;
    float _reflCoeff;
    glm::vec3 _xAxis;
    glm::vec3 _yAxis;
    glm::vec3 _zAxis; // in the coordinates of envMap
protected:
    void _precomputeMap();
};


class CookTorranceIcosMap : public PrecomputeMap
{
public:
    CookTorranceIcosMap(EnvMap & envMap, float r1, float r2, int i) : _roughness(r1), _reflCoeff(r2), _vertexIndex(i), PrecomputeMap(envMap) {};

    static std::string CookTorranceIcosMap::getCtIcosMapName(int);

    std::string mapType() { return "CookTorrance"; }

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
    DiffuseEnvMap(EnvMap & envMap, float radius, int n, int m) : PrecomputeMap(envMap, radius, n, m){};

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

class ObjGeometry : public Object
{
public:
    ObjGeometry(std::string filename) : Object() { _filename = filename; };
    void doDraw();

    ~ObjGeometry()
    {
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &_vertexArrayID);
    }

private:
    bool _geomReady;
    int _readGeom();

    std::string _filename;
    std::vector<glm::vec3> _vertices;
    std::vector<glm::vec3> _normals;
    std::vector<glm::vec2> _uvs;

    GLuint _vertexArrayID;
};

class World
{
public:
    World() : _cam(nullptr), _envMap(nullptr) { }

    void addObject(Object *);
    void addObject(Camera *);
    void addObject(EnvMap *);
    void assignShader(Object *, Shader *);
    Shader * findShader(Object *);

    //void removeObject(Object & obj) {  }

    Camera * getCam() { return _cam; }
    EnvMap * getEnvMap() { return _envMap; }
    void setEnvMap(unsigned int);
    void setEnvMap(EnvMap *);

    void draw();
private:
    std::vector<Object *> _objects;
    std::unordered_map<int, Shader *> _shaderMap;
    std::vector<EnvMap *> _envMaps;

    Camera * _cam;
    EnvMap * _envMap;
};

World & createWorld();

};

// Utility functions
char * textFileRead(const char);

// From http://blog.nobel-joergensen.com/2013/01/29/debugging-opengl-using-glgeterror/
#ifndef GLERROR_H
#define GLERROR_H
 
void _check_gl_error(const char *file, int line);
 
///
/// Usage
/// [... some opengl calls]
/// glCheckError();
///
#define check_gl_error() _check_gl_error(__FILE__,__LINE__)
 
#endif // GLERROR_H

std::string zero_padded_name(std::string, int, int);
