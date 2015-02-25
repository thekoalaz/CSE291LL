#include "scene.h"

using namespace Scene;
/** Global variables **/
int Object::NEXTID = 0;

/* Utility Functions */
char * textFileRead(const char * fn);

void World::addObject(Object * obj)
{
    _objects.push_back(obj);
    obj->setWorld(this);
}

void World::addObject(Camera * cam)
{
    if (_cam == nullptr)
    {
        _objects.push_back(cam);
        cam->setWorld(this);
        _cam = cam;
    }
    else
    {
        std::cout << "Cam already set!" << std::endl;
    }
}

void World::addObject(EnvMap * envMap)
{
    if (_envMap == nullptr)
    {
        _objects.push_back(envMap);
        envMap->setWorld(this);
        _envMap = envMap;
    }
    else
    {
        std::cout << "Env map already set!" << std::endl;
        _objects.push_back(envMap);
        envMap->setWorld(this);
    }
}

void World::assignShader(Object * obj, Shader * shader)
{
    _shaderMap[obj->getId()] = shader;
}
Shader * World::findShader(Object * obj)
{
    return _shaderMap[obj->getId()];
}

void World::draw()
{
    for(std::vector<Object *>::const_iterator object = _objects.begin() ;
        object < _objects.end() ; object++)
    {
        auto shader = _shaderMap.find((*object)->getId());
        if (shader != _shaderMap.end())
        {
            (*object)->draw(_shaderMap[(*object)->getId()]);
        }
        else
        {
            (*object)->draw();
        }
    }
}

void Object::draw()
{
    glPushMatrix();
    glTranslated(_tx, _ty, _tz);
    glRotated(_rotx,1,0,0);
    glRotated(_roty,0,1,0);
    glRotated(_rotz,0,0,1);
    doDraw();
    glPopMatrix();
}

void Object::draw(Shader * shader)
{
    glPushMatrix();
    glTranslated(_tx, _ty, _tz);
    glRotated(_rotx,1,0,0);
    glRotated(_roty,0,1,0);
    glRotated(_rotz,0,0,1);

    shader->link();
    doDraw();
    shader->unlink();
    glPopMatrix();
}


void Camera::doDraw()
{
}

void Grid::doDraw()
{
    for(int r = -(_rows/2); r <= (_rows/2); r++)
    {
        GlutDraw::drawLine(-(_cols/2.)*_gap, 0, r*_gap,
            (_cols/2.)*_gap, 0, r*_gap);
    }
    for(int c = -(_cols/2); c <= (_cols/2); c++)
    {
        GlutDraw::drawLine(c*_gap, 0, -(_rows/2.)*_gap,
            c*_gap, 0, (_rows/2.)*_gap);
    }
}

int EnvMap::_readMap()
{
    FILE* hdrfile;
    fopen_s(&hdrfile, _filename.c_str(), "rb");
    if (hdrfile != nullptr)
    {
        // Read header
        RGBE_ReadHeader(hdrfile, &_width, &_height, NULL);
        // Read data
        _data = new float[3 * _width * _height];
        RGBE_ReadPixels_RLE(hdrfile, _data, _width, _height);
        fclose(hdrfile);
    }
    else
    {
        return -1;
    }

    glGenTextures(1, &_textureID);
    glBindTexture(GL_TEXTURE_2D, _textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _width, _height, 0, GL_RGB, GL_FLOAT, _data);

    return 0;
}

int EnvMap::_writeMap()
{
    return _writeMap(_filename);
}

int EnvMap::_writeMap(std::string filename)
{
    FILE* hdrfile;
    fopen_s(&hdrfile, filename.c_str(), "wb");
    if (hdrfile != nullptr)
    {
        // Write header
        RGBE_WriteHeader(hdrfile, _width, _height, NULL);
        // Write data
        RGBE_WritePixels_RLE(hdrfile, _data, _width, _height);
        fclose(hdrfile);
    }
    else
    {
        return -1;
    }
    return 0;
}

