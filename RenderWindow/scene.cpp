#include "scene.h"

using namespace Scene;
/** Global variables **/
int Object::NEXTID = 0;
int EnvMap::NEXTTEXTUREID = 0;

/* Utility Functions */
char * textFileRead(const char * fn);

/* Method Definitions */
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
    _objects.push_back(envMap);
    envMap->setWorld(this);
    _envMaps.push_back(envMap);

    if (_envMap == nullptr)
    {
        setEnvMap(envMap);
    }
    else
    {
        //std::cout << "Env map already set!" << std::endl;
    }
}

void World::setEnvMap(unsigned int index)
{
    if (index < _envMaps.size()) _envMap = _envMaps[index];
}

void World::setEnvMap(EnvMap * envMap)
{
    _envMap = envMap;
}

void World::assignShader(Object * obj, Shader * shader)
{
    _shaderMap[obj->getID()] = shader;
}
Shader * World::findShader(Object * obj)
{
    return _shaderMap[obj->getID()];
}

void World::draw()
{
    if (_envMap != nullptr) { _envMap->bind(); }

    for(auto &object : _objects)
    {
        auto shader = _shaderMap.find(object->getID());
        if (shader != _shaderMap.end())
        {
            object->draw(_shaderMap[object->getID()]);
        }
        else
        {
            object->draw();
        }
    }

    if (_envMap != nullptr) { _envMap->unbind(); }
}

void Object::draw()
{
    if (!_visible)
    {
        return;
    }

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
    if (!_visible)
    {
        return;
    }

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
        GlutDraw::drawLine(-(_cols/2.0f)*_gap, 0, r*_gap,
            (_cols/2.0f)*_gap, 0, r*_gap);
    }
    for(int c = -(_cols/2); c <= (_cols/2); c++)
    {
        GlutDraw::drawLine(c*_gap, 0, -(_rows/2.0f)*_gap,
            c*_gap, 0, (_rows/2.0f)*_gap);
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

    std::cout << "Env map " << _filename << " has width: " << _width << " height: " << _height << std::endl;

    glGenTextures(1, &_textureID);
    glBindTexture(GL_TEXTURE_2D, _textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _width, _height, 0, GL_RGB, GL_FLOAT, _data);

    _mapReady = true;

    return _textureID;
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

void Sphere::doDraw()
{
    GlutDraw::drawSphere(_r,_n,_m);
}

void ObjGeometry::doDraw()
{
    if (!_geomReady)
    {
        _readGeom();
    }
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, &_vertices[0]);
    glNormalPointer(GL_FLOAT, 0, &_normals[0]);

    //check_gl_error();
    glDrawArrays(GL_TRIANGLES, 0, _vertices.size());

    return;
}

// Adopted from http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
int ObjGeometry::_readGeom()
{
    std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
    std::vector< glm::vec3 > tempVertices;
    std::vector< glm::vec2 > tempUVs;
    std::vector< glm::vec3 > tempNormals;
    int lineCount=0;
    int faceCount=0;
    int vertCount=0;

    std::ifstream file;
    file.open(_filename, std::ios::in);
    if (!file.is_open())
    {
        std::cout << "Could not open " << _filename << std::endl;
        return -1;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream linestream(line);
        std::string type;
        if (line.find("v ") == 0)
        {
            glm::vec3 vertex;
            linestream >> type >> vertex.x >> vertex.y >> vertex.z;
            vertex.x = vertex.x;
            vertex.y = vertex.y;
            vertex.z = vertex.z;
            tempVertices.push_back(vertex);
            vertCount++;
        }
        else if (line.find("vn ") == 0)
        {
            glm::vec3 normal;
            linestream >> type >> normal.x >> normal.y >> normal.z;
            tempNormals.push_back(normal);
        }
        else if (line.find("vt ") == 0)
        {
            glm::vec2 uv;
            linestream >> type >> uv.x >> uv.y;
            tempUVs.push_back(uv);
        }
        else if (line.find("f ") == 0)
        {
            unsigned int vertexIndex[3], normalIndex[3], uvIndex[3];
            char delim;
            linestream >> type >>
                vertexIndex[0] >> delim >> uvIndex[0] >> delim >> normalIndex[0] >>
                vertexIndex[1] >> delim >> uvIndex[1] >> delim >> normalIndex[1] >>
                vertexIndex[2] >> delim >> uvIndex[2] >> delim >> normalIndex[2];

            for (int i = 0; i < 3; i++)
            {
                vertexIndices.push_back(vertexIndex[i]);
                normalIndices.push_back(normalIndex[i]);
                uvIndices.push_back(uvIndex[i]);
            }
            faceCount++;
        }

        lineCount++;
        //if (lineCount % 1000 == 0)
        //{
        //    std::cout << "Parsing obj line: " << lineCount << "\r";
        //}
    }
    std::cout << "Parsed " << lineCount << " lines Verts: " << vertCount << " Triangles: " << faceCount << std::endl;
    file.close();

    for (unsigned int i = 0; i < vertexIndices.size(); i++)
    {
        unsigned int vertexIndex = vertexIndices[i];
        glm::vec3 vertex = tempVertices[vertexIndex - 1];
        _vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i < normalIndices.size(); i++)
    {
        unsigned int normalIndex = normalIndices[i];
        glm::vec3 normal = tempNormals[normalIndex - 1];
        _normals.push_back(normal);
    }
    for (unsigned int i = 0; i < uvIndices.size(); i++)
    {
        unsigned int uvIndex = uvIndices[i];
        glm::vec2 uv = tempUVs[uvIndex - 1];
        _uvs.push_back(uv);
    }

    _geomReady = true;

    return lineCount;
}

