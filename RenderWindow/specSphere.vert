#define M_PI 3.1415926535897932384626433832795

uniform vec3 camPos;
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

out vec2 xy;
out vec3 light_vector;
out vec3 normal_vector;
out vec3 halfway_vector;

void main()
{
    gl_Position = ftransform();
    vec3 normal = vec3(gl_Normal);
    vec3 vertex = vec3(gl_Vertex);
    vec3 viewDir = normalize(camPos-vertex);
    //vec3 dv = normal-viewDir;
    //vec3 refDir = normalize(normal+dv-2*dot(normal,dv)*normal);
    vec3 incident = normalize(vec3(vertex-camPos));
    vec3 refDir = normalize(reflect(incident,normal));
    float theta = atan(refDir.y,refDir.x);
    float phi = acos(refDir.z);
    float x = (1+theta/M_PI)/2;
    float y = phi/M_PI;
    xy = vec2(x,y);
    //gl_TexCoord[0].s=x;
    //gl_TexCoord[0].t=y;

	vec3 light_position = vec3(10,10,0);
    vec4 v = View * Model * vec4(vertex, 1.0);
    vec3 normal1 = normalize(normal);
 
    light_vector = normalize((View * vec4(light_position, 1.0)).xyz - v.xyz);
    normal_vector = (inverse(transpose(View * Model)) * vec4(normal1, 0.0)).xyz;
    halfway_vector = light_vector + normalize(-v.xyz);
}
