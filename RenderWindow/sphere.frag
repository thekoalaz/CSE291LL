uniform sampler2D envMap;
varying vec2 xy;

void main()
{
    vec4 env = texture2D(envMap, xy);
    gl_FragColor = env*10;
}
