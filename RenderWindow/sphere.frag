uniform sampler2D envMap;
in vec2 xy;

void main()
{
    gl_FragColor = 15*texture2D(envMap, xy);
    //vec4 env = texture2D( envMap, gl_TexCoord[0].st);
    //gl_FragColor = env*15;
}