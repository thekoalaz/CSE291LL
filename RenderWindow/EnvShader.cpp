#include "scene.h"
#include "utils.h"
#include "EnvShader.h"

using namespace Scene;

void EnvShader::link()
{
    Shader::link();
    _envMap->bind();
    GLint texLoc = glGetUniformLocation(getProgram(), "envMap");
    glUniform1i(texLoc, _envMap->getTextureID());
}

void EnvShader::unlink()
{
    _envMap->unbind();
    Shader::unlink();
}

void CtShader::link()
{
    Shader::link();
    for (auto &radMap : _radMaps)
    {
        auto index = &radMap - &_radMaps[0];
        std::string radName = RadMap::getRadMapName(index);
        radMap->bind();
        GLint radMapLocation = glGetUniformLocation(getProgram(), radName.c_str());
        glUniform1i(radMapLocation, radMap->getTextureID());
    }

    if (_diffuseMap != nullptr)
    {
        _diffuseMap->bind();
        GLint diffMapLocation = glGetUniformLocation(getProgram(), "diffMap");
        glUniform1i(diffMapLocation, _diffuseMap->getTextureID());
    }
    else
    {
        GLint diffMapLocation = glGetUniformLocation(getProgram(), "diffMap");
        glUniform1i(diffMapLocation, 15);
    }
}

