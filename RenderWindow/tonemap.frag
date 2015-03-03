uniform sampler2D envMap;
varying vec2 xy;

vec4 tonemap(vec4 x)
{
	float r = x[0] / (1.0 + x[0]);
	float g = x[1] / (1.0 + x[1]);
	float b = x[2] / (1.0 + x[2]);

	return vec4(r,g,b,x[3]);
}

void main()
{
    vec4 env = texture2D(envMap, xy);
    gl_FragColor = tonemap(env) * 5;
}
