#include "scene.h"

using namespace Scene;
/** Global variables **/
int Object::NEXTID = 0;

/* Utility Functions */
char * textFileRead(const char * fn);

/*
double ** matrixMultiply(double ** A, double ** B, int nRow, int nCol, int innerDim) {
    double ** C;
    C = new double*[nRow];
    for (int i = 0; i < nRow; i++){
        C[i] = new double[nCol];
        for (int j = 0; j < nCol; j++){
            C[i][j] = 0;
            for (int k = 0; k < innerDim; k++){
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return C;
}
*/

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

void EnvMap::_readMap()
{
    FILE* hdrfile;
    fopen_s(&hdrfile, _fileName.c_str(), "rb");
    if (hdrfile != nullptr)
    {
        // Read header
        RGBE_ReadHeader(hdrfile, &_width, &_height, NULL);
        // Read data
        _data = new float[3 * _width*_height];
        RGBE_ReadPixels_RLE(hdrfile, _data, _width, _height);
    }

    glGenTextures(1, &_textureID);
    glBindTexture(GL_TEXTURE_2D, _textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _width, _height, 0, GL_RGB, GL_FLOAT, _data);
}

void EnvMap::doDraw()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D , _textureID);
    GLUquadric* quad = gluNewQuadric(); 
    gluQuadricTexture(quad,GL_TRUE); 

    gluSphere(quad,_r,_n,_m);

    glDisable(GL_TEXTURE_2D);
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
//    glActiveTexture(GL_TEXTURE0 + 0);
//    glBindTexture(GL_TEXTURE_2D, _textureID);
}

void EnvMap::unbind()
{
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

    //GLint lightDir = glGetUniformLocation(shader->getProgram(), "lightDir");
    //GLfloat lightDirValue[3] = { 0.0, 1.0, 1.0 };
    //glUniform3fv(lightDir, 1, lightDirValue);

    Camera * cam = _world->getCam();
    GLint CamPos = glGetUniformLocation(shader->getProgram(), "camPos");

    double xAng = cam->getRotx();
    double yAng = cam->getRoty();
    double zAng = cam->getRotz();

    double V[3] = { 0, 0, 1 }; // initial camera viewing direction is along (minus)z-axis?
    double Rx[3][3] = { { 1, 0, 0 }, { 0, cos(xAng), -sin(xAng) }, { 0, sin(xAng), cos(xAng) } };
    double Ry[3][3] = { { cos(yAng), 0, sin(yAng) }, { 0, 1, 0 }, { -sin(yAng), 0, cos(yAng) } };
    double Rz[3][3] = { { cos(zAng), -sin(zAng), 0 }, { sin(zAng), cos(zAng), 0 }, { 0, 0, 1 } };
    double RzRy[3][3] = { { 0, 0, 0, }, { 0, 0, 0 }, { 0, 0, 0 } };
    double RxV[3] = { 0, 0, 0 };
    for (int i = 0; i < 3; i++){
        for (int k = 0; k < 3; k++){
            RxV[i] += Rx[i][k] * V[k];
            for (int j = 0; j < 3; j++){
                RzRy[i][j] += Rz[i][k] * Ry[k][j];
            }
        }
    }
    for (int i = 0; i < 3; i++){
        V[i] = 0;
        for (int k = 0; k < 3; k++){
            V[i] += RzRy[i][k] * RxV[k];
        }
    }
    V[1] += cam->getTx();
    V[2] += cam->getTy();
    V[3] += cam->getTz();

    glUniform3dv(CamPos, 1, V);

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
