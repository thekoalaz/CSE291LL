uniform sampler2D envMap;
in vec2 xy;

in vec3 normal_vector;
in vec3 light_vector;
in vec3 halfway_vector;
 
void main (void) {
    vec3 normal1         = normalize(normal_vector);
    vec3 light_vector1   = normalize(light_vector);
    vec3 halfway_vector1 = normalize(halfway_vector);

    vec4 c = texture(envMap, xy)*50;

    vec4 emissive_color = vec4(0.0, 1.0, 0.0, 1.0); // green
    vec4 ambient_color  = vec4(1.0, 1.0, 1.0, 1.0); // white
    vec4 diffuse_color  = vec4(1.0, 1.0, 1.0, 1.0); // white
    vec4 specular_color = vec4(0.0, 0.0, 1.0, 1.0); // blue

    float emissive_contribution = 0.02;
    float ambient_contribution  = 0.01;
    float diffuse_contribution  = 0.74;
    float specular_contribution = 0.23;

    float d = dot(normal1, light_vector1);
    bool facing = d > 0.0;

    gl_FragColor = emissive_color * emissive_contribution +
            ambient_color  * ambient_contribution  * c +
            diffuse_color  * diffuse_contribution  * c * max(d, 0) +
            (facing ?
                specular_color * specular_contribution * c * pow(dot(normal1, halfway_vector1), 80.0) :
                vec4(0.0, 0.0, 0.0, 0.0));
    gl_FragColor.a = 1.0;
}