std::tuple<float, float, float> EnvMap::map(const float theta, const float phi)
{
    const float x = theta / M_PI + _width;
    const float y = phi / M_PI;

    return getColor(x, y);
}

std::tuple<float, float, float> EnvMap::getColor(const float x, const float y)
{
    float red = _bilinearInterpolate(&_data[0], x, y);
    float green = _bilinearInterpolate(&_data[1], x, y);
    float blue = _bilinearInterpolate(&_data[2], x, y);

    return std::tuple<float, float, float>(red, green, blue);
}

float EnvMap::_bilinearInterpolate(const float * _colors, const float x, const float y)
{
    int px = (int) x;
    int py = (int) y;

    const float p0 = _colors[3*(px + py*_width)];
    const float p1 = _colors[3*((px+1) + py*_width)];
    const float p2 = _colors[3*(px + (py+1)*_width)];
    const float p3 = _colors[3*((px+1)+ (py+1) *_width)];

    float fx1 = x + 1 - px;
    float fy1 = y + 1 - py;
    float fx2 = x - px;
    float fy2 = y - py;

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
    }
    glActiveTexture(GL_TEXTURE0 + _textureID);
    glBindTexture(GL_TEXTURE_2D, _textureID);
}

void EnvMap::unbind()
{
    //TODO
}

int PrecomputeMap::_readMap()
{
    int read = -1;

    if (_cached)
    {
        read = EnvMap::_readMap();
    }

    if (read > 0)
    {
        std::cout << "Read map cache from " << _filename << std::endl;
    }
    else
    {
        int integrationStart = glutGet(GLUT_ELAPSED_TIME);
        std::cout << "Starting integration " << mapType() << std::endl;
        _precomputeMap();

        int integrationEnd = glutGet(GLUT_ELAPSED_TIME);
        std::cout << std::endl
            << "Integration took " << ((integrationEnd - integrationStart) / 1000.0f) << "s" << std::endl;
    }

    if (_cached)
    {
        _writeMap();
    }

    glGenTextures(1, &_textureID);
    glBindTexture(GL_TEXTURE_2D, _textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _width, _height, 0, GL_RGB, GL_FLOAT, _data);

    _mapReady = true;

    return 0;
}

void InterpolateMap::_precomputeMap()
{
    _width = _newWidth;
    _height = _newHeight;
    _data = new float[3 * _width * _height];
    int w0 = _envMap._getWidth();   // old dimensions
    int h0 = _envMap._getHeight();
    int w1 = _newWidth;             // new dimensions
    int h1 = _newHeight;
    float * x = new float[w0];
    float * y = new float[h0];
    for (int i = 0; i < w0; i++) x[i] = i*(float)(w1 - 1) / (w0 - 1);
    for (int i = 0; i < h0; i++) y[i] = i*(float)(h1 - 1) / (h0 - 1);
};

