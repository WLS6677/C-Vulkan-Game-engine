#ifndef WL_MATH
#define WL_MATH

#include <stdint.h>

// Vec2
typedef struct {
    uint32_t x, y;
} WLU32Vec2;

WLU32Vec2 operator+(WLU32Vec2 l, WLU32Vec2 r);

// Vec3
typedef struct WLFLOATVec3 {
    float x, y, z;
} WLFLOATVec3;

#endif