int PrecomputeMap::_readMap()
{
    int read = -1;

    if (_cached)
    {
        read = EnvMap::_readMap();
    }

    if (read > 0)
    {
        std::cout << "Read map cache from " << _filename << std::endl;
    }
    else
    {
        int integrationStart = glutGet(GLUT_ELAPSED_TIME);
        std::cout << "Starting integration " << mapType() << std::endl;
        _precomputeMap();

        int integrationEnd = glutGet(GLUT_ELAPSED_TIME);
        std::cout << std::endl
            << "Integration took " << ((integrationEnd - integrationStart) / 1000.0f) << "s" << std::endl;
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

    _mapReady = true;

    return 0;
}

void InterpolateMap::_precomputeMap()
{
    _width = _newWidth;
    _height = _newHeight;
    _data = new float[3 * _width * _height];
    int w0 = _envMap.getWidth();   // old dimensions
    int h0 = _envMap.getHeight();
    std::cout << _newWidth << ", " << _newHeight << std::endl;
    float wMag = (float)(_newWidth - 1) / (w0 - 1);
    float hMag = (float)(_newHeight - 1) / (h0 - 1);
    for (int i = 0; i < _newWidth; i++){
        float i0 = (float)i / wMag;
        for (int j = 0; j < _newHeight; j++){
            float j0 = (float)j / hMag;
            _setPixelR(i, j, _envMap.getPixelR(i0, j0));
            _setPixelG(i, j, _envMap.getPixelG(i0, j0));
            _setPixelB(i, j, _envMap.getPixelB(i0, j0));
        }
    }
};

void DiffuseEnvMap::_precomputeMap()
{
    _width = _envMap.getWidth();
    _height = _envMap.getHeight();
    _data = new float[3 * _width * _height];

    int xStep = 1;
    int yStep = xStep;
    float a = 2 * M_PI / (float)(_width*_height) * (float)(xStep * yStep);
    
    for (int jj = 0; jj < _height - _height%_ySkip + _ySkip; jj += _ySkip)
    {
        int j = std::min(jj, _height - 1);
        std::cout << "We're on height " << j << "/" << _height << "\r";
        float phiN = M_PI*(float)j / (float)_height;
        float yN = cos(phiN);
        for (int i = 0; i < _width; i += _xSkip)
        {
            
            float thetaN = M_PI*(2 * (float)i / (float)_width - 1);
            float xN = sin(phiN)*sin(thetaN);
            float zN = -sin(phiN)*cos(thetaN);
            float Rsum = 0;
            float Gsum = 0;
            float Bsum = 0;
            for (int l = 0; l < _height; l += yStep)
            {
                float phiE = M_PI*(float)l / (float)_height;
                float yE = cos(phiE);
                for (int k = 0; k < _width; k += xStep)
                {
                    float thetaE = M_PI*(2 * (float)k / (float)_width - 1);
                    float xE = sin(phiE)*sin(thetaE);
                    float zE = -sin(phiE)*cos(thetaE);
                    float R = _envMap.getPixelR(k, l);
                    float G = _envMap.getPixelG(k, l);
                    float B = _envMap.getPixelB(k, l);
                    float cosAng = xE*xN + yE*yN + zE*zN;
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
        int i1 = i - i%_xSkip;
        int i2 = i1 + _xSkip;
        float dTheta1 = 2 * M_PI * (float)(i - i1) / _width;
        float dTheta2 = 2 * M_PI * (float)(i2 - i) / _width;
        i2 = i2%_width;
        for (int j = 0; j < _height; j++){
            int j1 = j - j%_ySkip;
            int j2 = std::min(j1 + _ySkip, _height - 1);
            float phi = M_PI*(float)j / _height;
            float phi1 = M_PI*(float)j1 / _height;
            float phi2 = M_PI*(float)j2 / _height;
            float d1 = cos(phi1) - cos(phi);
            float d2 = cos(phi) - cos(phi2);
            if (j1 == j2){
                d1 = 1;
                d2 = 1;
            }
            float a11 = dTheta1*d1;
            float a12 = dTheta1*d2;
            float a21 = dTheta2*d1;
            float a22 = dTheta2*d2;
            float A = a11 + a12 + a21 + a22;
            float R = (a22*getPixelR(i1, j1) + a21*getPixelR(i1, j2) + a12*getPixelR(i2, j1) + a11*getPixelR(i2, j2)) / A;
            float G = (a22*getPixelG(i1, j1) + a21*getPixelG(i1, j2) + a12*getPixelG(i2, j1) + a11*getPixelG(i2, j2)) / A;
            float B = (a22*getPixelB(i1, j1) + a21*getPixelB(i1, j2) + a12*getPixelB(i2, j1) + a11*getPixelB(i2, j2)) / A;
            _setPixelR(i, j, R);
            _setPixelG(i, j, G);
            _setPixelB(i, j, B);;
        }
    }
}


void PhongEnvMap::_precomputeMap()
{
    _width = _envMap.getWidth();
    _height = _envMap.getHeight();
    _data = new float[3 * _width * _height];

    int xStep = 1;
    int yStep = xStep;
    float a = (1 + _s) * M_PI / (float)(_width*_height) * (float)(xStep * yStep);
    
    for (int jj = 0; jj < _height - _height%_ySkip + _ySkip; jj += _ySkip)
    {
        int j = std::min(jj, _height - 1);
        std::cout << "We're on height " << j << "/" << _height << "\r";
        float phiN = M_PI*(float)j / (float)_height;
        float yN = cos(phiN);
        for (int i = 0; i < _width; i += _xSkip)
        {
            float thetaN = M_PI*(2 * (float)i / (float)_width - 1);
            float xN = sin(phiN)*sin(thetaN);
            float zN = -sin(phiN)*cos(thetaN);
            float Rsum = 0;
            float Gsum = 0;
            float Bsum = 0;
            for (int l = 0; l < _height; l += yStep)
            {
                float phiE = M_PI*(float)l / (float)_height;
                float yE = cos(phiE);
                for (int k = 0; k < _width; k += xStep)
                {
                    float thetaE = M_PI*(2 * (float)k / (float)_width - 1);
                    float xE = sin(phiE)*sin(thetaE);
                    float zE = -sin(phiE)*cos(thetaE);
                    float R = _envMap.getPixelR(k, l);
                    float G = _envMap.getPixelG(k, l);
                    float B = _envMap.getPixelB(k, l);
                    float cosAng = pow(xE*xN + yE*yN + zE*zN,_s);
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
        int i1 = i - i%_xSkip;
        int i2 = i1 + _xSkip;
        float dTheta1 = 2 * M_PI * (float)(i - i1) / _width;
        float dTheta2 = 2 * M_PI * (float)(i2 - i) / _width;
        i2 = i2%_width;
        for (int j = 0; j < _height; j++)
        {
            int j1 = j - j%_ySkip;
            int j2 = std::min(j1 + _ySkip, _height - 1);
            float phi = M_PI*(float)j / _height;
            float phi1 = M_PI*(float)j1 / _height;
            float phi2 = M_PI*(float)j2 / _height;
            float d1 = cos(phi1) - cos(phi);
            float d2 = cos(phi) - cos(phi2);
            if (j1 == j2){
                d1 = 1;
                d2 = 1;
            }
            float a11 = dTheta1*d1;
            float a12 = dTheta1*d2;
            float a21 = dTheta2*d1;
            float a22 = dTheta2*d2;
            float A = a11 + a12 + a21 + a22;
            float R = (a22*getPixelR(i1, j1) + a21*getPixelR(i1, j2) + a12*getPixelR(i2, j1) + a11*getPixelR(i2, j2)) / A;
            float G = (a22*getPixelG(i1, j1) + a21*getPixelG(i1, j2) + a12*getPixelG(i2, j1) + a11*getPixelG(i2, j2)) / A;
            float B = (a22*getPixelB(i1, j1) + a21*getPixelB(i1, j2) + a12*getPixelB(i2, j1) + a11*getPixelB(i2, j2)) / A;
            _setPixelR(i, j, R);
            _setPixelG(i, j, G);
            _setPixelB(i, j, B);
        }
    }
}

void CookTorranceIcosMap::_precomputeMap()
{
    _width = _envMap.getWidth();
    _height = _envMap.getHeight();
    _data = new float[3 * _width * _height];
    glm::mat3 R_alignEV;
    glm::vec3 xAxV_E = Scene::ICOS_XAXES[_vertexIndex];
    glm::vec3 yAxV_E = Scene::ICOS_YAXES[_vertexIndex];
    glm::vec3 zAxV_E = Scene::ICOS_ZAXES[_vertexIndex];
    glm::vec3 xAxE_E(1.0f, 0.0f, 0.0f);
    glm::vec3 yAxE_E(0.0f, 1.0f, 0.0f);
    glm::vec3 zAxE_E(0.0f, 0.0f, 1.0f);
    R_alignEV[0] = xAxV_E;
    R_alignEV[1] = yAxV_E;
    R_alignEV[2] = zAxV_E;
    R_alignEV = glm::transpose(R_alignEV);
    glm::vec3 V_V(0.0f, 0.0f, 1.0f);
    int halfway = static_cast<int>(std::ceil(_height / 2.0f));
    float a = 2 * M_PI*M_PI / (float)(_width*_height);

    for (int jj = 0; jj < halfway - halfway % _ySkip + _ySkip; jj += _ySkip) {
        int j = std::min(jj, halfway - 1);
        float phiN_V = M_PI*(float)j / _height;
        std::cout << "We're on height " << j << "/" << _height << "\r";
        for (int i = 0; i < _width; i += _xSkip){
            float thetaN_V = 2 * M_PI*((float)i / _width - 1);
            glm::vec3 N_V(sin(phiN_V)*cos(thetaN_V), sin(phiN_V)*sin(thetaN_V), cos(phiN_V));
            float NdotV = N_V.z;
            float Rsum = 0;
            float Gsum = 0;
            float Bsum = 0;
            for (int k = 0; k < _width; k++){
                float thetaL_E = 2 * M_PI*((float)k / _width - 1);
                for (int l = 0; l < _height; l++){
                    float phiL_E = M_PI*(float)l / _height;
                    glm::vec3 L_E(sin(phiL_E)*sin(thetaL_E), cos(phiL_E), -sin(phiL_E)*cos(thetaL_E));
                    glm::vec3 L_V = R_alignEV*L_E;
                    float NdotL = glm::dot(N_V, L_V);
                    if (NdotL <= 0) continue;
                    glm::vec3 H_V = glm::normalize(L_V + V_V);
                    float NdotH = glm::dot(N_V, H_V);
                    float VdotH = glm::dot(V_V, H_V);
                    float LdotH = glm::dot(L_V, H_V);
                    float G = std::min(2 * NdotH*NdotV / VdotH, 2 * NdotH*NdotL / LdotH);
                    G = std::min((float)1.0f, G);
                    float D = exp((NdotH*NdotH - 1) / (_roughness*_roughness*NdotH*NdotH));
                    D /= M_PI*_roughness*_roughness*pow(NdotH, 4.0f);
                    float F = _reflCoeff + (1 - _reflCoeff)*pow(1 - VdotH, 5.0f);
                    float brdf = F*D*G / (M_PI*NdotL*NdotV);
                    float envR = _envMap.getPixelR(k, l);
                    float envG = _envMap.getPixelG(k, l);
                    float envB = _envMap.getPixelB(k, l);
                    Rsum += envR*brdf*NdotL*sin(phiL_E);
                    Gsum += envG*brdf*NdotL*sin(phiL_E);
                    Bsum += envB*brdf*NdotL*sin(phiL_E);
                }
            }
            _setPixelR(i, j, a*Rsum);
            _setPixelG(i, j, a*Gsum);
            _setPixelB(i, j, a*Bsum);
            if (j == 0)
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

    if (_xSkip>1 || _ySkip > 1) {
        for (int i = 0; i < _width; i++)
        {
            int i1 = i - i%_xSkip;
            int i2 = i1 + _xSkip;
            float dTheta1 = 2 * M_PI * (float)(i - i1) / _width;
            float dTheta2 = 2 * M_PI * (float)(i2 - i) / _width;
            i2 = i2%_width;
            for (int j = 0; j < halfway; j++)
            {
                int j1 = j - j%_ySkip;
                int j2 = std::min(j1 + _ySkip, halfway - 1);
                float phi = M_PI*(float)j / _height;
                float phi1 = M_PI*(float)j1 / _height;
                float phi2 = M_PI*(float)j2 / _height;
                float d1 = cos(phi1) - cos(phi);
                float d2 = cos(phi) - cos(phi2);
                if (j1 == j2){
                    d1 = 1;
                    d2 = 1;
                }
                float a11 = dTheta1*d1;
                float a12 = dTheta1*d2;
                float a21 = dTheta2*d1;
                float a22 = dTheta2*d2;
                float A = a11 + a12 + a21 + a22;
                float R = (a22*getPixelR(i1, j1) + a21*getPixelR(i1, j2) + a12*getPixelR(i2, j1) + a11*getPixelR(i2, j2)) / A;
                float G = (a22*getPixelG(i1, j1) + a21*getPixelG(i1, j2) + a12*getPixelG(i2, j1) + a11*getPixelG(i2, j2)) / A;
                float B = (a22*getPixelB(i1, j1) + a21*getPixelB(i1, j2) + a12*getPixelB(i2, j1) + a11*getPixelB(i2, j2)) / A;
                _setPixelR(i, j, R);
                _setPixelG(i, j, G);
                _setPixelB(i, j, B);
            }
        }
    }
}

std::string CookTorranceIcosMap::getCtIcosMapName(int index)
{
    return zero_padded_name("ctIcos", index, 2);
}


std::string RadMap::getRadMapName(int index)
{
    return zero_padded_name("radMap", index, 2);
}
