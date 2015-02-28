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
        _envMap = envMap;
    }
    else
    {
        std::cout << "Env map already set!" << std::endl;
        _objects.push_back(envMap);
        envMap->setWorld(this);
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

int DiffuseEnvMap::_readMap()
{
    int read = -1;
    int integrationStart = glutGet(GLUT_ELAPSED_TIME);

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
        _width = _envMap._getWidth();
        _height = _envMap._getHeight();
        _data = new float[3 * _width * _height];

        int xStep = 1;
        int yStep = xStep;
        int xSkip = 256;
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
                /*
                _setPixelR(i, j, _envMap._getPixelR(i, j));
                _setPixelG(i, j, _envMap._getPixelG(i, j));
                _setPixelB(i, j, _envMap._getPixelB(i, j));
                */
            }
        }
        // interpolate if integration was not done on that patch
        for (int i = 0; i < _width; i++){
            int i1 = i - i%xSkip;
            int i2 = i1 + xSkip;
            double dTheta1 = 2 * M_PI * (double)(i - i1) / _width;
            double dTheta2 = 2 * M_PI * (double)(i2 - i) / _width;
            i2 = i2%_width;
            //if (i2 > _width - 1) {
            //    i2 = 0;
            //    dTheta2 = M_PI*(3 - 2 * (double)i / _width);
            //}
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
                _setPixelB(i, j, B);
            }
        }

        /*
        int u, v, r;
        for (int i = 0; i < _width; i++){
        r = i%skip;
        if (r < skip / 2) u = i - r;
        else u = std::min(i - r + skip, _width) % _width;
        for (int j = 0; j < _height; j++){
        r = j%skip;
        v = j - r;
        if (r >= skip / 2 && v + skip<_height) v += skip;
        _setPixelR(i, j, _getPixelR(u, v));
        _setPixelG(i, j, _getPixelG(u, v));
        _setPixelB(i, j, _getPixelB(u, v));
        }
        }
        */

        int integrationEnd = glutGet(GLUT_ELAPSED_TIME);
        std::cout << std::endl
            << "Integration took " << ((integrationEnd - integrationStart) / 100.0) << "s" << std::endl;

        if (_cached)
        {
            _writeMap();
        }

        glGenTextures(1, &_textureID);
        glBindTexture(GL_TEXTURE_2D, _textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _width, _height, 0, GL_RGB, GL_FLOAT, _data);
    }
    return 0;
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

