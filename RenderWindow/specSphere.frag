uniform sampler2D envMap;
in vec2 xy;
in vec4 diffuse;
in vec4 specular;

void main()
{
    vec4 texture = 15*texture2D(envMap, xy);
    gl_FragColor =  texture * diffuse;
    //gl_FragColor =  0.6* texture * diffuse + 0.4 * texture * specular;
}
