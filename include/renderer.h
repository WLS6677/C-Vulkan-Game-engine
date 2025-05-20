#ifndef WL_RENDERER
#define WL_RENDERER

#include <core.h>
#include <vulkan/vulkan.h>



#ifdef _WIN32_
#define VK_USE_PLATFORM_WIN32_KHR
#endif

WLResult wlCreateRenderer();
WLResult wlInitRenderer();
WLResult wlBindObjectBuffer();
WLResult wlDestroyRenderer();



#endif