/*
double dot3(double * x, double * y) {
    return x[0] * y[0] + x[1] * y[1] + x[2] * y[2];
}
double norm3(double *x){ return sqrt(dot3(x, x)); }
double * cross3(double * x, double * y) {
    double out[3] = { x[1] * y[2] - x[2] * y[1], x[2] * y[0] - x[0] * y[2], x[0] * y[1] - x[1] * y[0] };
    return out;
}
double * halfAngle3(double * X, double * Y) {
    double mag = norm3(X);
    double x[3] = { X[0] / mag, X[1] / mag, X[2] / mag };
    mag = norm3(Y);
    double y[3] = { Y[0] / mag, Y[1] / mag, Y[2] / mag };
    double out[3] = { x[0] + y[0], x[1] + y[1], x[2] + y[2] };
    double mag = norm3(out);
    out[0] /= mag;
    out[1] /= mag;
    out[2] /= mag;
    return out;
}
double ** wToR3(double * w){
    double theta = norm3(w);
    double ** R;
    R = new double*[3];
    R[0] = new double[3];
    R[1] = new double[3];
    R[2] = new double[3];
    double a = cos(theta); // multiplicative factor holder
    R[0][0] = a;
    R[1][1] = a;
    R[2][2] = a;
    a = sin(theta) / theta;
    R[0][1] = -a*w[2];
    R[0][2] = a*w[1];
    R[1][2] = -a*w[0];
    R[1][0] = -R[0][1];
    R[2][0] = -R[0][2];
    R[2][1] = -R[1][2];
    a = (1 - cos(theta)) / theta / theta;
    R[0][0] += a*w[0] * w[0];
    R[0][1] += a*w[0] * w[1];
    R[0][2] += a*w[0] * w[2];
    R[1][0] += a*w[1] * w[0];
    R[1][1] += a*w[1] * w[1];
    R[1][2] += a*w[1] * w[2];
    R[2][0] += a*w[2] * w[0];
    R[2][1] += a*w[2] * w[1];
    R[2][2] += a*w[2] * w[2];
    return R;
}
double ** Ralign3(double *X, double *Y){
    // y=Ralign3*x aligns x with y by rotating x around the half-angle-vector
    double * w = halfAngle3(X, Y);
    w[0] = M_PI*w[0];
    w[1] = M_PI*w[1];
    w[2] = M_PI*w[2];
    return wToR(w);
}
double * rotate3(double * x, double ** R){
    double y[3] = { 0, 0, 0 };
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            y[i] += R[i][j] * x[j];
        }
    }
    return y;
}
double * rotate3(double * x, double * w){
    return rotate3(x, wToR3(w));
}
// (x,y,z) is orientation of the patch normal (e.g. icosahedral directions) relative to the envMap
// Compute the viewpoint for such a patch from every theta (polar), phi (azimuthal) relative to the normal
warpEnvMap(double x, double y, double z) {
    double xAxE_E[3] = { 1, 0, 0 }; // x-axis of EnvMap in the frame of EnvMap
    double yAxE_E[3] = { 0, 1, 0 };
    double zAxE_E[3] = { 0, 0, 1 };
    double xAxP_E[3]; // x-axis of Patch in the frame of EnvMap
    double yAxP_E[3];
    double zAxP_E[3];
    if (x == 0 && y == 0){
        if (z >= 0){
            xAxP_E[0] = 1; // x-axis unit vector in EnvMap coordinates in special case
            xAxP_E[1] = 0;
            xAxP_E[2] = 0;
            yAxP_E[0] = 0; // y-axis set such that coordinates are right-handed
            yAxP_E[1] = 1;
            yAxP_E[2] = 0;
            zAxP_E[0] = 0; // z-axis set to surface normal
            zAxP_E[1] = 0;
            zAxP_E[2] = 1;
        }
        else {
            xAxP_E[0] = 1; //
            xAxP_E[1] = 0;
            xAxP_E[2] = 0;
            yAxP_E[0] = 0; //
            yAxP_E[1] = -1;
            yAxP_E[2] = 0;
            zAxP_E[0] = 0; //
            zAxP_E[1] = 0;
            zAxP_E[2] = -1;
        }
    }
    else {
        double mag = sqrt(x*x + y*y + z*z);
        zAxP_E[0] = x / mag;
        zAxP_E[1] = y / mag;
        zAxP_E[2] = z / mag;
        // xAxis is [0,0,1] orthogonalized relative to normal (z-Axis)
        xAxP_E[0] = -zAxP_E[2] * zAxP_E[0];
        xAxP_E[1] = -zAxP_E[2] * zAxP_E[1];
        xAxP_E[2] = -zAxP_E[2] * zAxP_E[2] + 1;
        mag = norm3(xAxP_E);
        xAxP_E[0] = xAxP_E[0] / mag;
        xAxP_E[1] = xAxP_E[1] / mag;
        xAxP_E[2] = xAxP_E[2] / mag;
        // yAxis is zAxis cross xAxis
        yAxP_E[0] = zAxP_E[1] * xAxP_E[2] - xAxP_E[2] * xAxP_E[1];
        yAxP_E[1] = zAxP_E[2] * xAxP_E[0] - zAxP_E[0] * xAxP_E[2];
        yAxP_E[2] = zAxP_E[0] * xAxP_E[1] - zAxP_E[1] * xAxP_E[0];
    }
    double a = 2 * M_PI*M_PI / (double)(_width*_height);
    for (int i = 0 ; i < _width; i++){
        double thetaV_P = 2 * M_PI*((double)i / _width - 1);
        for (int j = 0; j <= _height/2; j++){
            double phiV_P = M_PI*(double)j / _height;
            double N_P[3] = { 0, 0, 1 };
            double V_P[3] = { sin(phiV_P)*cos(thetaV_P), sin(phiV_P)*sin(thetaV_P), cos(phiV_P) };
            double NdotV = V_P[2];
            double Rsum, Gsum, Bsum = 0;
            for (int k = 0; k < _width; k++){
                double thetaL_E = 2 * M_PI*((double)k / _width - 1);
                for (int l = 0; l < _height; l++){
                    double phiL_E = M_PI*(double)j / _height;
                    double L_E[3] = { sin(phiL_E)*sin(thetaL_E), cos(phiL_E), -sin(phiL_E)*cos(thetaL_E) };
                    double * zAxE_half_zAxP_E = halfAngle3(zAxE_E, zAxP_E);
                    double wAlignZ[3] = { M_PI*zAxE_half_zAxP_E[0],
                        M_PI*zAxE_half_zAxP_E[1],
                        M_PI*zAxE_half_zAxP_E[2] };
                    double * xAxE_temp = rotate3(xAxE_E, wAlignZ);
                    double dir = cross3(xAxE_E, xAxE_temp)[3] / fabs(cross3(xAxE_E, xAxE_temp)[3]);
                    double wAlignX[3] = { 0, 0, dir*acos(dot3(xAxE_E, xAxE_temp)) };
                    double * L_P = rotate3(rotate3(L_E, wAlignZ), wAlignX);
                    double * H_P = halfAngle3(L_P, V_P);
                    double NdotL = L_P[2];
                    double NdotH = dot3(N_P, H_P);
                    double VdotH = dot3(V_P, H_P);
                    double LdotH = dot3(L_P, H_P);
                    double G = std::min(1.0, 2 * NdotH*NdotV / VdotH, 2 * NdotH*NdotL / LdotH);
                    double D = exp((NdotH*NdotH - 1) / (_roughness*_roughness*NdotH*NdotH));
                    D /= M_PI*_roughness*_roughness*pow(NdotH,4);
                    double F = _reflCoef + (1 - _reflCoef)*pow(1 - VdotH, 5);
                    double brdf = F*D*G / (M_PI*NdotL*NdotV);
                    // integrate
                    double R = _envMap._getPixelR(k, l);
                    double G = _envMap._getPixelG(k, l);
                    double B = _envMap._getPixelB(k, l);
                    Rsum += R*brdf*NdotL*sin(phiL_E);
                    Gsum += G*brdf*NdotL*sin(phiL_E);
                    Bsum += B*brdf*NdotL*sin(phiL_E);
                }
            }
            _setPixelR(i, j, a*Rsum);
            _setPixelG(i, j, a*Gsum);
            _setPixelB(i, j, a*Bsum);
        }
    }
}
*/