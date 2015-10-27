#include "scene.h"
#include "utils.h"

using namespace Scene;
/** Global variables **/
int Object::NEXTID = 0;
int EnvMap::NEXTTEXTUREID = 0;

/* Method Definitions */
World & Scene::createWorld()
{
    World * new_world = new World();
    return *new_world;
}

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

void World::removeObject(Object * obj)
{
    auto sameID = [&](Object * object) { return object->getID() == obj->getID();  };
    auto to_remove = std::remove_if(std::begin(_objects), std::end(_objects), sameID);
    _objects.erase(to_remove);
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

    for (auto &object : _objects)
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
    glRotated(_rotx, 1, 0, 0);
    glRotated(_roty, 0, 1, 0);
    glRotated(_rotz, 0, 0, 1);

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
    glRotatef(_rotx, 1, 0, 0);
    glRotatef(_roty, 0, 1, 0);
    glRotatef(_rotz, 0, 0, 1);

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
    for (int r = -(_rows / 2); r <= (_rows / 2); r++)
    {
        GlutDraw::drawLine(-(_cols / 2.0f)*_gap, 0, r*_gap,
            (_cols / 2.0f)*_gap, 0, r*_gap);
    }
    for (int c = -(_cols / 2); c <= (_cols / 2); c++)
    {
        GlutDraw::drawLine(c*_gap, 0, -(_rows / 2.0f)*_gap,
            c*_gap, 0, (_rows / 2.0f)*_gap);
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

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _width, _height, 0, GL_RGB, GL_FLOAT, _data);

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
    glBindTexture(GL_TEXTURE_2D, _textureID);
    GLUquadric* quad = gluNewQuadric(); 
    gluQuadricTexture(quad, GL_TRUE);

    gluSphere(quad, _r, _n, _m);

    glDisable(GL_TEXTURE_2D);
    unbind();
}

void Sphere::doDraw()
{
    GlutDraw::drawSphere(_r, _n, _m);
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
    int lineCount = 0;
    int faceCount = 0;
    int vertCount = 0;

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
        if (lineCount % 1000 == 0)
        {
            std::cout << "Parsing obj line: " << lineCount << "\r";
        }
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
    int px = (int)x;
    int py = (int)y;

    const float p0 = _colors[3 * (px + py*_width)];
    const float p1 = _colors[3 * ((px + 1) + py*_width)];
    const float p2 = _colors[3 * (px + (py + 1)*_width)];
    const float p3 = _colors[3 * ((px + 1) + (py + 1) *_width)];

    float fx1 = x + 1 - px;
    float fy1 = y + 1 - py;
    float fx2 = x - px;
    float fy2 = y - py;

    return (p0 * fx1 * fy1)
        + (p1 * fx2 * fy1)
        + (p2 * fx1 * fy2)
        + (p3 * fx2 * fy2);
}

float EnvMap::_sphericalInterpolate(const float * _colors, const float x, const float y)
{
    int px1 = (int)x;
    int py1 = (int)y;
    int px2 = (px1 + 1) % _width;
    int py2 = std::max(py1 + 1, _height - 1);

    const float p11 = _colors[3 * (px1 + py1*_width)];
    const float p21 = _colors[3 * (px2 + py1*_width)];
    const float p12 = _colors[3 * (px1 + py2*_width)];
    const float p22 = _colors[3 * (px2 + py2*_width)];

    float dTheta1 = 2 * M_PI * (x - (float)px1) / _width;
    float dTheta2 = 2 * M_PI * ((float)px2 - x) / _width;

    float phi = M_PI*y / _height;
    float phi1 = M_PI*(float)py1 / _height;
    float phi2 = M_PI*(float)py2 / _height;

    float d1 = cos(phi1) - cos(phi);
    float d2 = cos(phi) - cos(phi2);
    if (py1 == py2){
        d1 = 1;
        d2 = 1;
    }
    float a11 = dTheta1*d1;
    float a12 = dTheta1*d2;
    float a21 = dTheta2*d1;
    float a22 = dTheta2*d2;
    float A = a11 + a12 + a21 + a22;

    return (p11*a22 + p12*a21 + p21*a12 + p22*a11) / A;
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

    char *vs, *fs;

    if (_vertfile == "" && _fragfile == ""){ return; }
    _program = glCreateProgram();

    if (_vertfile != "")
    {
        _vertex = glCreateShader(GL_VERTEX_SHADER);
        vs = textFileRead(_vertfile.c_str());
        const char * vv = vs;
        glShaderSource(_vertex, 1, &vv, NULL);
        free(vs);
        glCompileShader(_vertex);
        if (_checkShaderError(_vertex))
        {
            std::cout << _vertfile << " compiled successfully." << std::endl;
            glAttachShader(_program, _vertex);
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
    log = (char *)malloc(logsize + 1);
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

