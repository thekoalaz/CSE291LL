#define M_PI 3.1415926535897932384626433832795

uniform vec3 icosXaxes[12];
uniform vec3 icosYaxes[12];
uniform vec3 icosZaxes[12];

varying int view[3];
varying vec2 uv[3];
varying float w[3];

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
    vec3 normal = vec3(gl_NormalMatrix*gl_Normal);
    vec3 vertexPosition = vec3(gl_Position);
    vec3 viewDir = normalize(-vertexPosition);
    vec3 r = normalize(reflect(viewDir,normal)); // INDEX INTO REFLECTED DIRECTION
    r = vec3(inverse(gl_NormalMatrix)*r);

    float prox[12];
    for (int i=0; i<12; i++) prox[i] = dot(r,icosZaxes[i]);
    int view[3] = {0, 1, 2};
    int runt;
    if (prox[0]<prox[1] && prox[0]<prox[2]) runt = 0;
    elseif (prox[1]<prox[0] && prox[1]<prox[2]) runt = 1;
    else runt = 2;
    float runtProx = prox[runt];
    for (int i=3; i<12; i++)
    {
        if (prox[i]>=runtProx) continue;
        view[runt] = i;
        if (prox[(runt+1)%3]<prox[runt] && prox[(runt+1)%3]<prox[(runt+2)%3]) runt=(runt+1)%3;
        elseif (prox[(runt+2)%3]<prox[runt] && prox[(runt+2)%3]<prox[(runt+1)%3]) runt=(runt+2)%3;
        runtProx = prox[view[runt]];
    }

    vec3 x[3] = { icosXaxes[view[0]], icosXaxes[view[1]], icosXaxes[view[2]] };
    vec3 y[3] = { icosYaxes[view[0]], icosYaxes[view[1]], icosYaxes[view[2]] };
    vec3 z[3] = { icosZaxes[view[0]], icosZaxes[view[1]], icosZaxes[view[2]] };
    
    for (int i=0; i<3; i++)
    {
        int im1 = (i+2)%3 // i-1 MOD 3
        int ip1 = (i+1)%3
        float alpha = acos(dot(normalize(cross(r,z[im1])),normalize(cross(z[ip1],z[im1])))); // DIHEDRAL ANGLES
        float beta  = acos(dot(normalize(cross(z[ip1],r)),normalize(cross(z[ip1],z[im1]))));
        float gamma = acos(dot(normalize(cross(z[ip1],r)),normalize(cross(z[im1],r))));
        w[i] = alpha + beta + gamma - M_PI;
        float phi = acos(dot(r,z[i]));
        float theta = atan(dot(r,y[i]),dot(r,x[i]));
        uv[i] = vec2((1+theta/M_PI)/2,phi/M_PI); // U,V COORDINATES ON RADIANCE MAP
    }
}