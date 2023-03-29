#pragma once

#include "vec.h"
#include "camera.h"

#include <assert.h>
#include <math.h>
#include <stdbool.h>

typedef struct Mat4 {
    float m00, m01, m02, m03;
    float m10, m11, m12, m13;
    float m20, m21, m22, m23;
    float m30, m31, m32, m33;
} Mat4;

const float epsilon = 0.00001f;
static inline bool float_equal(float a, float b) {
    return fabs(a-b) <= epsilon;
}

static inline bool mat4_equal(Mat4 a, Mat4 b) {
    return float_equal(a.m00, b.m00) && float_equal(a.m01, b.m01) && float_equal(a.m02, b.m02) && float_equal(a.m03, b.m03) &&
           float_equal(a.m10, b.m10) && float_equal(a.m11, b.m11) && float_equal(a.m12, b.m12) && float_equal(a.m13, b.m13) &&
           float_equal(a.m20, b.m20) && float_equal(a.m21, b.m21) && float_equal(a.m22, b.m22) && float_equal(a.m23, b.m23) &&
           float_equal(a.m30, b.m30) && float_equal(a.m31, b.m31) && float_equal(a.m32, b.m32) && float_equal(a.m33, b.m33);
}

static inline void mat4_print(Mat4 m) {
    printf("%5.2f%5.2f%5.2f%5.2f\n", m.m00, m.m01, m.m02, m.m03);
    printf("%5.2f%5.2f%5.2f%5.2f\n", m.m10, m.m11, m.m12, m.m13);
    printf("%5.2f%5.2f%5.2f%5.2f\n", m.m20, m.m21, m.m22, m.m23);
    printf("%5.2f%5.2f%5.2f%5.2f\n", m.m30, m.m31, m.m32, m.m33);
}

static inline Mat4 mat4_identity() {
    return (Mat4){
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };
}

static inline Mat4 mat4_transpose(Mat4 m) {
    return (Mat4){
        m.m00, m.m10, m.m20, m.m30,
        m.m01, m.m11, m.m21, m.m31,
        m.m02, m.m12, m.m22, m.m32,
        m.m03, m.m13, m.m23, m.m33,
    };
}

static inline Mat4 mat4_mul(Mat4 a, Mat4 b) {
    Mat4 res;

    res.m00 = a.m00*b.m00 + a.m01*b.m10 + a.m02*b.m20 + a.m03*b.m30;
    res.m01 = a.m00*b.m01 + a.m01*b.m11 + a.m02*b.m21 + a.m03*b.m31;
    res.m02 = a.m00*b.m02 + a.m01*b.m12 + a.m02*b.m22 + a.m03*b.m32;
    res.m03 = a.m00*b.m03 + a.m01*b.m13 + a.m02*b.m23 + a.m03*b.m33;

    res.m10 = a.m10*b.m00 + a.m11*b.m10 + a.m12*b.m20 + a.m13*b.m30;
    res.m11 = a.m10*b.m01 + a.m11*b.m11 + a.m12*b.m21 + a.m13*b.m31;
    res.m12 = a.m10*b.m02 + a.m11*b.m12 + a.m12*b.m22 + a.m13*b.m32;
    res.m13 = a.m10*b.m03 + a.m11*b.m13 + a.m12*b.m23 + a.m13*b.m33;

    res.m20 = a.m20*b.m00 + a.m21*b.m10 + a.m22*b.m20 + a.m23*b.m30;
    res.m21 = a.m20*b.m01 + a.m21*b.m11 + a.m22*b.m21 + a.m23*b.m31;
    res.m22 = a.m20*b.m02 + a.m21*b.m12 + a.m22*b.m22 + a.m23*b.m32;
    res.m23 = a.m20*b.m03 + a.m21*b.m13 + a.m22*b.m23 + a.m23*b.m33;

    res.m30 = a.m30*b.m00 + a.m31*b.m10 + a.m32*b.m20 + a.m33*b.m30;
    res.m31 = a.m30*b.m01 + a.m31*b.m11 + a.m32*b.m21 + a.m33*b.m31;
    res.m32 = a.m30*b.m02 + a.m31*b.m12 + a.m32*b.m22 + a.m33*b.m32;
    res.m33 = a.m30*b.m03 + a.m31*b.m13 + a.m32*b.m23 + a.m33*b.m33;

    return res;
}