void DiffuseEnvMap::_precomputeMap()
{
    _width = _envMap._getWidth();
    _height = _envMap._getHeight();
    _data = new float[3 * _width * _height];

    int xStep = 1;
    int yStep = xStep;
    float a = 2 * M_PI / (float)(_width*_height) * (float)(xStep * yStep);
    
    for (int jj = 0; jj < _height-_height%_ySkip+_ySkip ; jj += _ySkip)
    {
        int j = std::min(jj,_height-1);
        std::cout << "We're on height " << j << "/" << _height << "\r";
        float phiN = M_PI*(float)j / (float)_height;
        float yN = cos(phiN);
        for (int i = 0; i < _width; i += _xSkip)
        {
            
            float thetaN = M_PI*(2 * (float)i / (float)_width - 1);
            float xN = sin(phiN)*sin(thetaN);
            float zN = -sin(phiN)*cos(thetaN);
            float Rsum = 0;
            float Gsum = 0;
            float Bsum = 0;
            for (int l = 0; l < _height; l += yStep)
            {
                float phiE = M_PI*(float)l / (float)_height;
                float yE = cos(phiE);
                for (int k = 0; k < _width; k += xStep)
                {
                    float thetaE = M_PI*(2 * (float)k / (float)_width - 1);
                    float xE = sin(phiE)*sin(thetaE);
                    float zE = -sin(phiE)*cos(thetaE);
                    float R = _envMap._getPixelR(k, l);
                    float G = _envMap._getPixelG(k, l);
                    float B = _envMap._getPixelB(k, l);
                    float cosAng = xE*xN + yE*yN + zE*zN;
                    if (cosAng <= 0) continue;
                    Rsum += R*cosAng*sin(phiE);
                    Gsum += G*cosAng*sin(phiE);
                    Bsum += B*cosAng*sin(phiE);
                }
            }
            _setPixelR(i, j, a*Rsum);
            _setPixelG(i, j, a*Gsum);
            _setPixelB(i, j, a*Bsum);
            // if we are at the poles, set row (top or bottom) to the same value, and skip to next row j
            if (j == 0 || j == _height - 1) {
                for (int iPole = 1; iPole < _width; iPole++){
                    _setPixelR(iPole, j, a*Rsum);
                    _setPixelG(iPole, j, a*Gsum);
                    _setPixelB(iPole, j, a*Bsum);
        }
                break;
    }
        }
    }

    // interpolate if integration was not done on that patch
    for (int i = 0; i < _width; i++){
        int i1 = i - i%_xSkip;
        int i2 = i1 + _xSkip;
        float dTheta1 = 2 * M_PI * (float)(i - i1) / _width;
        float dTheta2 = 2 * M_PI * (float)(i2 - i) / _width;
        i2 = i2%_width;
        for (int j = 0; j < _height; j++){
            int j1 = j - j%_ySkip;
            int j2 = std::min(j1 + _ySkip, _height - 1);
            float phi = M_PI*(float)j / _height;
            float phi1 = M_PI*(float)j1 / _height;
            float phi2 = M_PI*(float)j2 / _height;
            float a11 = dTheta1*(cos(phi1) - cos(phi));
            float a12 = dTheta1*(cos(phi) - cos(phi2));
            float a21 = dTheta2*(cos(phi1) - cos(phi));
            float a22 = dTheta2*(cos(phi) - cos(phi2));
            if (j1 == j2) a11, a12, a21, a22 = 1;
            float A = a11 + a12 + a21 + a22;
            float R = (a22*_getPixelR(i1, j1) + a21*_getPixelR(i1, j2) + a12*_getPixelR(i2, j1) + a11*_getPixelR(i2, j2)) / A;
            float G = (a22*_getPixelG(i1, j1) + a21*_getPixelG(i1, j2) + a12*_getPixelG(i2, j1) + a11*_getPixelG(i2, j2)) / A;
            float B = (a22*_getPixelB(i1, j1) + a21*_getPixelB(i1, j2) + a12*_getPixelB(i2, j1) + a11*_getPixelB(i2, j2)) / A;
            _setPixelR(i, j, R);
            _setPixelG(i, j, G);
            _setPixelB(i, j, B);;
        }
    }
}


