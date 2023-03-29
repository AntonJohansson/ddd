#pragma once

#include "vec.h"

typedef struct Quat {
    Vec3 v;
    float s;
} Quat;

static inline Quat quat_from_axis_angle(Vec3 axis, float angle) {
    const float sin_half_angle = sinf(angle/2.0f);
    const float cos_half_angle = cosf(angle/2.0f);
    return (Quat) {
        .v = {
            .x = sin_half_angle*axis.x,
            .y = sin_half_angle*axis.y,
            .z = sin_half_angle*axis.z,
        },
        .s = cos_half_angle,
    };
}

static inline Quat quat_mul(Quat a, Quat b) {
    return (Quat) {
        .v = vec3_add(vec3_add(vec3_scale(b.s, a.v), vec3_scale(a.s, b.v)), vec3_cross(a.v, b.v)),
        .s = a.s*b.s - vec3_dot(a.v, b.v),
    };
}

static inline float quat_len2(Quat q) {
    return q.v.x*q.v.x + q.v.y*q.v.y + q.v.z*q.v.z + q.s*q.s;
}

static inline float quat_len(Quat q) {
    return sqrtf(quat_len2(q));
}

static inline Quat quat_normalize(Quat q) {
    const float len = quat_len(q);
    return (Quat) {
        .v = {
            .x = q.v.x / len,
            .y = q.v.y / len,
            .z = q.v.z / len,
        },
        .s = q.s / len,
    };
}

static inline Quat quat_conjugate(Quat q) {
    return (Quat) {
        .v = vec3_neg(q.v),
        .s = q.s,
    };
}

static inline Vec3 quat_rotate(Quat q, Vec3 v) {
    q = quat_normalize(q);
    Quat p = quat_mul(q, quat_mul((Quat){.v = v, .s = 0}, quat_conjugate(q)));
    return p.v;
}