static inline Vec4 mat4_mulv(Mat4 m, Vec4 v) {
    return (Vec4) {
        .x = v.x*m.m00 + v.y*m.m01 + v.z*m.m02 + v.w*m.m03,
        .y = v.x*m.m10 + v.y*m.m11 + v.z*m.m12 + v.w*m.m13,
        .z = v.x*m.m20 + v.y*m.m21 + v.z*m.m22 + v.w*m.m23,
        .w = v.x*m.m30 + v.y*m.m31 + v.z*m.m32 + v.w*m.m33,
    };
}

static inline Mat4 mat4_inverse(Mat4 m) {
    //
    // M^-1 = (1/det(M)) * adj(M)
    //
    // adj(M) = transpose(cofactor(M))
    //

    //
    // 00  01  02  03
    // 10  11  12  13
    // 20  21  22  23
    // 30  31  32  33
    //
    // + - + -
    // - + - +
    // + - + -
    // - + - +
    //

    // Compute cofactor matrix
    const float c00 = + (m.m11*(m.m22*m.m33 - m.m23*m.m32) + m.m12*(m.m23*m.m31 - m.m21*m.m33) + m.m13*(m.m21*m.m32 - m.m22*m.m31));
    const float c01 = - (m.m10*(m.m22*m.m33 - m.m23*m.m32) + m.m12*(m.m23*m.m30 - m.m20*m.m33) + m.m13*(m.m20*m.m32 - m.m22*m.m30));
    const float c02 = + (m.m10*(m.m21*m.m33 - m.m23*m.m31) + m.m11*(m.m23*m.m30 - m.m20*m.m33) + m.m13*(m.m20*m.m31 - m.m21*m.m30));
    const float c03 = - (m.m10*(m.m21*m.m32 - m.m22*m.m31) + m.m11*(m.m22*m.m30 - m.m20*m.m32) + m.m12*(m.m20*m.m31 - m.m21*m.m30));

    const float c10 = - (m.m01*(m.m22*m.m33 - m.m23*m.m32) + m.m02*(m.m23*m.m31 - m.m21*m.m33) + m.m03*(m.m21*m.m32 - m.m22*m.m31));
    const float c11 = + (m.m00*(m.m22*m.m33 - m.m23*m.m32) + m.m02*(m.m23*m.m30 - m.m20*m.m33) + m.m03*(m.m20*m.m32 - m.m22*m.m30));
    const float c12 = - (m.m00*(m.m21*m.m33 - m.m23*m.m31) + m.m01*(m.m23*m.m30 - m.m20*m.m33) + m.m03*(m.m20*m.m31 - m.m21*m.m30));
    const float c13 = + (m.m00*(m.m21*m.m32 - m.m22*m.m31) + m.m01*(m.m22*m.m30 - m.m20*m.m32) + m.m02*(m.m20*m.m31 - m.m21*m.m30));

    const float c20 = + (m.m01*(m.m12*m.m33 - m.m13*m.m32) + m.m02*(m.m13*m.m31 - m.m11*m.m33) + m.m03*(m.m11*m.m32 - m.m12*m.m31));
    const float c21 = - (m.m00*(m.m12*m.m33 - m.m13*m.m32) + m.m02*(m.m13*m.m30 - m.m10*m.m33) + m.m03*(m.m10*m.m32 - m.m12*m.m30));
    const float c22 = + (m.m00*(m.m11*m.m33 - m.m13*m.m32) + m.m01*(m.m13*m.m30 - m.m10*m.m33) + m.m03*(m.m10*m.m31 - m.m11*m.m30));
    const float c23 = - (m.m00*(m.m11*m.m32 - m.m12*m.m31) + m.m01*(m.m12*m.m30 - m.m10*m.m32) + m.m02*(m.m10*m.m31 - m.m11*m.m30));

    const float c30 = - (m.m01*(m.m12*m.m23 - m.m13*m.m22) + m.m02*(m.m13*m.m21 - m.m11*m.m23) + m.m03*(m.m11*m.m22 - m.m12*m.m21));
    const float c31 = + (m.m00*(m.m12*m.m23 - m.m13*m.m22) + m.m02*(m.m13*m.m20 - m.m10*m.m23) + m.m03*(m.m10*m.m22 - m.m12*m.m20));
    const float c32 = - (m.m00*(m.m11*m.m23 - m.m13*m.m21) + m.m01*(m.m13*m.m20 - m.m10*m.m23) + m.m03*(m.m10*m.m21 - m.m11*m.m20));
    const float c33 = + (m.m00*(m.m11*m.m22 - m.m12*m.m21) + m.m01*(m.m12*m.m20 - m.m10*m.m22) + m.m02*(m.m10*m.m21 - m.m11*m.m20));

    // Compute determinant from cofactors from a row expansion along the first row
    const float determinant = m.m00*c00 + m.m01*c01 + m.m02*c02 + m.m03*c03;
    assert(!float_equal(determinant, 0.0f));

    const float scale = 1.0f / fabsf(determinant);

    // Compute adjugate by tranposing cofactor, the inverse
    // is then obtained by scaling with one over the determinant.
    const Mat4 inverse = {
        scale*c00, scale*c10, scale*c20, scale*c30,
        scale*c01, scale*c11, scale*c21, scale*c31,
        scale*c02, scale*c12, scale*c22, scale*c32,
        scale*c03, scale*c13, scale*c23, scale*c33,
    };

    return inverse;
}

