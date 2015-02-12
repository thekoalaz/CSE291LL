varying float intensity;

void main()
{
    vec3 lightDir = normalize(vec3(1,2,-5));
	intensity = dot(lightDir,gl_Normal);
	gl_Position = ftransform();
}
