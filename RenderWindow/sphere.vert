/*
uniform vec3 lightDir;
varying float intensity;

void main()
{
	intensity = max(dot(lightDir,gl_Normal), 0.0);

	gl_FrontColor = intensity * gl_FrontMaterial.diffuse;
	gl_Position = ftransform();
}
*/

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