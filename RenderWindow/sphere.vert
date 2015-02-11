#define M_PI 3.1415926535897932384626433832795

uniform vec3 camPos;
out vec2 xy;

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
    vec3 normal = vec3(gl_Normal);
    vec3 vertex = vec3(gl_Vertex);
    vec3 viewDir = normalize(camPos-vertex);
    vec3 dv = normal-viewDir;
    vec3 refDir = normalize(normal+dv-2*dot(normal,dv)*normal);
    double theta = atan2(refDir[2]/refDir[1]);
    double phi = acos(refDir[3]);
    xy = vec2((1+theta/M_PI)/2,phi/M_PI);
}