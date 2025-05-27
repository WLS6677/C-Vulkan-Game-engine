#ifndef WL_CHUNK 
#define WL_CHUNK

#include <core.h>

//
//  this engine relies on 16x16x16 meter chunks, where each chunk has its own SVO
//  the purpose of this splitting is to support massive procedural generation while keeping edited chunks and supporting LODs
//
// TODO: put the chunks into a SVO or world LOD thingy idk
// the world/chunks would live inside an SVO thats the same resolution as the chunk SVOs which is 4096 meters
// then that SVO needs to be inside another SVO
//
// TODO: serialize the edited chunks
// the main way the engine will handle the SVOs is through a sample point function
// the entire world has to be defined with a sample function at a point
// will have a hashtable list of edited chunks
//

typedef struct WLChunk {
 
    // runtime Sparse-Voxel-Octee that will be sent when chunk is loaded
    // its an index to the root node of an SVO with its metadeta
    SVOInstance SVO;
    
    // this is a flag for if the chunk has been edited which means it wont be sampled to get the data form it
    // and it will need to be stored on disk, and the runtime SVO will be loaded form disk
    bool is_edited;

    // this will be a hash generated based on wolrd position and will be used for various accessing of the chunk
    // TODO: make the hasing function and actually access the chunk from disk
    uint64_t unique_id;
} WLChunk;

WLRenderObject WLGetChunkSVOMesh(WLChunk chunk);

#endif