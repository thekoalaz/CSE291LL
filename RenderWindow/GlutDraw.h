#ifndef GLUTDRAW_H
#define GLUTDRAW_H

#include "stdafx.h"

namespace GlutDraw
{

void drawLine(double x1, double y1, double z1,
        double x2, double y2, double z2);

//TODO PRIO: drawRectangle so that we can do buttons.
void drawRectangle(double x, double y,
        double width, double height);

};
#endif /* GLUTDRAW_H */