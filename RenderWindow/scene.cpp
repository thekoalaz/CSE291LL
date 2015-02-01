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
}

void EnvMap::draw()
{
}

std::tuple<float, float, float> EnvMap::map(double theta, double phi)
{
    double x = theta / M_PI + _width;
    double y = phi / M_PI;

    float red = _bilinearInterpolate(&_data[0], x, y);
    float green = _bilinearInterpolate(&_data[1], x, y);
    float blue = _bilinearInterpolate(&_data[2], x, y);

    return std::tuple<float, float, float>(red, green, blue);
}

float EnvMap::_bilinearInterpolate(const float * _colors, const double x, const double y)
{
    int px = (int) x;
    int py = (int) y;

    const float p0 = _colors[px + py*_width];
    const float p1 = _colors[(px+1) + py*_width];
    const float p2 = _colors[px + (py+1)*_width];
    const float p3 = _colors[(px+1)+ (py+1) *_width];

    double fx = x - px;
    double fy = y - py;
    double fx1 = 1 - fx;
    double fy1 = 1 - fy;

    return static_cast<float>( (p0 * fx * fy)
        + (p1 * fx1 * fy)
        + (p2 * fx * fy1)
        + (p3 * fx1 * fy1) );
}


World & Scene::createWorld()
{
    World * new_world = new World();
    return *new_world;
}