void PhongEnvMap::_precomputeMap()
{
    _width = _envMap._getWidth();
    _height = _envMap._getHeight();
    _data = new float[3 * _width * _height];

    int xStep = 1;
    int yStep = xStep;
    float a = (1+_s) * M_PI / (float)(_width*_height) * (float)(xStep * yStep);
    
    for (int jj = 0; jj < _height-_height%_ySkip+_ySkip ; jj += _ySkip)
    {
        int j = std::min(jj,_height-1);
        std::cout << "Integration Progress: y " << j << "\r";
        float phiN = M_PI*(float)j / (float)_height;
        float yN = cos(phiN);
        for (int i = 0; i < _width; i += _xSkip)
        {
            float thetaN = M_PI*(2 * (float)i / (float)_width - 1);
            float xN = sin(phiN)*sin(thetaN);
            float zN = -sin(phiN)*cos(thetaN);
            float Rsum = 0;
            float Gsum = 0;
            float Bsum = 0;
            for (int l = 0; l < _height; l += yStep)
            {
                float phiE = M_PI*(float)l / (float)_height;
                float yE = cos(phiE);
                for (int k = 0; k < _width; k += xStep)
                {
                    float thetaE = M_PI*(2 * (float)k / (float)_width - 1);
                    float xE = sin(phiE)*sin(thetaE);
                    float zE = -sin(phiE)*cos(thetaE);
                    float R = _envMap._getPixelR(k, l);
                    float G = _envMap._getPixelG(k, l);
                    float B = _envMap._getPixelB(k, l);
                    float cosAng = pow(xE*xN + yE*yN + zE*zN,_s);
                    if (cosAng <= 0) continue;
                    Rsum += R*cosAng*sin(phiE);
                    Gsum += G*cosAng*sin(phiE);
                    Bsum += B*cosAng*sin(phiE);
                }
            }
            _setPixelR(i, j, a*Rsum);
            _setPixelG(i, j, a*Gsum);
            _setPixelB(i, j, a*Bsum);
            // if we are at the poles, set row (top or bottom) to the same value, and skip to next row j
            if (j == 0 || j == _height - 1)
            {
                for (int iPole = 1; iPole < _width; iPole++)
                {
                    _setPixelR(iPole, j, a*Rsum);
                    _setPixelG(iPole, j, a*Gsum);
                    _setPixelB(iPole, j, a*Bsum);
                }
                break;
            }
        }
    }

    // interpolate if integration was not done on that patch
    for (int i = 0; i < _width; i++)
    {
        int i1 = i - i%_xSkip;
        int i2 = i1 + _xSkip;
        float dTheta1 = 2 * M_PI * (float)(i - i1) / _width;
        float dTheta2 = 2 * M_PI * (float)(i2 - i) / _width;
        i2 = i2%_width;
        for (int j = 0; j < _height; j++)
        {
            int j1 = j - j%_ySkip;
            int j2 = std::min(j1 + _ySkip, _height - 1);
            float phi = M_PI*(float)j / _height;
            float phi1 = M_PI*(float)j1 / _height;
            float phi2 = M_PI*(float)j2 / _height;
            float a11 = dTheta1*(cos(phi1) - cos(phi));
            float a12 = dTheta1*(cos(phi) - cos(phi2));
            float a21 = dTheta2*(cos(phi1) - cos(phi));
            float a22 = dTheta2*(cos(phi) - cos(phi2));
            if (j1 == j2) a11, a12, a21, a22 = 1;
            float A = a11 + a12 + a21 + a22;
            float R = (a22*_getPixelR(i1, j1) + a21*_getPixelR(i1, j2) + a12*_getPixelR(i2, j1) + a11*_getPixelR(i2, j2)) / A;
            float G = (a22*_getPixelG(i1, j1) + a21*_getPixelG(i1, j2) + a12*_getPixelG(i2, j1) + a11*_getPixelG(i2, j2)) / A;
            float B = (a22*_getPixelB(i1, j1) + a21*_getPixelB(i1, j2) + a12*_getPixelB(i2, j1) + a11*_getPixelB(i2, j2)) / A;
            _setPixelR(i, j, R);
            _setPixelG(i, j, G);
            _setPixelB(i, j, B);
        }
    }
}

World & Scene::createWorld()
{
    World * new_world = new World();
    return *new_world;
}

