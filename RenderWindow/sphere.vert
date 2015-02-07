uniform vec3 lightDir;
varying float intensity;
//vec3 lightDir= vec3(0.0,0.0,1.0);

void main()
{
	intensity = dot(lightDir,gl_Normal);
	gl_Position = ftransform();
}