#include "GlutDraw.h"

void GlutDraw::drawLine(double x1, double y1, double z1,
        double x2, double y2, double z2)
{
    glBegin(GL_LINES);
        glVertex3d(x1, y1, z1);
        glVertex3d(x2, y2, z2);
    glEnd();
}

//TODO PRIO: drawRectangle so that we can do buttons.
void GlutDraw::drawRectangle(double x, double y,
        double width, double height)
{
    glBegin(GL_QUADS);
        glVertex2d(x, y);
        glVertex2d(x + width, y);
        glVertex2d(x + width, y + height);
        glVertex2d(x, y + height);
    glEnd();
}

void GlutDraw::drawSphere(double r,double n, double m/*, float* outR, float* outG, float* outB*/)
{
    for (int i = 0; i < n; i++){
        for (int j = 0; j < m; j++){
            //glColor3f(outR[i][j], outG[i][j], outB[i][j]);
            glColor3f(cos(M_PI*i / n), cos(M_PI*i / n), cos(M_PI*i / n));
            glBegin(GL_QUADS);
                glVertex3d(r*sin(M_PI*i / n)*cos(2 * M_PI*j / m), r*sin(M_PI*i / n)*sin(2 * M_PI*j / m), r*cos(M_PI*i / n));
                glVertex3d(r*sin(M_PI*i / n)*cos(2 * M_PI*(j + 1) / m), r*sin(M_PI*i / n)*sin(2 * M_PI*(j + 1) / m), r*cos(M_PI*i / n));
                glVertex3d(r*sin(M_PI*(i + 1) / n)*cos(2 * M_PI*(j + 1) / m), r*sin(M_PI*(i + 1) / n)*sin(2 * M_PI*(j + 1) / m), r*cos(M_PI*(i + 1) / n));
                glVertex3d(r*sin(M_PI*(i + 1) / n)*cos(2 * M_PI*j / m), r*sin(M_PI*(i + 1) / n)*sin(2 * M_PI*j / m), r*cos(M_PI*(i + 1) / n));
            glEnd();
        }
    }
}
