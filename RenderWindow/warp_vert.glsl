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
varying float w[3];
varying vec2 uv[3];

vec3 closestViews(float p0, float p1, float p2, float p3, float p4, float p5, float p6, float p7, float p8, float p9, float p10, float p11)
{
    int views[3];
    float prox[12] = {p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11};
    float m = 0;
    for (int i=0; i<12; i++) {
        if (prox[i]<=m) continue;
        m = prox[i];
        views[0] = i;
    }
    prox[views[0]] = 0;
    m = 0;
    for (int i=0; i<12; i++) {
        if (prox[i]<=m) continue;
        m = prox[i];
        views[1] = i;
    }
    prox[views[1]] = 0;
    m = 0;
    for (int i=0; i<12; i++) {
        if (prox[i]<=m) continue;
        m = prox[i];
        views[2] = i;
    }
    return vec3(float(views[0]),float(views[1]),float(views[2]));
}

void main()
{

    gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
    
    vec3 n = normalize(gl_NormalMatrix*gl_Normal);
    vec3 vertexPosition = vec3(gl_ModelViewMatrix*gl_Vertex);
    vec3 viewDir = normalize(-vertexPosition);
    vec3 r = normalize(reflect(viewDir,n)); // INDEX INTO REFLECTED DIRECTION
    r = inverse(gl_NormalMatrix)*r;
    vec3 vd = inverse(gl_NormalMatrix)*vec3(0.0f,0.0f,1.0f);
    
    float prox[12];
    for (int i=0; i<12; i++) prox[i] = dot(vd,ICOS_ZAXES[i]);

    /*
    int view[3] = {0, 1, 2};
    int runt;
    if (prox[0]<prox[1] && prox[0]<prox[2]) runt = 0;
    else if (prox[1]<prox[2] && prox[1]<prox[0]) runt = 1;
    else runt = 2;
    float runtProx = prox[runt];
    for (int i=3; i<12; i++)
    {
        if (prox[i]<=runtProx) continue;
        view[runt] = i;
        if      (prox[view[(runt+1)%3]]<prox[view[(runt+2)%3]] && prox[view[(runt+1)%3]]<prox[view[(runt+0)%3]]) runt=(runt+1)%3;
        else if (prox[view[(runt+2)%3]]<prox[view[(runt+0)%3]] && prox[view[(runt+2)%3]]<prox[view[(runt+1)%3]]) runt=(runt+2)%3;
        runtProx = prox[view[runt]];
    }
    V[0] = float(view[0]);
    V[1] = float(view[1]);
    V[2] = float(view[2]);
    */
    
    vec3 views = closestViews(  prox[0],prox[1],prox[2],prox[3],prox[4],prox[5],
                                prox[6],prox[7],prox[8],prox[9],prox[10],prox[11]   );
    V[0] = views[0];
    V[1] = views[1];
    V[2] = views[2];
    
    vec3 x[3] = { ICOS_XAXES[int(V[0])], ICOS_XAXES[int(V[1])], ICOS_XAXES[int(V[2])] };
    vec3 y[3] = { ICOS_YAXES[int(V[0])], ICOS_YAXES[int(V[1])], ICOS_YAXES[int(V[2])] };
    vec3 z[3] = { ICOS_ZAXES[int(V[0])], ICOS_ZAXES[int(V[1])], ICOS_ZAXES[int(V[2])] };
    
    for (int i=0; i<3; i++)
    {
        int im1 = (i+2)%3; // i-1 MOD 3
        int ip1 = (i+1)%3;
        float alpha = acos(dot(normalize(cross(vd,z[im1])),normalize(cross(z[ip1],z[im1])))); // DIHEDRAL ANGLES
        float beta  = acos(dot(normalize(cross(z[ip1],vd)),normalize(cross(z[ip1],z[im1]))));
        float gamma = acos(dot(normalize(cross(z[ip1],vd)),normalize(cross(z[im1],vd))));
        w[i] = alpha + beta + gamma - M_PI;
        vec3 h = normalize(r+z[i]);
        float phi = acos(dot(h,z[i]));
        float theta = atan(dot(h,y[i]),dot(h,x[i]));
        uv[i] = vec2((1+theta/M_PI)/2,phi/M_PI); // U,V COORDINATES ON RADIANCE MAP
    }
}