void Shader::_initShaders()
{
    if (_vertfile == "" || _fragfile == "")
    {
        std::cout << "No shaders! Initialization failing." << std::endl;
        return;
    }
    else if (_shaderReady)
    {
        std::cout << "Shader has already initialized." << std::endl;
        return;
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
        if (_checkShaderError(_vertex))
        {
            std::cout << _vertfile << " compiled successfully." << std::endl;
            glAttachShader(_program,_vertex);
        }
    }
    if (_fragfile != "")
    {
        _frag = glCreateShader(GL_FRAGMENT_SHADER);
        fs = textFileRead(_fragfile.c_str());
        const char * ff = fs;
        glShaderSource(_frag, 1, &ff, NULL);
        free(fs);
        glCompileShader(_frag);
        if (_checkShaderError(_frag))
        {
            std::cout << _fragfile << " compiled successfully." << std::endl;
            glAttachShader(_program, _frag);
        }
    }

    glLinkProgram(_program);

    glDetachShader(_program, _vertex);
    glDetachShader(_program, _frag);
    glDeleteShader(_vertex);
    glDeleteShader(_frag);

    _shaderReady = true;
    return;
}

bool Shader::_checkShaderError(GLuint shader)
{
    GLint result = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

    if (result == GL_TRUE) return true;

    GLint logsize = 0;
    char * log;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logsize);
    log = (char *) malloc(logsize + 1);
    glGetShaderInfoLog(shader, logsize, &result, log);

    std::cout << log << std::endl;
    return false;
}

void Shader::link()
{
    glUseProgram(_program);
}

void Shader::unlink()
{
    glUseProgram(0);
}

void EnvShader::link()
{
    Shader::link();
    _envMap->bind();
    GLint texLoc = glGetUniformLocation(getProgram(), "envMap");
    glUniform1i(texLoc, _envMap->_getTextureID());
}

void EnvShader::unlink()
{
    _envMap->unbind();
    Shader::unlink();
}

//Consider doing this method for index: http://stackoverflow.com/questions/10962290/find-position-of-element-in-c11-range-based-for-loop
void CtShader::link()
{
    Shader::link();
    for(auto &radMap : _radMaps)
    {
        auto index = &radMap - &_radMaps[0];
        std::string radName = RadMap::getRadMapName(index);
        radMap->bind();
        GLint radMapLocation = glGetUniformLocation(getProgram(), radName.c_str());
        std::cout << radMap->_getTextureID() << std::endl;
        glUniform1i(radMapLocation, radMap->_getTextureID());
    }
}

