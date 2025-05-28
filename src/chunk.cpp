#include <chunk.h>

#define MAX_SVO_VERTEX_COUNT (1024*1024*1)

static WLVertex* WRITE_TARGET = NULL;
static uint32_t WRITE_VERTEX_COUNT = 0;

bool terrain_sample_function(vec3f sample_position){
    float temp;
    temp = 5*sin(sample_position.x*sample_position.y);
    return sample_position.z < temp;
}

void set_vertex_write_target(WLRenderObject* pRender_object_target){
     WL_LOG(WL_LOG_TRACE, "creating the mesh buffer");
    pRender_object_target->pVertex_buffer = (WLVertex*)wlAlloc(MAX_SVO_VERTEX_COUNT*sizeof(WLVertex));
    WRITE_TARGET = pRender_object_target->pVertex_buffer;
    WRITE_VERTEX_COUNT = 0;
    WL_LOG(WL_LOG_TRACE, "mesh buffer allocated successfully");
}
void voxel_pos_to_vertex_data(vec3f voxel_pos, float voxel_length, uint32_t neighbours_mask, uint32_t material){
    //printf("creating mesh for voxel: (%f, %f, %f)\n", voxel_pos.x,  voxel_pos.y, voxel_pos.z);
    // up face
    WRITE_TARGET[WRITE_VERTEX_COUNT+0] = {voxel_pos + vec3f{0,0,0}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+1] = {voxel_pos + vec3f{1,1,0}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+2] = {voxel_pos + vec3f{0,1,0}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+3] = {voxel_pos + vec3f{0,0,0}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+4] = {voxel_pos + vec3f{1,0,0}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+5] = {voxel_pos + vec3f{1,1,0}*voxel_length, {voxel_pos*16.0f}};
    WRITE_VERTEX_COUNT += 6;

    // down face
    WRITE_TARGET[WRITE_VERTEX_COUNT+0] = {voxel_pos + vec3f{0,0,1}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+1] = {voxel_pos + vec3f{1,1,1}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+2] = {voxel_pos + vec3f{1,0,1}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+3] = {voxel_pos + vec3f{0,0,1}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+4] = {voxel_pos + vec3f{0,1,1}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+5] = {voxel_pos + vec3f{1,1,1}*voxel_length, {voxel_pos*16.0f}};
    WRITE_VERTEX_COUNT += 6;

    // north face
    WRITE_TARGET[WRITE_VERTEX_COUNT+0] = {voxel_pos + vec3f{0,1,0}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+1] = {voxel_pos + vec3f{1,1,1}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+2] = {voxel_pos + vec3f{0,1,1}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+3] = {voxel_pos + vec3f{0,1,0}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+4] = {voxel_pos + vec3f{1,1,0}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+5] = {voxel_pos + vec3f{1,1,1}*voxel_length, {voxel_pos*16.0f}};
    WRITE_VERTEX_COUNT += 6;

    // south
    WRITE_TARGET[WRITE_VERTEX_COUNT+0] = {voxel_pos + vec3f{0,0,0}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+1] = {voxel_pos + vec3f{1,0,1}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+2] = {voxel_pos + vec3f{1,0,0}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+3] = {voxel_pos + vec3f{0,0,0}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+4] = {voxel_pos + vec3f{0,0,1}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+5] = {voxel_pos + vec3f{1,0,1}*voxel_length, {voxel_pos*16.0f}};
    WRITE_VERTEX_COUNT += 6;

    // east
    WRITE_TARGET[WRITE_VERTEX_COUNT+0] = {voxel_pos + vec3f{1,0,0}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+1] = {voxel_pos + vec3f{1,1,1}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+2] = {voxel_pos + vec3f{1,1,0}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+3] = {voxel_pos + vec3f{1,0,0}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+4] = {voxel_pos + vec3f{1,0,1}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+5] = {voxel_pos + vec3f{1,1,1}*voxel_length, {voxel_pos*16.0f}};
    WRITE_VERTEX_COUNT += 6;

    // west
    WRITE_TARGET[WRITE_VERTEX_COUNT+0] = {voxel_pos + vec3f{0,0,0}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+1] = {voxel_pos + vec3f{0,1,1}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+2] = {voxel_pos + vec3f{0,0,1}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+3] = {voxel_pos + vec3f{0,0,0}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+4] = {voxel_pos + vec3f{0,1,0}*voxel_length, {voxel_pos*16.0f}};
    WRITE_TARGET[WRITE_VERTEX_COUNT+5] = {voxel_pos + vec3f{0,1,1}*voxel_length, {voxel_pos*16.0f}};
    WRITE_VERTEX_COUNT += 6;
}

void wlGenerateChunkSVO(WLChunk* chunk){
    srand(5648275);
    wlGenerateSVOWithSampleFunction(chunk->SVO , terrain_sample_function, 69420);
}
WLRenderObject WLGetChunkSVOMesh(WLChunk chunk){
    WLRenderObject chunk_mesh;
    set_vertex_write_target(&chunk_mesh);
    wlReadSVO(chunk.SVO, voxel_pos_to_vertex_data);
    chunk_mesh.vertex_count = WRITE_VERTEX_COUNT;
    return chunk_mesh;
}