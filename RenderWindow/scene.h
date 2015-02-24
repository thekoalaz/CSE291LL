#include "stdafx.h"
#include "GlutDraw.h"
extern "C" {
    #include "rgbe.h"
}

#define DEFAULT_ENV_MAP "./grace-new.hdr"

namespace Scene
{
/** Global variables **/
//const float GOLDEN_RATIO = (1 + sqrt(5)) / 2;
//const float ICOSAHEDRON_VERTS[][3] = {
//    {  0,  1,  GOLDEN_RATIO },
//    {  0, -1,  GOLDEN_RATIO },
//    {  0,  1, -GOLDEN_RATIO },
//    {  0, -1, -GOLDEN_RATIO },
//    {  1,  GOLDEN_RATIO,  0 },
//    { -1,  GOLDEN_RATIO,  0 },
//    {  1, -GOLDEN_RATIO,  0 },
//    { -1, -GOLDEN_RATIO,  0 },
//    {  1,  0,  GOLDEN_RATIO },
//    { -1,  0,  GOLDEN_RATIO },
//    {  1,  0, -GOLDEN_RATIO },
//    { -1,  0, -GOLDEN_RATIO }
//};

class World;

class Shader
    /* Base class for vert/frag shader. */
{
public:
/* Constructors */
    Shader() : _vertfile(), _fragfile() { };
    Shader(std::string vertfile, std::string fragfile)
        : _vertfile(vertfile), _fragfile(fragfile)
        { _initShaders(); };

    void link();
    void unlink();
    GLuint getProgram() { return _program; };

/* Destructors */
    ~Shader() { glDeleteProgram(_program); }

private:
    std::string _vertfile, _fragfile;
    GLuint _program;
    GLuint _vertex;
    GLuint _frag;
    bool _initialized;

    void _initShaders();
};

class Object
{
public:
/* Constructors */
    Object() : _tx(0), _ty(0), _tz(0), _rotx(0), _roty(0), _rotz(0), _visible(true)
        { _objectId = nextId(); }
    Object(double tx, double ty, double tz, double rotx, double roty, double rotz) : _tx(tx), _ty(ty), _tz(tz),
        _rotx(rotx), _roty(roty), _rotz(rotz), _visible(true)
        { _objectId = nextId(); }
    void draw();
    void draw(Shader *);
    virtual void doDraw() = 0;

    /* getters */
    double getTx() { return _tx; } const
    double getTy() { return _ty; } const
    double getTz() { return _tz; } const
    double getRotx() { return _rotx; } const
    double getRoty() { return _roty; } const
    double getRotz() { return _rotz; } const
    bool getVisible() { return _visible; } const
    World* getWorld() { return _world; } const
    int getId() { return _objectId; } const

    /* setters */
    void setTx(double tx) { _tx = tx; }
    void setTy(double ty) { _ty = ty; }
    void setTz(double tz) { _tz = tz; }
    void setRotx(double rotx) { _rotx = rotx; }
    void setRoty(double roty) { _roty = roty; }
    void setRotz(double rotz) { _rotz = rotz; }
    void setVisible(bool visible) { _visible = visible; }
    void setWorld(World * world) { _world = world; }

    /* Single line functions */
    int nextId() { return NEXTID++; }

protected:
    World * _world;
    int _objectId;
    double _tx, _ty, _tz;
    double _rotx, _roty, _rotz;
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
    Grid() : Object(), _rows(10), _cols(10), _gap(1.0) { }
    Grid(int rows, int cols, double gap) : Object(),
        _rows(rows), _cols(cols), _gap(gap) { }

    void doDraw();

private:
    int _rows, _cols;
    double _gap;
};

class Sphere : public Object
{
public:
    /* Constructors */
    Sphere() : Object(), _r(5), _n(100), _m(100) { }
    Sphere(double radius, int n, int m) : Object(), _r(radius), _n(n), _m(m) { }

    void doDraw();

protected:
    int _n, _m; // number of theta and phi subdivisions respectively
    double _r;
};

class EnvMap : public Sphere
{
public:
/* Constructors */
    EnvMap() : Sphere(1000.0, 20, 20), _fileName(DEFAULT_ENV_MAP), _mapReady(false) { };
    EnvMap(double radius, int n, int m) : Sphere(radius, n, m), _fileName(DEFAULT_ENV_MAP), _mapReady(false) {};

    virtual void doDraw();
    std::tuple<float, float, float> map(const double, const double);
    std::tuple<float, float, float> getColor(const double, const double);
    void bind();
    void unbind();

    const float _getPixelR(int x, int y) { return _data[(x + y * _width) + 0]; };
    const float _getPixelR(double x, double y) { return _bilinearInterpolate(&_data[0], x, y); };
    const float _getPixelG(int x, int y) { return _data[(x + y * _width) + 1]; };
    const float _getPixelG(double x, double y) { return _bilinearInterpolate(&_data[1], x, y); };
    const float _getPixelB(int x, int y) { return _data[(x + y * _width) + 2]; };
    const float _getPixelB(double x, double y) { return _bilinearInterpolate(&_data[2], x, y); };
    const int _getWidth() { return _width; };
    const int _getHeight() { return _height; };

/* Destructors */
    ~EnvMap() { if(_data != nullptr) delete _data; }

protected:
    virtual void _readMap();
    float * _data;
    int _width, _height;
    GLuint _textureID;
private:
    std::string _fileName;
    bool _mapReady;

    float _bilinearInterpolate(const float * _colors, const double x, const double y);
};

class DiffuseEnvMap : public EnvMap
{
public:
    DiffuseEnvMap(EnvMap & envMap) : EnvMap(), _envMap(envMap) {};
    DiffuseEnvMap(EnvMap & envMap, double radius, int n, int m) : EnvMap(radius, n, m), _envMap(envMap) {};

protected:
    void _readMap();

private:
    EnvMap & _envMap;
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

    void draw();
private:
    std::vector<Object *> _objects;
    std::unordered_map<int, Shader *> _shaderMap;

    Camera * _cam;
    EnvMap * _envMap;
};

World & createWorld();

};
