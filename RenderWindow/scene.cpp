#include "scene.h"

using namespace Scene;
/** Global variables **/
int Object::NEXTID = 0;

void World::draw()
{
    for(std::vector<Object *>::const_iterator object = _objects.begin() ;
        object < _objects.end() ; object++)
    {
        (*object)->draw();
    }
}

void Object::draw()
{
    glTranslated(_tx, _ty, _tz);
    glRotated(_rotx,1,0,0);
    glRotated(_roty,0,1,0);
    glRotated(_rotz,0,0,1);
}


void Camera::draw()
{
}

void Grid::draw()
{
    glPushMatrix();
    Object::draw();
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
    glPopMatrix();
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

void EnvMap::draw()
{
    GLUquadric* quad = gluNewQuadric(); 

    glPushMatrix();
    Object::draw();

    glColor3f(1.0f,1.0f,1.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D , _textureID);

    gluQuadricTexture(quad,GL_TRUE); 
    gluSphere(quad,10000,20,20);

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
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


World & Scene::createWorld()
{
    World * new_world = new World();
    return *new_world;
}