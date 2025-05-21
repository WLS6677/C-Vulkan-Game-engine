#ifndef WL_RENDERER
#define WL_RENDERER

#include <core.h>
#include <vulkan/vulkan.h>

#ifdef _WIN32_
#define VK_USE_PLATFORM_WIN32_KHR
#endif

typedef struct WLRenderer WLRenderer;
typedef struct WLRenderer WKRenderer;

WLRenderer* wlCreateRenderer(WLRenderer* renderer);
WLResult wlRender(WLRenderer* renderer);
WLResult wlDestroyRenderer(WLRenderer* renderer);

#endif