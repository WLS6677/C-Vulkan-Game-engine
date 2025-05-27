#include <chunk.h>

#define MAX_SVO_VERTEX_COUNT (1024*1024*8)

static WLVertex* WRITE_TARGET = NULL;
static uint32_t WRITE_VERTEX_COUNT = 0;

void set_vertex_write_target(WLRenderObject* pRender_object_target){
    pRender_object_target->pVertex_buffer = (WLVertex*)wlAlloc(MAX_SVO_VERTEX_COUNT*sizeof(WLVertex));
    WRITE_TARGET = pRender_object_target->pVertex_buffer;
    WRITE_VERTEX_COUNT = 0;
}
void voxel_pos_to_vertex_data(vec3f voxel_pos, float voxel_length, uint32_t neighbours_mask, uint32_t material){
    // make mesh idk
}

WLRenderObject WLGetChunkSVOMesh(WLChunk chunk){
    WLRenderObject chunk_mesh;
    set_vertex_write_target(&chunk_mesh);
    wlReadSVO(chunk.SVO, voxel_pos_to_vertex_data);
    chunk_mesh.vertex_count = WRITE_VERTEX_COUNT;
    return chunk_mesh;
}