void CookTorranceIcosMap::_precomputeMap()
{
    _width = _envMap._getWidth();
    _height = _envMap._getHeight();
    _data = new float[3 * _width * _height];
    glm::mat3 R_alignEV;
    glm::vec3 xAxV_E = Scene::ICOS_XAXES[_vertexIndex];
    glm::vec3 yAxV_E = Scene::ICOS_YAXES[_vertexIndex];
    glm::vec3 zAxV_E = Scene::ICOS_ZAXES[_vertexIndex];
    glm::vec3 xAxE_E(1.0f, 0.0f, 0.0f);
    glm::vec3 yAxE_E(0.0f, 1.0f, 0.0f);
    glm::vec3 zAxE_E(0.0f, 0.0f, 1.0f);
    R_alignEV[0] = xAxV_E;
    R_alignEV[1] = yAxV_E;
    R_alignEV[2] = zAxV_E;
    R_alignEV = glm::transpose(R_alignEV);
    glm::vec3 V_V(0.0f, 0.0f, 1.0f);
    int halfway = static_cast<int>(std::ceil(_height / 2.0f));
    float a = 2 * M_PI*M_PI / (float)(_width*_height);
    for (int jj = 0; jj < halfway - halfway % _ySkip + _ySkip; jj += _ySkip) {
        int j = std::min(jj, halfway - 1);
        float phiN_V = M_PI*(float)j / _height;
        std::cout << "Integration Progress: y " << j << "\r";
        for (int i = 0; i < _width; i += _xSkip){
            float thetaN_V = 2 * M_PI*((float)i / _width - 1);
            glm::vec3 N_V(sin(phiN_V)*cos(thetaN_V), sin(phiN_V)*sin(thetaN_V), cos(phiN_V));
            float NdotV = N_V.z;
            float Rsum = 0;
            float Gsum = 0;
            float Bsum = 0;
            for (int k = 0; k < _width; k++){
                float thetaL_E = 2 * M_PI*((float)k / _width - 1);
                for (int l = 0; l < _height; l++){
                    float phiL_E = M_PI*(float)l / _height;
                    glm::vec3 L_E(sin(phiL_E)*sin(thetaL_E), cos(phiL_E), -sin(phiL_E)*cos(thetaL_E));
                    glm::vec3 L_V = R_alignEV*L_E;
                    glm::vec3 H_V = glm::normalize(L_V + V_V);
                    float NdotL = glm::dot(N_V,L_V);
                    if (NdotL <= 0) continue;
                    float NdotH = glm::dot(N_V,H_V);
                    float VdotH = glm::dot(V_V,H_V);
                    float LdotH = glm::dot(L_V,H_V);
                    float G = std::min(2 * NdotH*NdotV / VdotH, 2 * NdotH*NdotL / LdotH);
                    G = std::min((float)1.0f, G);
                    float D = exp((NdotH*NdotH - 1) / (_roughness*_roughness*NdotH*NdotH));
                    D /= M_PI*_roughness*_roughness*pow(NdotH, 4.0f);
                    float F = _reflCoeff + (1 - _reflCoeff)*pow(1 - VdotH, 5.0f);
                    float brdf = F*D*G / (M_PI*NdotL*NdotV);
                    float envR = _envMap._getPixelR(k, l);
                    float envG = _envMap._getPixelG(k, l);
                    float envB = _envMap._getPixelB(k, l);
                    Rsum += envR*brdf*NdotL*sin(phiL_E);
                    Gsum += envG*brdf*NdotL*sin(phiL_E);
                    Bsum += envB*brdf*NdotL*sin(phiL_E);
                }
            }
            _setPixelR(i, j, a*Rsum);
            _setPixelG(i, j, a*Gsum);
            _setPixelB(i, j, a*Bsum);
            if (j == 0)
            {
                for (int iPole = 1; iPole < _width; iPole++)
                {
                    _setPixelR(iPole, j, a*Rsum);
                    _setPixelG(iPole, j, a*Gsum);
                    _setPixelB(iPole, j, a*Bsum);
                }
                break;
            }
        }
        if (_xSkip>1 || _ySkip > 1) {
            for (int i = 0; i < _width; i++)
            {
                int i1 = i - i%_xSkip;
                int i2 = i1 + _xSkip;
                float dTheta1 = 2 * M_PI * (float)(i - i1) / _width;
                float dTheta2 = 2 * M_PI * (float)(i2 - i) / _width;
                i2 = i2%_width;
                for (int j = 0; j < halfway; j++)
                {
                    int j1 = j - j%_ySkip;
                    int j2 = std::min(j1 + _ySkip, halfway - 1);
                    float phi = M_PI*(float)j / _height;
                    float phi1 = M_PI*(float)j1 / _height;
                    float phi2 = M_PI*(float)j2 / _height;
                    float a11 = dTheta1*(cos(phi1) - cos(phi));
                    float a12 = dTheta1*(cos(phi) - cos(phi2));
                    float a21 = dTheta2*(cos(phi1) - cos(phi));
                    float a22 = dTheta2*(cos(phi) - cos(phi2));
                    if (j1 == j2) a11, a12, a21, a22 = 1;
                    float A = a11 + a12 + a21 + a22;
                    float R = (a22*_getPixelR(i1, j1) + a21*_getPixelR(i1, j2) + a12*_getPixelR(i2, j1) + a11*_getPixelR(i2, j2)) / A;
                    float G = (a22*_getPixelG(i1, j1) + a21*_getPixelG(i1, j2) + a12*_getPixelG(i2, j1) + a11*_getPixelG(i2, j2)) / A;
                    float B = (a22*_getPixelB(i1, j1) + a21*_getPixelB(i1, j2) + a12*_getPixelB(i2, j1) + a11*_getPixelB(i2, j2)) / A;
                    _setPixelR(i, j, R);
                    _setPixelG(i, j, G);
                    _setPixelB(i, j, B);
                }
            }
        }
    }
}

std::string CookTorranceIcosMap::getCtIcosMapName(int index)
{
    return zero_padded_name("ctIcos", index, 2);
    }


std::string RadMap::getRadMapName(int index)
{
    return zero_padded_name("radMap", index, 2);
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

void _check_gl_error(const char *file, int line) {
        GLenum err (glGetError());
 
        while(err!=GL_NO_ERROR) {
                std::string error;
 
                switch(err) {
                        case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
                        case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
                        case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
                        case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
                        case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
                }
 
                std::cerr << "GL_" << error.c_str() <<" - "<<file<<":"<<line<<std::endl;
                err=glGetError();
        }
}

std::string zero_padded_name(std::string prefix, int number, int pad)
{
    std::ostringstream name;
    name << prefix << std::setfill('0') << std::setw(pad) << number;
    std::setfill(' ');

    return name.str();
}
