uniform vec4 camPos;
//uniform gluQuadric envMap;

out vec2 dir // [theta,phi] for grabbing texture

void main()
{
    vec4 viewDir = normalize(camPos-gl_vertex);
    vec4 dv = gl_Normal-viewDir;
    vec4 refDir = gl_Normal+dv-2*dot(gl_Normal,dv)*gl_Normal;
    vec3 refDir(refDir);
    refDir = normalize(refDir);
    dir = vec2(atan2(refDir[2]/refDir[1]),acos(refDir[3]));
}