static inline Mat4 mat4_model(Vec3 translation) {
    return (Mat4) {
        1, 0, 0, -translation.x,
        0, 1, 0, -translation.y,
        0, 0, 1, -translation.z,
        0, 0, 0, 1,
    };
}

static inline Mat4 mat4_view_from_camera(Camera camera) {
    Mat4 view_to_world = {
        camera.i.x,   camera.j.x,   camera.k.x,   camera.pos.x,
        camera.i.y,   camera.j.y,   camera.k.y,   camera.pos.y,
        camera.i.z,   camera.j.z,   camera.k.z,   camera.pos.z,
        0,     0,     0,     1,
    };

    Mat4 world_to_view = mat4_inverse(view_to_world);

    // Here we perform a redudency check to make sure the matrix
    // inverse is actually the inverse.
    assert(mat4_equal(mat4_mul(world_to_view, view_to_world), mat4_identity()));

    return world_to_view;
}


static inline Mat4 mat4_view(Vec3 pos, Vec3 dir) {
    const Vec3 world_up = {0,1,0};

    dir = vec3_normalize(dir);

    const Vec3 i = vec3_cross(dir, world_up);
    const Vec3 j = vec3_cross(i, dir);
    const Vec3 k = vec3_neg(dir);

    Mat4 view_to_world = {
        i.x,   j.x,   k.x,   pos.x,
        i.y,   j.y,   k.y,   pos.y,
        i.z,   j.z,   k.z,   pos.z,
        0,     0,     0,     1,
    };

    Mat4 world_to_view = mat4_inverse(view_to_world);

    // Here we perform a redudency check to make sure the matrix
    // inverse is actually the inverse.
    assert(mat4_equal(mat4_mul(world_to_view, view_to_world), mat4_identity()));

    return world_to_view;
}

#undef near
#undef far
#undef aspect
#undef fov
static inline Mat4 mat4_projection(float near, float far, float aspect, float fov) {

    //                  A  right
    //                  +-----+
    //                oo|oo   |
    //            oooo__|__oooo-------+
    //          oo\     |     /oo     |
    //         o   \   fov   /   o    |
    //        8     \   |   /     8   | near
    //       8       \  |  /       8  |
    //       8        \ | /        8  |
    //      8          \|/ angle    8 |
    // <----------------+-------------+->
    //
    // tan(angle) = near/right
    // => right = near/tan(angle)

    const float angle = M_PI*(1.0f - fov/180.0f)/2.0f;
    const float right = near/tanf(angle);
    const float top = right/aspect;

    Mat4 proj = {
        near/right, 0,        0,                         0,
        0,          near/top, 0,                         0,
        0,          0,        -(far+near)/(far-near),   -2.0f*near*far/(far-near),
        0,          0,        -1.0f,                     0,
    };

    return proj;
}
