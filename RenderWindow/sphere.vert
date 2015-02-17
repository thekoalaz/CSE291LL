#define M_PI 3.1415926535897932384626433832795

//uniform vec3 camPos;
//uniform mat4 modelViewMatrix;
varying vec2 xy;

void main()
{
    vec3 camPos=vec3(0.0,0.0,0.0);
    gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
    vec3 normal = vec3(gl_NormalMatrix*gl_Normal);
    vec3 vertex = vec3(gl_Position);
    
	//gl_Position = ftransform();
    //vec3 normal = vec3(gl_Normal);
    //vec3 vertex = vec3(gl_Vertex);
    vec3 viewDir = normalize(camPos-vertex);
    vec3 incident = normalize(vec3(vertex-camPos));
    vec3 refDir = normalize(reflect(incident,normal));
    refDir = vec3(inverse(gl_NormalMatrix)*refDir);
    float theta = atan(refDir.y,refDir.x);
    float phi = acos(refDir.z);
    float x = (1+theta/M_PI)/2;
    float y = phi/M_PI;
    xy = vec2(x,y);
}
