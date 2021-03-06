#version 330 compatibility
#define M_PI 3.1415926535897932384626433832795f
#define TwoSort(a,b) {  }

float GOLDEN_RATIO = (1 + sqrt(5)) / 2;
vec3 ICOS_ZAXES[12] = vec3[](
    normalize(vec3(0.0f ,  1.0f,  GOLDEN_RATIO)),
    normalize(vec3(0.0f , -1.0f,  GOLDEN_RATIO)),
    normalize(vec3(0.0f ,  1.0f, -GOLDEN_RATIO)),
    normalize(vec3(0.0f , -1.0f, -GOLDEN_RATIO)),
    normalize(vec3(1.0f ,  GOLDEN_RATIO, 0.0f)),
    normalize(vec3(-1.0f,  GOLDEN_RATIO, 0.0f)),
    normalize(vec3(1.0f , -GOLDEN_RATIO, 0.0f)),
    normalize(vec3(-1.0f, -GOLDEN_RATIO, 0.0f)),
    normalize(vec3(GOLDEN_RATIO , 0.0,  1.0f)),
    normalize(vec3(-GOLDEN_RATIO, 0.0,  1.0f)),
    normalize(vec3(GOLDEN_RATIO , 0.0, -1.0f)),
    normalize(vec3(-GOLDEN_RATIO, 0.0, -1.0f))
);
vec3 ICOS_YAXES[12] = vec3[](
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
);
vec3 ICOS_XAXES[12] = vec3[](
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
);

flat out ivec3 V;
out float w[3];
out vec2 uv[3];

out vec2 uvD;

ivec3 closestViews(float p0, float p1, float p2, float p3, float p4, float p5, float p6, float p7, float p8, float p9, float p10, float p11)
{
    int views[3];
    float prox[12] = float[](p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11);

    for (int n = 11; n < 0 ; --n) {
      for (int i = 0; i < n; ++i) {
        float tmp = max (prox[i], prox[i+1]);
        prox[i+1] = prox[i] + prox[i+1] - tmp;
        prox[i] = tmp;
      }
    }
    return ivec3(prox[0],prox[1],prox[2]);
}

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
    
    vec3 n = normalize(gl_NormalMatrix*gl_Normal);
    vec3 vertexPosition = vec3(gl_ModelViewMatrix*gl_Vertex);
    vec3 viewDir = normalize(vertexPosition);
    vec3 r = normalize(reflect(viewDir,n)); // INDEX INTO REFLECTED DIRECTION
    r = inverse(gl_NormalMatrix)*r;
    vec3 vd = viewDir;
    
    float prox[12];
    for (int i=0; i<12; i++) prox[i] = dot(vd,ICOS_ZAXES[i]);

    V = closestViews(  prox[0],prox[1],prox[2],prox[3],prox[4],prox[5],
                                prox[6],prox[7],prox[8],prox[9],prox[10],prox[11]   );
    
    vec3 x[3] = vec3[](ICOS_XAXES[V[0]], ICOS_XAXES[V[1]], ICOS_XAXES[V[2]]);
    vec3 y[3] = vec3[](ICOS_YAXES[V[0]], ICOS_YAXES[V[1]], ICOS_YAXES[V[2]]);
    vec3 z[3] = vec3[](ICOS_ZAXES[V[0]], ICOS_ZAXES[V[1]], ICOS_ZAXES[V[2]]);
    
    for (int i=0; i<3; i++)
    {
        int im1 = (i+2)%3; // i-1 MOD 3
        int ip1 = (i+1)%3;
        float alpha = acos(dot(normalize(cross(vd,z[im1])),normalize(cross(z[ip1],z[im1])))); // DIHEDRAL ANGLES
        float beta  = acos(dot(normalize(cross(z[ip1],vd)),normalize(cross(z[ip1],z[im1]))));
        float gamma = acos(dot(normalize(cross(z[ip1],vd)),normalize(cross(z[im1],vd))));
        w[i] = alpha + beta + gamma - M_PI;
        vec3 h = normalize(mat3(x[i], y[i], z[i]) * r + z[i]);
        float phi = acos(dot(h,z[i]));
        float theta = atan(dot(h,y[i]),dot(h,x[i]));
        uv[i] = vec2((1+theta/M_PI)/2,phi/M_PI); // U,V COORDINATES ON RADIANCE MAP
    }
    
    float thetaD = atan(gl_Normal.x,-gl_Normal.z);
    float phiD = acos(gl_Normal.y);
    uvD = vec2((1+thetaD/M_PI)/2,phiD/M_PI);
}