void EnvMap::doDraw()
{
    bind();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D , _textureID);
    GLUquadric* quad = gluNewQuadric(); 
    gluQuadricTexture(quad,GL_TRUE); 

    gluSphere(quad,_r,_n,_m);

    glDisable(GL_TEXTURE_2D);
    unbind();
}

std::tuple<float, float, float> EnvMap::map(const double theta, const double phi)
{
    const double x = theta / M_PI + _width;
    const double y = phi / M_PI;

    return getColor(x, y);
}

std::tuple<float, float, float> EnvMap::getColor(const double x, const double y)
{
    float red = _bilinearInterpolate(&_data[0], x, y);
    float green = _bilinearInterpolate(&_data[1], x, y);
    float blue = _bilinearInterpolate(&_data[2], x, y);

    return std::tuple<float, float, float>(red, green, blue);
}

float EnvMap::_bilinearInterpolate(const float * _colors, const double x, const double y)
{
    int px = (int) x;
    int py = (int) y;

    const float p0 = _colors[3*(px + py*_width)];
    const float p1 = _colors[3*((px+1) + py*_width)];
    const float p2 = _colors[3*(px + (py+1)*_width)];
    const float p3 = _colors[3*((px+1)+ (py+1) *_width)];

    double fx1 = x + 1 - px;
    double fy1 = y + 1 - py;
    double fx2 = x - px;
    double fy2 = y - py;

    return static_cast<float>( (p0 * fx1 * fy1)
        + (p1 * fx2 * fy1)
        + (p2 * fx1 * fy2)
        + (p3 * fx2 * fy2) );
}

void EnvMap::bind()
{
    if (!_mapReady)
    {
        _readMap();
        _mapReady = true;
    }
//    glActiveTexture(GL_TEXTURE0 + 0);
//    glBindTexture(GL_TEXTURE_2D, _textureID);
}

void EnvMap::unbind()
{
}

int DiffuseEnvMap::_readMap()
{
    int read;

    if (_cached)
    {
        read = EnvMap::_readMap();
    }
    if (read == 0)
    {
        std::cout << "Read diffuse map cache from " << _filename << std::endl;
    }
    else
    {
        _width = _envMap._getWidth();
        _height = _envMap._getHeight();
        _data = new float[3 * _width * _height];

        double normal[3];
        double intvec[3];
        for (int i = 0; i < _width; i++)
        {
            std::cout << "We're on x " << i << "\r";
            double theta = M_PI*(i - 1) / _width;
            for (int j = 0; j < _height; j++)
            {
                double phi = M_PI*j / _height;
                normal[0] = sin(phi)*cos(theta);
                normal[1] = sin(phi)*sin(theta);
                normal[2] = cos(phi);
                _setPixelR(i, j, 0);
                _setPixelG(i, j, 0);
                _setPixelB(i, j, 0);
                int skip = 256;
                for (int k = 0; k < _width; k += skip)
                {
                    theta = M_PI*(k - 1) / _width;
                    for (int l = 0; l < _height; l += skip)
                    {
                        phi = M_PI*l / _height;
                        intvec[0] = sin(phi)*cos(theta);
                        intvec[1] = sin(phi)*sin(theta);
                        intvec[2] = cos(phi);
                        double R = _envMap._getPixelR(k, l);
                        double G = _envMap._getPixelG(k, l);
                        double B = _envMap._getPixelB(k, l);
                        double cosAng = std::max(0.0, normal[0] * intvec[0] +
                            normal[1] * intvec[1] + normal[2] * intvec[2]);
                        if (cosAng != 0.0)
                        {
                            _setPixelR(i, j,
                                _getPixelR(i, j) + R*cosAng*sin(phi) * 2 * M_PI*M_PI / (_width*_height) * (skip * skip));
                            _setPixelG(i, j,
                                _getPixelG(i, j) + G*cosAng*sin(phi) * 2 * M_PI*M_PI / (_width*_height) * (skip * skip));
                            _setPixelB(i, j,
                                _getPixelB(i, j) + B*cosAng*sin(phi) * 2 * M_PI*M_PI / (_width*_height) * (skip * skip));
                        }
                    }
                }
                _setPixelR(i, j, _getPixelR(i, j) / M_PI);
                _setPixelG(i, j, _getPixelG(i, j) / M_PI);
                _setPixelB(i, j, _getPixelB(i, j) / M_PI);
            }
        }
        std::cout << std::endl;

        if (_cached)
        {
            _writeMap();
        }
    

        glGenTextures(1, &_textureID);
        glBindTexture(GL_TEXTURE_2D, _textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _width, _height, 0, GL_RGB, GL_FLOAT, _data);
    }
    return 0;
}

