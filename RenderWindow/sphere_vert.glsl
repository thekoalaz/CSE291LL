#define M_PI 3.1415926535897932384626433832795f

varying vec2 xy;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
    vec3 normal = vec3(gl_NormalMatrix*gl_Normal);
    vec3 vertex = vec3(gl_ModelViewMatrix * gl_Vertex);

    vec3 incident = normalize(vertex);
    vec3 refDir = normalize(reflect(incident,normal));
    refDir = vec3(inverse(gl_NormalMatrix)*refDir);
    float theta = atan(refDir.x,-refDir.z);
    float phi = acos(refDir.y);
    float x = (1.0+theta/M_PI)/2.0;
    float y = phi/M_PI;
    xy = vec2(x,y);
}
