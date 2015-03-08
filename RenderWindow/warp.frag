uniform sampler2D radMaps[12];

varying int view[3];
varying float w[3];
varying vec2 uv[3];

vec4 tonemap(vec4 x)
{
	float r = x[0] / (1.0 + x[0]);
	float g = x[1] / (1.0 + x[1]);
	float b = x[2] / (1.0 + x[2]);

	return vec4(r,g,b,x[3]);
}

void main()
{
    vec4 env = w[0]*texture2D(radMaps[view[0]], uv[0]);
    env += w[1]*texture2D(radMaps[view[1]],uv[1]);
    env += w[2]*texture2D(radMaps[view[2]],uv[2]); 
    gl_FragColor = tonemap(env/(w[0]+w[1]+w[2])) * 5;
}