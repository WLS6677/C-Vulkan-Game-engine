#ifndef WL_SPARSE_VOXEL_OCTREE
#define WL_SPARSE_VOXEL_OCTREE

// this is the SVO API for creating and reading from SVOs

#include <core.h>

typedef uint32_t SVONode;
// the 31st (1)bit of this uint32_t is used as flag which will
// split the nodes into either branch nodes of leaf notes:
//
//  branch nodes: 
//  they will use 23-30 (8)bits for a bit mask of which child nodes exist and which are "sparse"
//  then the branch will use the ramaing 0-22 (23)bits for an index offset which points to the first child
//
//  leaf nodes:
//  they use the rest of the bits on data (colour/material index)



#ifndef WL_SVO_INSTANCE
#define WL_SVO_INSTANCE
// reference to the root node of an SVO on the SVO system
typedef uint32_t SVOInstance;
#endif

enum SVOLevel {
    WL_SVO_LEVEL_1_16th_METER   = 0,
    WL_SVO_LEVEL_1_8th_METER    = 1,
    WL_SVO_LEVEL_1_4th_METER    = 2,
    WL_SVO_LEVEL_half_METER     = 3,
    WL_SVO_LEVEL_1_METER        = 4,
    WL_SVO_LEVEL_2_METERS       = 5,
    WL_SVO_LEVEL_4_METERS       = 6,
    WL_SVO_LEVEL_8_METERS       = 7,
    WL_SVO_LEVEL_16_METERS      = 8,
    WL_SVO_LEVEL_32_METERS      = 9,
};

// when using "copy SVO to SVO" the function will need to know how to blend the two datas of the two SVO
enum SVOBlendType {
    SVO_BLEND_OVERWRITE, // will straight up replace all the voxels in the first SVO with the non_air voxels in the second SVO
};

// allocates the SVO memory arena which will use 2^23 or ~8mb memory blocks
// if it runs out it will attempt to allocate a new block of 2^23 and move over some SVOs to it in the bakcground
void wlInitSVO();
// creates the root of an SVO with the specified data and gives you the handle
SVOInstance wlCreateSVOInstance(vec3f root_positon ,SVOLevel root_level, SVOLevel smallest_size);
// fills a region of voxels with a material if the function returns true when it is sampled

void wlGenerateSVOWithRegion(SVOInstance root, bool (sample_function)(vec3f), uint32_t material);
void wlReadSVO(SVOInstance SVO_root, void (read_voxel)(vec3f/*voxel position*/, float/*voxel side length*/, uint32_t/*neighbouring voxels mask*/, uint32_t/*material*/));

#endif