#include "color.h"
#include "utils.h"

int HDR::_readImage()
{
    FILE* hdrfile;
    float * _data;
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

    _processImage(_data);
}

void HDR::_processImage(float * _data)
{
    for (int x = 0; x < _width; x++)
    {
        for (int y = 0; y < _height; y++)
        {
            _pixels[_pixelPos(x, y)] = new Pixel(0,0,0);
        }
    }
}

const Pixel * HDR::getPixel(int x, int y)
{
    return _pixels[_pixelPos(x, y)];
}
