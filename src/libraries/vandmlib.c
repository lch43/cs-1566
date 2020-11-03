//Landon Higinbotham
#include "vandmlib.h"
#include <stdio.h>
#include <math.h>

void print_v4(vec4 v)
{
    printf("[ %f %f %f %f ]\n", v.x, v.y, v.z, v.w);
}

vec4 scalar_mult_v4(float s, vec4 v)
{
    return (vec4){v.x * s, v.y * s, v.z * s, v.w * s};
}

vec4 v4_add_v4(vec4 a, vec4 b)
{
    return (vec4){a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

vec4 v4_sub_v4(vec4 a, vec4 b)
{
    return (vec4){a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

float mag_v4(vec4 v)
{
    return sqrt(pow(v.x, 2)+pow(v.y, 2)+pow(v.z, 2)+pow(v.w, 2));
}

vec4 normalize_v4(vec4 v)
{
    return scalar_mult_v4(1/mag_v4(v), v);
}

float dot_prod_v4(vec4 a, vec4 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

vec4 cross_prod_v4(vec4 a, vec4 b)
{
    return (vec4){(a.y*b.z)-(a.z*b.y), (a.z*b.x)-(a.x*b.z), (a.x*b.y)-(a.y*b.x), 0};
}


void print_mat4(mat4 m)
{
    /*printf("[ x1 y1 z1 w1 ]\n");
    printf("[ x2 y2 z2 w2 ]\n");
    printf("[ x3 y3 z3 w3 ]\n");
    printf("[ x4 y4 z4 w4 ]\n\n");*/

    printf("[ %f %f %f %f ]\n", m.x.x, m.y.x, m.z.x, m.w.x);
    printf("[ %f %f %f %f ]\n", m.x.y, m.y.y, m.z.y, m.w.y);
    printf("[ %f %f %f %f ]\n", m.x.z, m.y.z, m.z.z, m.w.z);
    printf("[ %f %f %f %f ]\n", m.x.w, m.y.w, m.z.w, m.w.w);
}

mat4 scalar_mult_mat4(float s, mat4 v)
{
    return (mat4){scalar_mult_v4(s, v.x), scalar_mult_v4(s, v.y), scalar_mult_v4(s, v.z), scalar_mult_v4(s, v.w)};
}

mat4 mat4_add_mat4(mat4 a, mat4 b)
{
    return (mat4){v4_add_v4(a.x, b.x), v4_add_v4(a.y, b.y), v4_add_v4(a.z, b.z), v4_add_v4(a.w, b.w)};
}

mat4 mat4_sub_mat4(mat4 a, mat4 b)
{
    return (mat4){v4_sub_v4(a.x, b.x), v4_sub_v4(a.y, b.y), v4_sub_v4(a.z, b.z), v4_sub_v4(a.w, b.w)};
}

mat4 mat4_mult_mat4(mat4 a, mat4 b)
{
    return (mat4){
        (vec4){
            (a.x.x * b.x.x)+(a.y.x * b.x.y)+(a.z.x * b.x.z)+(a.w.x * b.x.w),
            (a.x.y * b.x.x)+(a.y.y * b.x.y)+(a.z.y * b.x.z)+(a.w.y * b.x.w),
            (a.x.z * b.x.x)+(a.y.z * b.x.y)+(a.z.z * b.x.z)+(a.w.z * b.x.w),
            (a.x.w * b.x.x)+(a.y.w * b.x.y)+(a.z.w * b.x.z)+(a.w.w * b.x.w)
        },
        (vec4){
            (a.x.x * b.y.x)+(a.y.x * b.y.y)+(a.z.x * b.y.z)+(a.w.x * b.y.w),
            (a.x.y * b.y.x)+(a.y.y * b.y.y)+(a.z.y * b.y.z)+(a.w.y * b.y.w),
            (a.x.z * b.y.x)+(a.y.z * b.y.y)+(a.z.z * b.y.z)+(a.w.z * b.y.w),
            (a.x.w * b.y.x)+(a.y.w * b.y.y)+(a.z.w * b.y.z)+(a.w.w * b.y.w)
        },
        (vec4){
            (a.x.x * b.z.x)+(a.y.x * b.z.y)+(a.z.x * b.z.z)+(a.w.x * b.z.w),
            (a.x.y * b.z.x)+(a.y.y * b.z.y)+(a.z.y * b.z.z)+(a.w.y * b.z.w),
            (a.x.z * b.z.x)+(a.y.z * b.z.y)+(a.z.z * b.z.z)+(a.w.z * b.z.w),
            (a.x.w * b.z.x)+(a.y.w * b.z.y)+(a.z.w * b.z.z)+(a.w.w * b.z.w)
        },
        (vec4){
            (a.x.x * b.w.x)+(a.y.x * b.w.y)+(a.z.x * b.w.z)+(a.w.x * b.w.w),
            (a.x.y * b.w.x)+(a.y.y * b.w.y)+(a.z.y * b.w.z)+(a.w.y * b.w.w),
            (a.x.z * b.w.x)+(a.y.z * b.w.y)+(a.z.z * b.w.z)+(a.w.z * b.w.w),
            (a.x.w * b.w.x)+(a.y.w * b.w.y)+(a.z.w * b.w.z)+(a.w.w * b.w.w)
        }
    };
}

float sarrus(float a, float b, float c, float d, float e, float f, float g, float h, float i)
{
    return a*e*i + b*f*g + c*d*h - g*e*c - h*f*a - i*d*b;
}

mat4 minor_mat4(mat4 m)
{
    return (mat4){
        (vec4){
            sarrus(m.y.y, m.z.y, m.w.y, m.y.z, m.z.z, m.w.z, m.y.w, m.z.w, m.w.w) /* m11 */,
            sarrus(m.y.x, m.z.x, m.w.x, m.y.z, m.z.z, m.w.z, m.y.w, m.z.w, m.w.w) /* m21 */,
            sarrus(m.y.x, m.z.x, m.w.x, m.y.y, m.z.y, m.w.y, m.y.w, m.z.w, m.w.w) /* m31 */,
            sarrus(m.y.x, m.z.x, m.w.x, m.y.y, m.z.y, m.w.y, m.y.z, m.z.z, m.w.z) /* m41 */
        },
        (vec4){
            sarrus(m.x.y, m.z.y, m.w.y, m.x.z, m.z.z, m.w.z, m.x.w, m.z.w, m.w.w) /* m12 */,
            sarrus(m.x.x, m.z.x, m.w.x, m.x.z, m.z.z, m.w.z, m.x.w, m.z.w, m.w.w) /* m22 */,
            sarrus(m.x.x, m.z.x, m.w.x, m.x.y, m.z.y, m.w.y, m.x.w, m.z.w, m.w.w) /* m32 */,
            sarrus(m.x.x, m.z.x, m.w.x, m.x.y, m.z.y, m.w.y, m.x.z, m.z.z, m.w.z) /* m42 */
        },
        (vec4){
            sarrus(m.x.y, m.y.y, m.w.y, m.x.z, m.y.z, m.w.z, m.x.w, m.y.w, m.w.w) /* m13 */,
            sarrus(m.x.x, m.y.x, m.w.x, m.x.z, m.y.z, m.w.z, m.x.w, m.y.w, m.w.w) /* m23 */,
            sarrus(m.x.x, m.y.x, m.w.x, m.x.y, m.y.y, m.w.y, m.x.w, m.y.w, m.w.w) /* m33 */,
            sarrus(m.x.x, m.y.x, m.w.x, m.x.y, m.y.y, m.w.y, m.x.z, m.y.z, m.w.z) /* m43 */
        },
        (vec4){
            sarrus(m.x.y, m.y.y, m.z.y, m.x.z, m.y.z, m.z.z, m.x.w, m.y.w, m.z.w) /* m14 */,
            sarrus(m.x.x, m.y.x, m.z.x, m.x.z, m.y.z, m.z.z, m.x.w, m.y.w, m.z.w) /* m24 */,
            sarrus(m.x.x, m.y.x, m.z.x, m.x.y, m.y.y, m.z.y, m.x.w, m.y.w, m.z.w) /* m34 */,
            sarrus(m.x.x, m.y.x, m.z.x, m.x.y, m.y.y, m.z.y, m.x.z, m.y.z, m.z.z) /* m44 */
        },
    };
}

mat4 cofact_mat4(mat4 m)
{
    return (mat4){
        (vec4){m.x.x, m.x.y * -1, m.x.z, m.x.w * -1},
        (vec4){m.y.x * -1, m.y.y, m.y.z * -1, m.y.w},
        (vec4){m.z.x, m.z.y * -1, m.z.z, m.z.w * -1},
        (vec4){m.w.x * -1, m.w.y, m.w.z * -1, m.w.w}
    };
}

float determ_mat4(mat4 a) //Assume given a mat4.
{
    mat4 m = minor_mat4(a);
    return a.x.x*m.x.x - a.y.x*m.y.x + a.z.x*m.z.x -  a.w.x*m.w.x;
}

mat4 inv_mat4(mat4 m)
{
    return scalar_mult_mat4(1/determ_mat4(m), trans_mat4(cofact_mat4(minor_mat4(m))));
};

mat4 trans_mat4(mat4 m) //Transpose
{
    return (mat4){
        (vec4){m.x.x, m.y.x, m.z.x, m.w.x},
        (vec4){m.x.y, m.y.y, m.z.y, m.w.y},
        (vec4){m.x.z, m.y.z, m.z.z, m.w.z},
        (vec4){m.x.w, m.y.w, m.z.w, m.w.w}
    };
}

vec4 mat4_mult_v4(mat4 m, vec4 v)
{
    return (vec4){
        (m.x.x * v.x) + (m.y.x * v.y) + (m.z.x * v.z) + (m.w.x * v.w),
        (m.x.y * v.x) + (m.y.y * v.y) + (m.z.y * v.z) + (m.w.y * v.w),
        (m.x.z * v.x) + (m.y.z * v.y) + (m.z.z * v.z) + (m.w.z * v.w),
        (m.x.w * v.x) + (m.y.w * v.y) + (m.z.w * v.z) + (m.w.w * v.w)
    };
}

mat4 translate_mat4(float x, float y, float z)
{
    return (mat4){
        (vec4){1, 0, 0, 0},
        (vec4){0, 1, 0, 0},
        (vec4){0, 0, 1, 0},
        (vec4){x, y, z, 1}
    };
}

mat4 scale_mat4(float scaleX, float scaleY, float scaleZ)
{
    return (mat4){
        (vec4){scaleX, 0, 0, 0},
        (vec4){0, scaleY, 0, 0},
        (vec4){0, 0, scaleZ, 0},
        (vec4){0, 0, 0, 1}
    };
}
mat4 rotateX_mat4(float degrees)
{
    double radians = (double) degrees*M_PI/180.00;
    return (mat4){
        (vec4){1, 0, 0, 0},
        (vec4){0, (float) cos(radians), (float) -1 * sin(radians), 0},
        (vec4){0, (float) sin(radians), (float) cos(radians), 0},
        (vec4){0, 0, 0, 1}
    };
}

mat4 rotateX_mat4_arb(float ay, float az, float d)
{
    return (mat4){
        (vec4){1, 0, 0, 0},
        (vec4){0, az/d, -1 * ay/d, 0},
        (vec4){0, ay/d, az/d, 0},
        (vec4){0, 0, 0, 1}
    };
}

mat4 rotateY_mat4(float degrees)
{
    double radians = (double) degrees*M_PI/180.00;
    return (mat4){
        (vec4){(float) cos(radians), 0, (float) sin(radians), 0},
        (vec4){0, 1, 0, 0},
        (vec4){(float) -1 * sin(radians), 0, (float) cos(radians), 0},
        (vec4){0, 0, 0, 1}
    };
}

mat4 rotateY_mat4_arb(float ax, float d)
{
    return (mat4){
        (vec4){d, 0, ax, 0},
        (vec4){0, 1, 0, 0},
        (vec4){-1 * ax, 0, d, 0},
        (vec4){0, 0, 0, 1}
    };
}

mat4 rotateZ_mat4(float degrees)
{
    double radians = (double) degrees*M_PI/180.00;
    return (mat4){
        (vec4){(float) cos(radians), (float) -1 * sin(radians), 0, 0},
        (vec4){(float) sin(radians), (float) cos(radians), 0, 0},
        (vec4){0, 0, 1, 0},
        (vec4){0, 0, 0, 1}
    };
}


mat4 look_at(vec4 eyePoint, vec4 atPoint, vec4 upVector)
{
    vec4 vpn = v4_sub_v4(eyePoint, atPoint);
    float d = mag_v4(vpn);
    vec4 n = normalize_v4(vpn); //z'
    vec4 upVcrossn = cross_prod_v4(upVector, n);
    vec4 u = normalize_v4(upVcrossn); //x'
    vec4 ncrossu = cross_prod_v4(n, u);
    vec4 v = normalize_v4(ncrossu); //y'

    /*printf("Eyepoint \n");
    print_v4(eyePoint);
    printf("atPoint \n");
    print_v4(atPoint);
    printf("upVector \n");
    print_v4(upVector);
    printf("upVcrossn \n");
    print_v4(upVcrossn);
    printf("VPN \n");
    print_v4(vpn);
    printf("n \n");
    print_v4(n);
    printf("u \n");
    print_v4(u);
    printf("v \n");
    print_v4(v);*/

    mat4 m = {(vec4) {u.x, v.x, n.x, 0.0},
    (vec4) {u.y, v.y, n.y, 0.0},
    (vec4) {u.z, v.z, n.z, 0.0},
    (vec4) {0.0, 0.0, 0.0, 1.0},
    };

    //printf("m \n");
    //print_mat4(m);

    mat4 translate = translate_mat4(-1* eyePoint.x, -1* eyePoint.y, -1* eyePoint.z);

    //printf("translate \n");
    //print_mat4(translate);

    return mat4_mult_mat4(m, translate);
}

mat4 ortho(float left, float right, float bottom, float top,float near, float far)
{
    return (mat4) {};
}

mat4 frustum(float left, float right, float bottom, float top,float near, float far)
{
    return (mat4) {};
}