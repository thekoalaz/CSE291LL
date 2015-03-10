#define M_PI 3.1415926535897932384626433832795f

varying vec2 xy;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;

    float theta = atan(gl_Normal.x,-gl_Normal.z);
    float phi = acos(gl_Normal.y);
    float x = (1+theta/M_PI)/2;
    float y = phi/M_PI;
    xy = vec2(x,y);
}
