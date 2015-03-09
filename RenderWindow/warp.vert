#define M_PI 3.1415926535897932384626433832795
float GOLDEN_RATIO = (1 + sqrt(5)) / 2;
vec3 ICOS_ZAXES[12] = {
    normalize(vec3(0.0f ,  1.0f,  GOLDEN_RATIO)),
    normalize(vec3(0.0f , -1.0f,  GOLDEN_RATIO)),
    normalize(vec3(0.0f ,  1.0f, -GOLDEN_RATIO)),
    normalize(vec3(0.0f , -1.0f, -GOLDEN_RATIO)),
    normalize(vec3(1.0f ,  GOLDEN_RATIO, 0.0f)),
    normalize(vec3(-1.0f,  GOLDEN_RATIO, 0.0f)),
    normalize(vec3(1.0f , -GOLDEN_RATIO, 0.0f)),
    normalize(vec3(-1.0f, -GOLDEN_RATIO, 0.0f)),
    normalize(vec3(1.0f , 0.0f,  GOLDEN_RATIO)),
    normalize(vec3(-1.0f, 0.0f,  GOLDEN_RATIO)),
    normalize(vec3(1.0f , 0.0f, -GOLDEN_RATIO)),
    normalize(vec3(-1.0f, 0.0f, -GOLDEN_RATIO))
};
vec3 ICOS_YAXES[12] = {
    vec3(1.0f, 0.0f, 0.0f),
    vec3(-1.0f, 0.0f, 0.0f),
    vec3(-1.0f, 0.0f, 0.0f),
    vec3(1.0f, 0.0f, 0.0f),
    vec3(0.0f, 0.0f, 1.0f),
    vec3(0.0f, 0.0f, -1.0f),
    vec3(0.0f, 0.0f, -1.0f),
    vec3(0.0f, 0.0f, 1.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, -1.0f, 0.0f),
    vec3(0.0f, -1.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f)
};
vec3 ICOS_XAXES[12] = {
    cross(ICOS_YAXES[0] , ICOS_ZAXES[0]),
    cross(ICOS_YAXES[1] , ICOS_ZAXES[1]),
    cross(ICOS_YAXES[2] , ICOS_ZAXES[2]),
    cross(ICOS_YAXES[3] , ICOS_ZAXES[3]),
    cross(ICOS_YAXES[4] , ICOS_ZAXES[4]),
    cross(ICOS_YAXES[5] , ICOS_ZAXES[5]),
    cross(ICOS_YAXES[6] , ICOS_ZAXES[6]),
    cross(ICOS_YAXES[7] , ICOS_ZAXES[7]),
    cross(ICOS_YAXES[8] , ICOS_ZAXES[8]),
    cross(ICOS_YAXES[9] , ICOS_ZAXES[9]),
    cross(ICOS_YAXES[10], ICOS_ZAXES[10]),
    cross(ICOS_YAXES[11], ICOS_ZAXES[11])
};

varying float V[3];
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
    for (int i=0; i<12; i++) prox[i] = dot(r,ICOS_ZAXES[i]);
    int view[3] = {0, 1, 2};
    int runt;
    if (prox[0]<prox[1] && prox[0]<prox[2]) runt = 0;
    else if (prox[1]<prox[0] && prox[1]<prox[2]) runt = 1;
    else runt = 2;
    float runtProx = prox[runt];
    for (int i=3; i<12; i++)
    {
        if (prox[i]>=runtProx) continue;
        view[runt] = i;
        if (prox[(runt+1)%3]<prox[runt] && prox[(runt+1)%3]<prox[(runt+2)%3]) runt=(runt+1)%3;
        else if (prox[(runt+2)%3]<prox[runt] && prox[(runt+2)%3]<prox[(runt+1)%3]) runt=(runt+2)%3;
        runtProx = prox[view[runt]];
    }
    
    vec3 x[3] = { ICOS_XAXES[view[0]], ICOS_XAXES[view[1]], ICOS_XAXES[view[2]] };
    vec3 y[3] = { ICOS_YAXES[view[0]], ICOS_YAXES[view[1]], ICOS_YAXES[view[2]] };
    vec3 z[3] = { ICOS_ZAXES[view[0]], ICOS_ZAXES[view[1]], ICOS_ZAXES[view[2]] };
    
    V[0] = view[0];
    V[1] = view[1];
    V[2] = view[2];

    for (int i=0; i<3; i++)
    {
        int im1 = (i+2)%3; // i-1 MOD 3
        int ip1 = (i+1)%3;
        float alpha = acos(dot(normalize(cross(r,z[im1])),normalize(cross(z[ip1],z[im1])))); // DIHEDRAL ANGLES
        float beta  = acos(dot(normalize(cross(z[ip1],r)),normalize(cross(z[ip1],z[im1]))));
        float gamma = acos(dot(normalize(cross(z[ip1],r)),normalize(cross(z[im1],r))));
        w[i] = alpha + beta + gamma - M_PI;
        float phi = acos(dot(r,z[i]));
        float theta = atan(dot(r,y[i]),dot(r,x[i]));
        uv[i] = vec2((1+theta/M_PI)/2,phi/M_PI); // U,V COORDINATES ON RADIANCE MAP
    }
}