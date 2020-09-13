#ifndef _VANDMLIB_H_
#define _VANDMLIB_H_

typedef struct
{
    float x;
    float y;
    float z;
    float w;
} vec4;

typedef struct vandmlib
{
    /* Column Major! 
    [X1][Y1][Z1][W1]
    [X2][Y2][Z2][W2]
    [X3][Y3][Z3][W3]
    [X4][Y4][Z4][W4]
    */
    vec4 x;
    vec4 y;
    vec4 z;
    vec4 w;
} mat4;

void print_v4(vec4 v);

vec4 scalar_mult_v4(float s, vec4 v);

vec4 v4_add_v4(vec4 a, vec4 b);

vec4 v4_sub_v4(vec4 a, vec4 b);

float mag_v4(vec4 v);

vec4 normalize_v4(vec4 v);

float dot_prod_v4(vec4 a, vec4 b);

vec4 cross_prod_v4(vec4 a, vec4 b);

void print_mat4(mat4 m);

mat4 scalar_mult_mat4(float s, mat4 v);

mat4 mat4_add_mat4(mat4 a, mat4 b);

mat4 mat4_sub_mat4(mat4 a, mat4 b);

mat4 mat4_mult_mat4(mat4 a, mat4 b);

mat4 inv_mat4(mat4 m);

mat4 trans_mat4(mat4 m);

vec4 mat4_mult_v4(mat4 m, vec4 v);

/*Helper functions that I would like to have just in case

float sarrus(float a, float b, float c, float d, float e, float f, float g, float h, float i);

mat4 minor_mat4(mat4 m);

mat4 cofact_mat4(mat4 m);
*/

#endif