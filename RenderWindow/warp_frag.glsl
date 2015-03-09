uniform sampler2D radMaps[12];

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

void main()
{
    int V0 = int(V[0]);
    int V1 = int(V[1]);
    int V2 = int(V[2]);
    vec4 env = w[0]*texture2D(radMaps[V0], uv[0]);
    //env += w[1]*texture2D(radMaps[V1],uv[1]);
    //env += w[2]*texture2D(radMaps[V2],uv[2]); 
    //gl_FragColor = tonemap(env/(w[0]+w[1]+w[2])) * 5;
    float soccer = exp((V[0]-5.5)/2)+exp((V[1]-5.5)/2)+exp((V[2]-5.5)/2);
    //gl_FragColor = tonemap(0.5*vec4(soccer,soccer,soccer,1.0f));
    gl_FragColor = tonemap(vec4(w[0],w[1],w[2],1.0f));
}