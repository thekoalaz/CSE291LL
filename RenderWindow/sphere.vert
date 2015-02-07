uniform vec3 lightDir;
varying float intensity;

void main()
{
	intensity = max(dot(lightDir,gl_Normal), 0.0);

	gl_FrontColor = intensity * gl_FrontMaterial.diffuse;
	gl_Position = ftransform();
}