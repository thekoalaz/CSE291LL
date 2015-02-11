uniform sampler2D envMap;

void main (void)
{
    vec4 env = texture2D( envMap, gl_TexCoord[0].st);
    gl_FragColor = env*15;
}
