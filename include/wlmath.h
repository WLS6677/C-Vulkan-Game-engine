#ifndef WL_MATH
#define WL_MATH

#include <stdint.h>

float fast_inverse_square_root( float number ){
	int32_t i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}

// Vec2
typedef struct {
    uint32_t x, y;
} WL32uVec2;
typedef struct {
    float x, y;
} WL32fVec2;

// Vec3
typedef struct {
    float x, y, z;
} WL32fVec3;

// scale
WL32fVec3 operator*(WL32fVec3 in, float scale ){
    return WL32fVec3{in.x*scale,in.y*scale,in.z*scale};
} 

// normalize
WL32fVec3 wlget32fVec3Normalized(WL32fVec3 in){
    return in * fast_inverse_square_root(in.x*in.x + in.y*in.y + in.z*in.z);
}

// vector products
WL32fVec3 wlCross32fVec3(WL32fVec3 in_1, WL32fVec3 in_2){
    return WL32fVec3 {
        in_1.y*in_2.z - in_1.z*in_2.y,
        -in_1.x*in_2.z + in_1.z*in_2.x,
        in_1.x*in_2.y - in_1.y*in_2.x,
    };
}
float wlDot32fVec3(WL32fVec3 in_1, WL32fVec3 in_2){
    return in_1.x*in_2.x + in_1.y*in_2.y + in_1.z*in_2.z;
}


#endif