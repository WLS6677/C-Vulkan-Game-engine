#ifndef WL_RENDERER
#define WL_RENDERER

#include <core.h>

#ifdef _WIN32_s
#define VK_USE_PLATFORM_WIN32_KHR
#endif

void wlCreateRenderer(void* window_hanlde);
void wlRender();
void wlDestroyRenderer();

#endif