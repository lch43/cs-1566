#include "vandmlib.h"
#include <stdio.h>

int main(void)
{
    vec4 v1 = {1, 2, 3, 4};
    vec4 v2 = {5, 6, 7, 8};

    mat4 m1 = {(vec4) {1,-5,9,13}, (vec4) {2,6,-10,14}, (vec4) {3,7,11,15}, (vec4) {4,8,12,-16}};
    mat4 m2 = {(vec4) {4,8,12,16}, (vec4) {3,7,11,15}, (vec4) {2,6,10,14}, (vec4) {1,5,9,13}};

    float s = 3.0;

    print_v4(scalar_mult_v4(s, v1));
    printf("\n");
    print_v4(v4_add_v4(v1, v2));
    printf("\n");
    print_v4(v4_sub_v4(v1, v2));
    printf("\n");
    printf("%f\n", mag_v4(v1));
    printf("\n");
    print_v4(normalize_v4(v1));
    printf("\n");
    printf("%f\n", dot_prod_v4(v1, v2));
    printf("\n");
    print_v4(cross_prod_v4(v1, v2));
    printf("\n");
    print_mat4(scalar_mult_mat4(s, m1));
    printf("\n");
    print_mat4(mat4_add_mat4(m1, m2));
    printf("\n");
    print_mat4(mat4_sub_mat4(m1, m2));
    printf("\n");
    print_mat4(mat4_mult_mat4(m1, m2));
    printf("\n");
    print_mat4(inv_mat4(m1));
    printf("\n");
    print_mat4(trans_mat4(m1));
    printf("\n");
    print_v4(mat4_mult_v4(m1, v1));
    printf("\n");


    mat4 minvTest = {
        (vec4){-4, -1, -4, -1},
        (vec4){0, 2, 4, -9},
        (vec4){0, 0, 4, -1},
        (vec4){0, 0, 0, 1}
    };

    print_mat4(minvTest);
    printf("\n");

    print_mat4(inv_mat4(minvTest));
    printf("\n");

    print_mat4(mat4_mult_mat4(minvTest, inv_mat4(minvTest)));
}