World & Scene::createWorld()
{
    World * new_world = new World();
    return *new_world;
}

void Sphere::doDraw()
{
    Shader * shader = _world->findShader(this);
    EnvMap * envMap = _world->getEnvMap();

//    envMap->bind(0);

    GlutDraw::drawSphere(_r,_n,_m);

//    envMap->unbind();
}

void Shader::_initShaders()
{
    bool no_shaders = false;
    if (_vertfile == "")
    {
        no_shaders = true;
    }
    if (_fragfile == "")
    {
        no_shaders = true;
    }
    if (no_shaders)
    {
        std::cout << "No shaders! Initialization failing." << std::endl;
        return;
    }
    else if (_initialized)
    {
        std::cout << "Already initialized." << std::endl;
    }
    else
    {
        _initialized = true;
    }

    char *vs,*fs;

    if (_vertfile == "" && _fragfile == ""){ return;  }
    _program = glCreateProgram();

    if (_vertfile != "")
    {
        _vertex = glCreateShader(GL_VERTEX_SHADER);
        vs = textFileRead(_vertfile.c_str());
        const char * vv = vs;
        glShaderSource(_vertex, 1, &vv,NULL);
        free(vs);
        glCompileShader(_vertex);
        glAttachShader(_program,_vertex);
    }

    if (_fragfile != "")
    {
        _frag = glCreateShader(GL_FRAGMENT_SHADER);
        fs = textFileRead(_fragfile.c_str());
        const char * ff = fs;
        glShaderSource(_frag, 1, &ff, NULL);
        free(fs);
        glCompileShader(_frag);
        glAttachShader(_program, _frag);
    }

    glLinkProgram(_program);

    glDetachShader(_program, _vertex);
    glDetachShader(_program, _frag);
    glDeleteShader(_vertex);
    glDeleteShader(_frag);
}

void Shader::link()
{
    glUseProgram(_program);
}

void Shader::unlink()
{
    glUseProgram(0);
}

/* Utility Functions */
char * textFileRead(const char * fn)
{
    FILE * fp;
    char * content = nullptr;

    int count = 0;
    if (fn != nullptr)
    {
        fopen_s(&fp, fn, "rt");
        if (fp != nullptr)
        {
            fseek(fp, 0, SEEK_END);
            count = ftell(fp);
            rewind(fp);
            if (count > 0)
            {
                content = (char *) malloc(sizeof(char) * (count + 1));
                count = fread(content, sizeof(char), count, fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
    }
    return content;
}

/*
// (x,y,z) is orientation of the patch (e.g. icosahedral directions)
// compute radiance map in coordinates of envMap for this patch
warpEnvMap(double x, double y, double z) {
    double mag = sqrt(x*x + y*y + z*z);
    double xN = x / mag;
    double yN = y / mag;
    double zN = z / mag;
    for (i = 0, ; i < _width; i++){
        double thetaR = M_PI*(i - 1) / _width;
        for (j = 0; j < _height; j++){
            double phiR = M_PI*j / _height;
            double xR = sin(phiR)*cos(thetaR);
            double yR = sin(phiR)*sin(thetaR);
            double zR = cos(phi);
            double f = xN*xR + yN*yR + zN*zR - 1;
            double xI = f*xN - xR;
            double yI = f*yN - yR;
            double zI = f*zN - zR;
            double phiI = acos(zI);
            double thetaI = atan2(yI, xI);
            double px = 1 + _width*thetaI / M_PI;
            double py = _height*phiI / M_PI;
            _data[px*_width + py][0];
            _data[px*_width + py][1];
            _data[px*_width + py][2];
        }
    }
}
*/