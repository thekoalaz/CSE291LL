uniform sampler2D envMap;
uniform sampler2D radMap00;
uniform sampler2D radMap01;
uniform sampler2D radMap02;
uniform sampler2D radMap03;
uniform sampler2D radMap04;
uniform sampler2D radMap05;
uniform sampler2D radMap06;
uniform sampler2D radMap07;
uniform sampler2D radMap08;
uniform sampler2D radMap09;
uniform sampler2D radMap10;
uniform sampler2D radMap11;

varying float V[3];
varying float w[3];
varying vec2 uv[3];

vec4 tonemap(vec4 x)
{
	float r = x[0] / (1.0 + x[0]);
	float g = x[1] / (1.0 + x[1]);
	float b = x[2] / (1.0 + x[2]);
	return vec4(r,g,b,x[3]);
}
int V0 = int(V[0]);
int V1 = int(V[1]);
int V2 = int(V[2]);
void main()
{
    vec4 c;
    if (V0==0) c = w[0]*texture2D(radMap00, uv[0]);
    if (V0==1) c = w[0]*texture2D(radMap01, uv[0]);
    if (V0==2) c = w[0]*texture2D(radMap02, uv[0]);
    if (V0==3) c = w[0]*texture2D(radMap03, uv[0]);
    if (V0==4) c = w[0]*texture2D(radMap04, uv[0]);
    if (V0==5) c = w[0]*texture2D(radMap05, uv[0]);
    if (V0==6) c = w[0]*texture2D(radMap06, uv[0]);
    if (V0==7) c = w[0]*texture2D(radMap07, uv[0]);
    if (V0==8) c = w[0]*texture2D(radMap08, uv[0]);
    if (V0==9) c = w[0]*texture2D(radMap09, uv[0]);
    if (V0==10) c = w[0]*texture2D(radMap10, uv[0]);
    if (V0==11) c = w[0]*texture2D(radMap11, uv[0]);
    if (V1==0) c += w[1]*texture2D(radMap00, uv[1]);
    if (V1==1) c += w[1]*texture2D(radMap01, uv[1]);
    if (V1==2) c += w[1]*texture2D(radMap02, uv[1]);
    if (V1==3) c += w[1]*texture2D(radMap03, uv[1]);
    if (V1==4) c += w[1]*texture2D(radMap04, uv[1]);
    if (V1==5) c += w[1]*texture2D(radMap05, uv[1]);
    if (V1==6) c += w[1]*texture2D(radMap06, uv[1]);
    if (V1==7) c += w[1]*texture2D(radMap07, uv[1]);
    if (V1==8) c += w[1]*texture2D(radMap08, uv[1]);
    if (V1==9) c += w[1]*texture2D(radMap09, uv[1]);
    if (V1==10) c += w[1]*texture2D(radMap10, uv[1]);
    if (V1==11) c += w[1]*texture2D(radMap11, uv[1]);
    if (V2==0) c += w[2]*texture2D(radMap00, uv[2]);
    if (V2==1) c += w[2]*texture2D(radMap01, uv[2]);
    if (V2==2) c += w[2]*texture2D(radMap02, uv[2]);
    if (V2==3) c += w[2]*texture2D(radMap03, uv[2]);
    if (V2==4) c += w[2]*texture2D(radMap04, uv[2]);
    if (V2==5) c += w[2]*texture2D(radMap05, uv[2]);
    if (V2==6) c += w[2]*texture2D(radMap06, uv[2]);
    if (V2==7) c += w[2]*texture2D(radMap07, uv[2]);
    if (V2==8) c += w[2]*texture2D(radMap08, uv[2]);
    if (V2==9) c += w[2]*texture2D(radMap09, uv[2]);
    if (V2==10) c += w[2]*texture2D(radMap10, uv[2]);
    if (V2==11) c += w[2]*texture2D(radMap11, uv[2]);
    c /= w[0]+w[1]+w[2];
    c[3] = 1;
    gl_FragColor = c;
    
    //float soccer;
    //soccer = exp((V[0]-5.5)/2)+exp((V[1]-5.5)/2)+exp((V[2]-5.5)/2);
    //gl_FragColor = tonemap(0.5*vec4(soccer,soccer,soccer,1.0f));
    //gl_FragColor = tonemap(vec4(w[0],w[1],w[2],1.0f));
    //gl_FragColor = tonemap(vec4(exp((V[0]-5.5)/6),exp((V[1]-5.5)/6),exp((V[2]-5.5)/6),1.0f));
}