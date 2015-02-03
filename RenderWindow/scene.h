#include "stdafx.h"
#include "GlutDraw.h"
extern "C" {
    #include "rgbe.h"
}

#define DEFAULT_ENV_MAP "./grace-new.hdr"

namespace Scene
{
/** Global variables **/

class Object;
class Shader;

class World
{
public:
    World() { }

    void addObject(Object * obj) { _objects.push_back(obj); }
    void assignShader(Object * obj, Shader * shader);

    //void removeObject(Object & obj) {  }

    void draw();
private:
    std::vector<Object *> _objects;
    std::unordered_map<int, Shader *> _shaderMap;
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
    int getId() { return _objectId; } const

    /* setters */
    void setTx(double tx) { _tx = tx; }
    void setTy(double ty) { _ty = ty; }
    void setTz(double tz) { _tz = tz; }
    void setRotx(double rotx) { _rotx = rotx; }
    void setRoty(double roty) { _roty = roty; }
    void setRotz(double rotz) { _rotz = rotz; }
    void setVisible(bool visible) { _visible = visible; }

    /* Single line functions */
    int nextId() { return NEXTID++; }
private:
    static int NEXTID;
    int _objectId;
    double _tx, _ty, _tz;
    double _rotx, _roty, _rotz;
    bool _visible;
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

    void doDraw();

private:
    int _rows, _cols;
    double _gap;
};

class Sphere : public Object
{
public:
    /* Constructors */
    Sphere() : Object(), _r(5), _n(100), _m(100) {
        /*
        _fR = new float[_n][_m];
        _fG = new float[_n][_m];
        _fB = new float[_n][_m];
        _outR = new float[_n][_m];
        _outG = new float[_n][_m];
        _outB = new float[_n][_m];
        for (int i = 0; i < n; i++){
            for (int j = 0; j < m; j++){
                _fR[i][j] = 1;
                _fB[i][j] = 1;
                _fG[i][j] = 1;
                _outR[i][j] = rand() / RAND_MAX;
                _outG[i][j] = rand() / RAND_MAX;
                _outB[i][j] = rand() / RAND_MAX;
            }
        }
        */
    }

    void doDraw();
    void setR(double r) { _r = r; }
    /*
    void setBrdfR(float* f) {
        for (int i = 0; i < n; i++){
            for (int j = 0; j < m; j++){
                _fR[i][j] = f[i][j][0];
                _fG[i][j] = f[i][j][1];
                _fB[i][j] = f[i][j][2];
            }
        }
    }
    double getN() { return _n; }
    double getM() { return _m; }
    double theta(int i) { return M_PI*(double)i / _n; }
    double phi(int j) { return 2 * M_PI*(double)j / _m; }
    double normalx(int i, int j) { return sin(theta(i))*cos(phi(j)); }
    double normaly(int i, int j) { return sin(theta(i))*sin(phi(j)); }
    double normalz(int i) { return cos(theta(i)); }
    void shade(int i, int j, double rIn, double gIn, double bIn, double thetaIn, double phiIn, double thetaOut, double phiOut) {
        cosAng = sin(thetaIn)*sin(thetaOut)*(cos(phiIn)*cos(phiOut) + sin(phiIn)*sin(phiOut)) + cos(thetaIn)*cos(thetaOut);
        _outR[i][j] = RIn*_fR[i][j]*cosAng;
        _outG[i][j] = GIn*_fG[i][j]*cosAng;
        _outB[i][j] = BIn*_fB[i][j]*cosAng;
    }
    */

private:
    int _n, _m; // number of theta and phi subdivisions respectively
    double _r;
    /*
    float* _fR; // RGB brdfs under lambertian assumption
    float* _fG;
    float* _fB;
    float* _outR; // RGB appearance of surface
    float* _outG;
    float* _outB;

    */
};

class EnvMap : public Object
{
public:
/* Constructors */
    EnvMap() : Object(), _fileName(DEFAULT_ENV_MAP) { _readMap(); };

    void doDraw();
    std::tuple<float, float, float> map(const double theta, const double phi);
    std::tuple<float, float, float> getColor(const double x, const double y);

/* Destructors */
    ~EnvMap() { if(_data != nullptr) delete _data; }

private:
    std::string _fileName;
    int _width, _height;
    float * _data;
    GLuint _textureID;

    void _readMap();
    float _bilinearInterpolate(const float * _colors, const double x, const double y);
};

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

/* Destructors */
    ~Shader() {
        glDetachShader(_program, _vertex);
        glDetachShader(_program, _frag);
        glDeleteShader(_vertex);
        glDeleteShader(_frag);
        glDeleteProgram(_program);
    }

private:
    std::string _vertfile, _fragfile;
    GLuint _program;
    GLuint _vertex;
    GLuint _frag;
    bool _initialized;

    void _initShaders();
};

World & createWorld();

};
