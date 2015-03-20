#include "stdafx.h"

class Pixel
{
public:
    Pixel(float r, float g, float b) { color = glm::vec3(r, g, b); };

private:
    glm::vec3 color;
};

class HDR
{
public:
    HDR(std::string filename) : _filename(filename) { };

    const Pixel * getPixel(int x, int y);
    const Pixel * getPixel(float x, float y);

private:
    void _setPixel(int x, int y);
    void _setPixel(float x, float y);

    int _pixelPos(int x, int y) { return x + 3 * y; };
    int _readImage();
    void _processImage(float *);

    std::vector<Pixel *> _pixels;
    std::string _filename;
    int _width, _height;
};
