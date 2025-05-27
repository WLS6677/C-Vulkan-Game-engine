#ifndef WL_RENDERER
#define WL_RENDERER

#include <core.h>

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#ifndef WL_VERTEX_OBJECT
#define WL_VERTEX_OBJECT
typedef struct WLVertex{
    vec3f Pos, Color;
} WLVertex;
typedef struct WLRenderObject {
    WLVertex* pVertex_buffer;
    uint32_t vertex_count;
    uint32_t unique_id;
} WLRenderObject;
#endif

void wlCreateRenderer(void* window_hanlde);

void wlDestroyRenderer();

void wlCreateSwapChain(void* window_handle);
void wlCreateRasterizedRenderPipelineLayout();
void wlCreateBasicPipeLine();
void wlCreateCommandBuffers();
void wlCreateFrameBuffers();
void wlCreateUniformBuffers();

// rendering functions
void wlRender(glm::mat4 camera_matrix);
void wlInitVertexBuffer(const WLRenderObject* pObjects, const uint32_t object_count);
void wlUpdateCameraBuffer(glm::mat4 camera_matrix);

#endif