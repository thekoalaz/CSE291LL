uniform sampler2D tex;
in vec2 xy;

void main()
{
    gl_FragColor = texture2D(tex, xy);
}