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
        setEnvMap(envMap);
    }
    else
    {
        std::cout << "Env map already set!" << std::endl;
        _objects.push_back(envMap);
        envMap->setWorld(this);
    }
}

void World::setEnvMap(EnvMap * envMap)
{
    _envMap = envMap;
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
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, _textureID);
}

void EnvMap::unbind()
{
}

int PrecomputeMap::_readMap()
{
    int read = -1;

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
        int integrationStart = glutGet(GLUT_ELAPSED_TIME);
        std::cout << "Starting integration." << std::endl;
        _precomputeMap();

        int integrationEnd = glutGet(GLUT_ELAPSED_TIME);
        std::cout << std::endl
            << "Integration took " << ((integrationEnd - integrationStart) / 1000.0) << "s" << std::endl;
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
    return 0;
}

void PhongEnvMap::_precomputeMap()
{
    _width = _envMap._getWidth();
    _height = _envMap._getHeight();
    _data = new float[3 * _width * (_height/2)];

    int s = 5; // Phong exponent
    int xStep = 1;
    int yStep = xStep;
    int xSkip = 256;
    int ySkip = 64;
    double a = (1+s) * M_PI / (double)(_width*_height) * (double)(xStep * yStep);
    
    for (int jj = 0; jj < _height-_height%ySkip+ySkip ; jj += ySkip)
    {
        int j = std::min(jj,_height-1);
        std::cout << "Integration Progress: y " << j << "\r";
        double phiN = M_PI*(double)j / (double)_height;
        double yN = cos(phiN);
        for (int i = 0; i < _width; i += xSkip)
        {
            double thetaN = M_PI*(2 * (double)i / (double)_width - 1);
            double xN = sin(phiN)*sin(thetaN);
            double zN = -sin(phiN)*cos(thetaN);
            double Rsum = 0;
            double Gsum = 0;
            double Bsum = 0;
            for (int l = 0; l < _height; l += yStep)
            {
                double phiE = M_PI*(double)l / (double)_height;
                double yE = cos(phiE);
                for (int k = 0; k < _width; k += xStep)
                {
                    double thetaE = M_PI*(2 * (double)k / (double)_width - 1);
                    double xE = sin(phiE)*sin(thetaE);
                    double zE = -sin(phiE)*cos(thetaE);
                    double R = _envMap._getPixelR(k, l);
                    double G = _envMap._getPixelG(k, l);
                    double B = _envMap._getPixelB(k, l);
                        double cosAng = pow(xE*xN + yE*yN + zE*zN,s);
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
        int i1 = i - i%xSkip;
        int i2 = i1 + xSkip;
        double dTheta1 = 2 * M_PI * (double)(i - i1) / _width;
        double dTheta2 = 2 * M_PI * (double)(i2 - i) / _width;
        i2 = i2%_width;
        for (int j = 0; j < _height; j++)
        {
            int j1 = j - j%ySkip;
            int j2 = std::min(j1 + ySkip, _height - 1);
            double phi = M_PI*(double)j / _height;
            double phi1 = M_PI*(double)j1 / _height;
            double phi2 = M_PI*(double)j2 / _height;
            double a11 = dTheta1*(cos(phi1) - cos(phi));
            double a12 = dTheta1*(cos(phi) - cos(phi2));
            double a21 = dTheta2*(cos(phi1) - cos(phi));
            double a22 = dTheta2*(cos(phi) - cos(phi2));
            if (j1 == j2) a11, a12, a21, a22 = 1;
            double A = a11 + a12 + a21 + a22;
            double R = (a22*_getPixelR(i1, j1) + a21*_getPixelR(i1, j2) + a12*_getPixelR(i2, j1) + a11*_getPixelR(i2, j2)) / A;
            double G = (a22*_getPixelG(i1, j1) + a21*_getPixelG(i1, j2) + a12*_getPixelG(i2, j1) + a11*_getPixelG(i2, j2)) / A;
            double B = (a22*_getPixelB(i1, j1) + a21*_getPixelB(i1, j2) + a12*_getPixelB(i2, j1) + a11*_getPixelB(i2, j2)) / A;
            _setPixelR(i, j, R);
            _setPixelG(i, j, G);
            _setPixelB(i, j, B);
        }
    }
}

void DiffuseEnvMap::_precomputeMap()
{
    _width = _envMap._getWidth();
    _height = _envMap._getHeight();
    _data = new float[3 * _width * _height];

    int xStep = 1;
    int yStep = xStep;
    int xSkip = 1;
    int ySkip = xSkip;
    double a = 2 * M_PI / (double)(_width*_height) * (double)(xStep * yStep);
    
    for (int jj = 0; jj < _height-_height%ySkip+ySkip ; jj += ySkip)
    {
        int j = std::min(jj,_height-1);
        std::cout << "We're on y " << j << "\r";
        double phiN = M_PI*(double)j / (double)_height;
        double yN = cos(phiN);
        for (int i = 0; i < _width; i += xSkip)
        {
            
            double thetaN = M_PI*(2 * (double)i / (double)_width - 1);
            double xN = sin(phiN)*sin(thetaN);
            double zN = -sin(phiN)*cos(thetaN);
            double Rsum = 0;
            double Gsum = 0;
            double Bsum = 0;
            for (int l = 0; l < _height; l += yStep)
            {
                double phiE = M_PI*(double)l / (double)_height;
                double yE = cos(phiE);
                for (int k = 0; k < _width; k += xStep)
                {
                    double thetaE = M_PI*(2 * (double)k / (double)_width - 1);
                    double xE = sin(phiE)*sin(thetaE);
                    double zE = -sin(phiE)*cos(thetaE);
                    double R = _envMap._getPixelR(k, l);
                    double G = _envMap._getPixelG(k, l);
                    double B = _envMap._getPixelB(k, l);
                    double cosAng = xE*xN + yE*yN + zE*zN;
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
        int i1 = i - i%xSkip;
        int i2 = i1 + xSkip;
        double dTheta1 = 2 * M_PI * (double)(i - i1) / _width;
        double dTheta2 = 2 * M_PI * (double)(i2 - i) / _width;
        i2 = i2%_width;
        for (int j = 0; j < _height; j++){
            int j1 = j - j%ySkip;
            int j2 = std::min(j1 + ySkip, _height - 1);
            double phi = M_PI*(double)j / _height;
            double phi1 = M_PI*(double)j1 / _height;
            double phi2 = M_PI*(double)j2 / _height;
            double a11 = dTheta1*(cos(phi1) - cos(phi));
            double a12 = dTheta1*(cos(phi) - cos(phi2));
            double a21 = dTheta2*(cos(phi1) - cos(phi));
            double a22 = dTheta2*(cos(phi) - cos(phi2));
            if (j1 == j2) a11, a12, a21, a22 = 1;
            double A = a11 + a12 + a21 + a22;
            double R = (a22*_getPixelR(i1, j1) + a21*_getPixelR(i1, j2) + a12*_getPixelR(i2, j1) + a11*_getPixelR(i2, j2)) / A;
            double G = (a22*_getPixelG(i1, j1) + a21*_getPixelG(i1, j2) + a12*_getPixelG(i2, j1) + a11*_getPixelG(i2, j2)) / A;
            double B = (a22*_getPixelB(i1, j1) + a21*_getPixelB(i1, j2) + a12*_getPixelB(i2, j1) + a11*_getPixelB(i2, j2)) / A;
            _setPixelR(i, j, R);
            _setPixelG(i, j, G);
            _setPixelB(i, j, B);;
        }
    }

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

    envMap->bind();

    GlutDraw::drawSphere(_r,_n,_m);

//    Doesn't work yet.
//    envMap->unbind();
}

void ObjGeometry::doDraw()
{
    if (!_geomReady)
    {
        _readGeom();
        _geomReady = true;
    }

    Shader * shader = _world->findShader(this);
    EnvMap * envMap = _world->getEnvMap();

    envMap->bind();
    GLuint vertexbuffer;
    glGenVertexArrays(1, &vertexbuffer);
    glBindVertexArray(vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(glm::vec3), &_vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, _vertices.size(), GL_FLOAT, GL_FALSE, 0,(void*) 0);

    // Draw the triangle !
    glDrawArrays(GL_QUADS, 0, _vertices.size());
    glDisableVertexAttribArray(0);

//    Doesn't work yet.
//    envMap->unbind();
    return;
}

int ObjGeometry::_readGeom()
{
    std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
    std::vector< glm::vec3 > tempVertices;
    std::vector< glm::vec2 > tempUVs;
    std::vector< glm::vec3 > tempNormals;
    int lineCount=0;

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
            vertex.x = 0.01 * vertex.x;
            vertex.y = 0.01 * vertex.y;
            vertex.z = 0.01 * vertex.z;
            tempVertices.push_back(vertex);
        }
        else if (line.find("vt ") == 0)
        {
            glm::vec2 uv;
            linestream >> type >> uv.x >> uv.y;
            tempUVs.push_back(uv);
        }
        else if (line.find("n ") == 0)
        {
            glm::vec3 normal;
            linestream >> type >> normal.x >> normal.y >> normal.z;
            tempNormals.push_back(normal);
        }
        else if (line.find("f ") == 0)
        {
            unsigned int vertexIndex[4], uvIndex[4];
            char delim;
            linestream >> type >> vertexIndex[0] >> delim >> uvIndex[0] >>
                vertexIndex[1] >> delim >> uvIndex[1] >>
                vertexIndex[2] >> delim >> uvIndex[2] >>
                vertexIndex[3] >> delim >> uvIndex[3];

            for (int i = 0; i < 4; i++)
            {
                vertexIndices.push_back(vertexIndex[i]);
                uvIndices.push_back(uvIndex[i]);
            }
        }

        lineCount++;
        if (lineCount % 1000 == 0)
        {
        std::cout << "Parsing obj line: " << lineCount << "\r";
    }
    }
    std::cout << std::endl;
    file.close();

    for (unsigned int i = 0; i < vertexIndices.size(); i++)
    {
        unsigned int vertexIndex = vertexIndices[i];
        glm::vec3 vertex = tempVertices[vertexIndex - 1];
        _vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i < uvIndices.size(); i++)
    {
        unsigned int uvIndex = uvIndices[i];
        glm::vec2 uv = tempUVs[uvIndex - 1];
        _uvs.push_back(uv);
    }

    return lineCount;
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

glm::mat3 R_alignAxes(glm::vec3 X1, glm::vec3 Y1, glm::vec3 Z1, glm::vec3 X2, glm::vec3 Y2, glm::vec3 Z2) {
    glm::vec3 x1 = normalize(X1);
    glm::vec3 y1 = normalize(Y1);
    glm::vec3 z1 = normalize(Z1);
    glm::vec3 x2 = normalize(X2);
    glm::vec3 y2 = normalize(Y2);
    glm::vec3 z2 = normalize(Z2);
    glm::vec3 w = (float)M_PI*glm::normalize(z1 + z2);
    glm::mat3 RalignZ = (float)(2 / M_PI / M_PI)*glm::outerProduct(w, w) - glm::mat3();
    y1 = RalignZ*y1;
    float theta = glm::dot(glm::cross(y2, y1), z2);
    w = theta*z2;
    glm::mat3 wCrossMat;
    wCrossMat[0] = glm::vec3(0.0, w[2], -w[1]);
    wCrossMat[1] = glm::vec3(-w[2], 0.0, w[0]);
    wCrossMat[2] = glm::vec3(w[1], -w[0], 0.0);
    glm::mat3 RalignY = (float)((1 - cos(theta)) / M_PI / M_PI)*glm::outerProduct(w,w) + cos(theta)*glm::mat3()+sin(theta)/theta*wCrossMat;
    return RalignY*RalignZ;
}

// (x,y,z) is orientation of the patch normal (e.g. icosahedral directions) relative to the envMap
// Compute the viewpoint for such a patch from every theta (polar), phi (azimuthal) relative to the normal
void CookTorranceMap::_precomputeMap()
{
    _width = _envMap._getWidth();
    _height = _envMap._getHeight();
    _data = new float[3 * _width * _height];
    glm::mat3 R_alignEP;
    glm::vec3 xAxP_E, yAxP_E;
    glm::vec3 xAxE_E(1.0f, 0.0f, 0.0f); // x-axis of EnvMap in the frame of EnvMap
    glm::vec3 yAxE_E(0.0f, 1.0f, 0.0f);
    glm::vec3 zAxE_E(0.0f, 0.0f, 1.0f);
    glm::vec3 zAxP_E = glm::normalize(_zAxis);
    if (_zAxis.x == 0 && _zAxis.y == 0){
        yAxP_E = glm::vec3(0.0f, 1.0f, 0.0f);
    }
    else yAxP_E = glm::normalize(zAxE_E - glm::dot(zAxE_E, zAxP_E)*zAxP_E);
    xAxP_E = glm::cross(yAxP_E, zAxP_E);
    _xAxis = xAxP_E;
    _yAxis = yAxP_E;
    _zAxis = zAxP_E;
    R_alignEP[0] = xAxP_E;
    R_alignEP[1] = yAxP_E;
    R_alignEP[2] = zAxP_E;
    R_alignEP = glm::transpose(R_alignEP);
    float a = 2 * M_PI*M_PI / (double)(_width*_height);
    for (int i = 0; i < _width; i++){
        std::cout << "We're on x " << i << "\r";
        float thetaV_P = 2 * M_PI*((double)i / _width - 1);
        for (int j = 0; j <= _height / 2; j++)
        {
            float phiV_P = M_PI*(double)j / _height;
            glm::vec3 V_P(sin(phiV_P)*cos(thetaV_P), sin(phiV_P)*sin(thetaV_P), cos(phiV_P));
            float NdotV = V_P.z;
            double Rsum = 0;
            double Gsum = 0;
            double Bsum = 0;
            //double sAng = 0;
            for (int k = 0; k < _width; k++){
                float thetaL_E = 2 * M_PI*((double)k / _width - 1);
                for (int l = 0; l < _height; l++){
                    float phiL_E = M_PI*(double)l / _height;
                    glm::vec3 L_E(sin(phiL_E)*sin(thetaL_E), cos(phiL_E), -sin(phiL_E)*cos(thetaL_E));
                    glm::vec3 L_P = R_alignEP*L_E;
                    glm::vec3 H_P = glm::normalize(L_P + V_P);
                    float NdotL = L_P.z;
                    if (NdotL <= 0) continue;
                    //sAng += a*sin(phiL_E);
                    float NdotH = H_P.z;
                    float VdotH = glm::dot(V_P, H_P);
                    float LdotH = glm::dot(L_P, H_P);
                    float G = std::min(2 * NdotH*NdotV / VdotH, 2 * NdotH*NdotL / LdotH);
                    G = std::min((float)1.0, G);
                    float D = exp((NdotH*NdotH - 1) / (_roughness*_roughness*NdotH*NdotH));
                    D /= M_PI*_roughness*_roughness*pow(NdotH, 4);
                    float F = _reflCoeff + (1 - _reflCoeff)*pow(1 - VdotH, 5);
                    float brdf = F*D*G / (M_PI*NdotL*NdotV);
                    // integrate
                    double envR = _envMap._getPixelR(k, l);
                    double envG = _envMap._getPixelG(k, l);
                    double envB = _envMap._getPixelB(k, l);
                    Rsum += envR*brdf*NdotL*sin(phiL_E);
                    Gsum += envG*brdf*NdotL*sin(phiL_E);
                    Bsum += envB*brdf*NdotL*sin(phiL_E);
                }
            }
            _setPixelR(i, j, a*Rsum);
            _setPixelG(i, j, a*Gsum);
            _setPixelB(i, j, a*Bsum);
        }
    }
}

void CookTorranceIcosMap::_precomputeMap()
{
    _width = _envMap._getWidth();
    _height = _envMap._getHeight();
    _data = new float[3 * _width * _height];
    glm::mat3 R_alignEP;
    glm::vec3 xAxP_E = Scene::ICOS_XAXES[_vertexIndex];
    glm::vec3 yAxP_E = Scene::ICOS_YAXES[_vertexIndex];
    glm::vec3 zAxP_E = Scene::ICOS_ZAXES[_vertexIndex];
    glm::vec3 xAxE_E(1.0f, 0.0f, 0.0f); // x-axis of EnvMap in the frame of EnvMap
    glm::vec3 yAxE_E(0.0f, 1.0f, 0.0f);
    glm::vec3 zAxE_E(0.0f, 0.0f, 1.0f);
    R_alignEP[0] = xAxP_E;
    R_alignEP[1] = yAxP_E;
    R_alignEP[2] = zAxP_E;
    R_alignEP = glm::transpose(R_alignEP);
    float a = 2 * M_PI*M_PI / (double)(_width*_height);
    for (int i = 0; i < _width; i++){
        std::cout << "We're on x " << i << "\r";
        float thetaV_P = 2 * M_PI*((double)i / _width - 1);
        for (int j = 0; j <= _height / 2; j++)
        {
            float phiV_P = M_PI*(double)j / _height;
            glm::vec3 V_P(sin(phiV_P)*cos(thetaV_P), sin(phiV_P)*sin(thetaV_P), cos(phiV_P));
            float NdotV = V_P.z;
            double Rsum = 0;
            double Gsum = 0;
            double Bsum = 0;
            //double sAng = 0;
            for (int k = 0; k < _width; k++){
                float thetaL_E = 2 * M_PI*((double)k / _width - 1);
                for (int l = 0; l < _height; l++){
                    float phiL_E = M_PI*(double)l / _height;
                    glm::vec3 L_E(sin(phiL_E)*sin(thetaL_E), cos(phiL_E), -sin(phiL_E)*cos(thetaL_E));
                    glm::vec3 L_P = R_alignEP*L_E;
                    glm::vec3 H_P = glm::normalize(L_P + V_P);
                    float NdotL = L_P.z;
                    if (NdotL <= 0) continue;
                    //sAng += a*sin(phiL_E);
                    float NdotH = H_P.z;
                    float VdotH = glm::dot(V_P, H_P);
                    float LdotH = glm::dot(L_P, H_P);
                    float G = std::min(2 * NdotH*NdotV / VdotH, 2 * NdotH*NdotL / LdotH);
                    G = std::min((float)1.0, G);
                    float D = exp((NdotH*NdotH - 1) / (_roughness*_roughness*NdotH*NdotH));
                    D /= M_PI*_roughness*_roughness*pow(NdotH, 4);
                    float F = _reflCoeff + (1 - _reflCoeff)*pow(1 - VdotH, 5);
                    float brdf = F*D*G / (M_PI*NdotL*NdotV);
                    // integrate
                    double envR = _envMap._getPixelR(k, l);
                    double envG = _envMap._getPixelG(k, l);
                    double envB = _envMap._getPixelB(k, l);
                    Rsum += envR*brdf*NdotL*sin(phiL_E);
                    Gsum += envG*brdf*NdotL*sin(phiL_E);
                    Bsum += envB*brdf*NdotL*sin(phiL_E);
                }
            }
            _setPixelR(i, j, a*Rsum);
            _setPixelG(i, j, a*Gsum);
            _setPixelB(i, j, a*Bsum);
        }
    }
}
