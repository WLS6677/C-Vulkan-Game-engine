#ifndef WL_SCENE
#define WL_SCENE

#include <core.h>
#include <glm/gtc/matrix_transform.hpp>

struct WLChunk;

struct WLCamera {
    vec3f position;
    float yaw, pitch;
};
typedef struct WLScene {
    //TODO: make the chunks dynamic
    uint32_t chunk_count;
    WLChunk* pChunks;

    struct WLCamera camera;
} WLScene;

// will convert all chunks and objects to meshes/WLRenderObjects and send them to the GPU
// TODO: make a ver that only submits changed chunks
void wlCreateSceneMeshObjects(WLScene scene);
glm::mat4x4 wlGetCameraMatrix(WLScene scene);


#endif