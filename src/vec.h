#pragma once

#include <math.h>

typedef struct Vec3 {
    float x, y, z;
} Vec3;

typedef struct Vec4 {
    float x, y, z, w;
} Vec4;

static inline Vec3 vec3_neg(Vec3 v) {
    return (Vec3) {-v.x, -v.y, -v.z};
}

static inline Vec3 vec3_add(Vec3 a, Vec3 b) {
    return (Vec3) {
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z,
    };
}

static inline Vec3 vec3_scale(float s, Vec3 v) {
    return (Vec3) {
        .x = s*v.x,
        .y = s*v.y,
        .z = s*v.z,
    };
}

static inline float vec3_dot(Vec3 a, Vec3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

static inline Vec3 vec3_cross(Vec3 a, Vec3 b) {
    // |x  y  z |
    // |xa ya za| = x(ya*zb-za*yb) + y(za*xb-xa*zb) + z(xa*yb-ya*xb)
    // |xb yb zb|
    return (Vec3) {
        .x = a.y*b.z - a.z*b.y,
        .y = a.z*b.x - a.x*b.z,
        .z = a.x*b.y - a.y*b.x,
    };
}

static inline float vec3_len2(Vec3 v) {
    return v.x*v.x + v.y*v.y + v.z*v.z;
}

static inline float vec3_len(Vec3 v) {
    return sqrtf(vec3_len2(v));
}

static inline Vec3 vec3_normalize(Vec3 v) {
    const float scale = 1.0f / vec3_len(v);
    return (Vec3) {scale*v.x, scale*v.y, scale*v.z};
}
