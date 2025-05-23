#ifndef WL_RENDERER
#define WL_RENDERER

#include <core.h>

#ifdef _WIN32_s
#define VK_USE_PLATFORM_WIN32_KHR
#endif

typedef struct WLRenderer WLRenderer;
typedef struct WLRenderer WKRenderer;

WLRenderer* wlCreateRenderer();
void wlRender(WLRenderer* renderer);
void wlDestroyRenderer(WLRenderer* renderer);

#endif