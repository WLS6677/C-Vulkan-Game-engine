#include <wlmath.h>



WLU32Vec2 wlVecAdd(WLU32Vec2 first, WLU32Vec2 second) {
    first.x += second.x;
    first.y += second.y;
    return first;
}