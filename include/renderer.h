#ifndef WL_RENDERER
#define WL_RENDERER

#include <core.h>

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

typedef struct WLVertex{
    vec3f Pos, Color;
} WLVertex;
typedef struct WLRenderObject {
    WLVertex* pVertex_buffer;
    uint32_t vertex_count;
    uint32_t unique_id;
} WLRenderObject;

void wlCreateRenderer(void* window_hanlde);
void wlRender();
void wlDestroyRenderer();

void wlCreateSwapChain(void* window_handle);
void wlCreateRasterizedRenderPipelineLayout();
void wlCreateBasicPipeLine();

#endif