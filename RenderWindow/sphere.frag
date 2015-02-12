uniform sampler2D envMap;
varying vec2 xy;

void main()
{
    gl_FragColor = 10*texture2D(envMap, xy);
    //vec4 env = texture2D( envMap, gl_TexCoord[0].st);
    //gl_FragColor = env*15;
}