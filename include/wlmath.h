#ifndef WL_MATH
#define WL_MATH

#include <stdint.h>
#include <math.h>

inline float wl_fast_inverse_square_root( float number ){
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

// add
inline WL32fVec3 operator+(WL32fVec3 in_1, WL32fVec3 in_2 ){
    return WL32fVec3{
        in_1.x + in_2.x,
        in_1.y + in_2.y,
        in_1.z + in_2.z
    };
}
inline void operator+=(WL32fVec3& in_1, WL32fVec3 in_2 ){
    in_1 = WL32fVec3{
        in_1.x + in_2.x,
        in_1.y + in_2.y,
        in_1.z + in_2.z
    };
}
inline void operator-=(WL32fVec3& in_1, WL32fVec3 in_2 ){
    in_1 = WL32fVec3{
        in_1.x - in_2.x,
        in_1.y - in_2.y,
        in_1.z - in_2.z
    };
}
// scale
inline WL32fVec3 operator*(WL32fVec3 in, float scale ){
    return WL32fVec3{in.x*scale,in.y*scale,in.z*scale};
} 

// normalize
inline WL32fVec3 wlget32fVec3Normalized(WL32fVec3 in){
    return in * wl_fast_inverse_square_root(in.x*in.x + in.y*in.y + in.z*in.z);
}

// vector products
inline WL32fVec3 wlCross32fVec3(WL32fVec3 in_1, WL32fVec3 in_2){
    return WL32fVec3 {
        in_1.y*in_2.z - in_1.z*in_2.y,
        -in_1.x*in_2.z + in_1.z*in_2.x,
        in_1.x*in_2.y - in_1.y*in_2.x,
    };
}
inline float wlDot32fVec3(WL32fVec3 in_1, WL32fVec3 in_2){
    return in_1.x*in_2.x + in_1.y*in_2.y + in_1.z*in_2.z;
}

// random stuff
inline float wl_fake_random_thingy(WL32fVec3 pos){
    float hash = (pos.x * 73856093) + (pos.y * 19349663) + (pos.z * 83492791);
    hash += pow(hash, 13);
    hash *= 0x5bd1e995;
    hash += pow(hash, 5);
    return hash;
}

#endif