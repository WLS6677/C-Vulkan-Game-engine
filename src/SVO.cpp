#include <SVO.h>
#include <math.h>

#define MAX_SVO_NODE_COUNT ( 1<<23 )
#define MAX_FREED_INDICES ( 1<<20 )
#define SMALLEST_VOXEL_LENGTH 5.0f

#define ROOT_OFFSET_LEVEL 0
#define ROOT_OFFSET_SMALLEST_LEVEL 1
#define ROOT_OFFSET_X 2
#define ROOT_OFFSET_Y 3
#define ROOT_OFFSET_Z 4
#define ROOT_OFFSET_DATA 5
#define ROOT_DATA_AMMOUNT 6

//typedef uint32_t SVONode;
// the 31st (1)bit of this uint32_t is used as flag which will
// split the nodes into either branch nodes of leaf notes:
//
//  branch nodes: 
//  they will use 23-30 (8)bits for a bit mask of which child nodes exist and which are "sparse"
//  then the branch will use the ramaing 0-22 (23)bits for an index offset which points to the first child
//
//  leaf nodes:
//  they use the rest of the bits on data (colour/material index)

#ifdef WL_GENERATE_NOISE
#define RANDOM_BOOL ((bool)rand()%2);
#endif
#ifdef WL_DEBUG
    static int debug_voxel_counter = 0;

void print_binary(uint32_t value) {
    for (int i = 31; i >= 0; i--) {
        printf("%c", (value & (1U << i)) ? '1' : '0');
        if (i % 8 == 0) printf(" "); // Optional: separate into bytes
    }
    printf("\n");
}
#endif

// used to get child offsets when recursivly creating the SVO
static struct {
    uint32_t* SVO_array; // this is the main array from which the SVO will alloc and dealloc
    uint32_t* freed_indices;
    uint32_t allocated_ammount;
    uint32_t freed_indices_count;
} SVO_manager = {
    NULL,   // SVO_array
    NULL,   // freed_indices
    0,      // allocated ammount
    0       // freed_indices_count
};

void wlInitSVO() {
    WL_LOG(WL_LOG_TRACE, "initiating SVO...");
    SVO_manager.SVO_array = (uint32_t*)wlAlloc((MAX_SVO_NODE_COUNT)*sizeof(SVONode));
    SVO_manager.freed_indices = (uint32_t*)wlAlloc(MAX_FREED_INDICES*(sizeof(uint32_t)));
    WL_LOG(WL_LOG_TRACE, "SVO initiated successfully!");
}

uint32_t SVO_alloc_children_nodes(uint32_t children_node_count){

    if (SVO_manager.allocated_ammount + children_node_count > MAX_SVO_NODE_COUNT) {
        WL_LOG(WL_LOG_FATAL,"SVO allocation exceeded capacity");
        return UINT32_MAX;
    }
    
    uint32_t first_child_index = SVO_manager.allocated_ammount;

    SVO_manager.allocated_ammount += children_node_count;

    return first_child_index;
}
uint32_t SVO_alloc_root_node(){
    WL_LOG(WL_LOG_TRACE, "allocating root node...");

    if(SVO_manager.SVO_array == NULL){
        WL_LOG(WL_LOG_FATAL,"SVO memory not initialized");
        return UINT32_MAX;
    }

    if (SVO_manager.allocated_ammount + ROOT_DATA_AMMOUNT > MAX_SVO_NODE_COUNT) {
        WL_LOG(WL_LOG_FATAL,"SVO allocation exceeded capacity");
        return UINT32_MAX;
    }
    
    uint32_t root_index = 0;
    root_index = SVO_manager.allocated_ammount;
    // 1 for SVOLevel, 3 for a vec3f position, 1 for SVONode
    SVO_manager.allocated_ammount += ROOT_DATA_AMMOUNT;

    WL_LOG(WL_LOG_TRACE, "root node allocated successfully!");
    return root_index;
}

// creates the root of an SVO with the specified data and gives you the handle
SVOInstance wlCreateSVOInstance(vec3f root_positon ,SVOLevel root_level, SVOLevel smallest_size){
    WL_LOG(WL_LOG_TRACE, "creating SVO instane...");
    uint32_t root_offset = SVO_alloc_root_node();
    if(root_offset == UINT32_MAX){
        WL_LOG(WL_LOG_FATAL,"failed to allocate SVO instance");
        return UINT32_MAX;
    }
    uint32_t* root = SVO_manager.SVO_array + root_offset;

    WL_LOG(WL_LOG_TRACE, "setting data for the root node");
    // setting data to the root for traversing
    root[ROOT_OFFSET_LEVEL] = root_level;
    root[ROOT_OFFSET_SMALLEST_LEVEL] = smallest_size;
    root[2] = *(uint32_t*)(&root_positon.x);
    root[3] = *(uint32_t*)(&root_positon.y);
    root[4] = *(uint32_t*)(&root_positon.z);
    root[ROOT_OFFSET_DATA] = 0;

    WL_LOG(WL_LOG_TRACE, "SVO instance created Successfully!");
    return root_offset;
}

const vec3f child_offsets[8] = {
    {0, 0, 0},
    {1, 0, 0},
    {0, 1, 0},
    {1, 1, 0},
    {0, 0, 1},
    {1, 0, 1},
    {0, 1, 1},
    {1, 1, 1},
};

uint32_t sample_8_corners_of_node(vec3f node_position, float node_length,  bool (sample_function)(vec3f)){
    uint32_t mask = 0;

    for (size_t i = 0; i < 8; i++)
    {
        if(
            sample_function(
                vec3f{
                    node_position.x + child_offsets[i].x * node_length,
                    node_position.y + child_offsets[i].y * node_length,
                    node_position.z + child_offsets[i].z * node_length
                }
            )
        ){
            mask |= (1<<i);
        }
    }
    
    return mask;
}

uint32_t get_child_node_count(const uint32_t mask){
    uint32_t count = 0;
    uint32_t read_mask = mask;
    for (size_t i = 0; i < 8; i++){
        count += read_mask & 1;
        read_mask>>=1;
    }
    return count;   
}
void generate_SVO_node_recursive(SVOInstance node_offset, bool (sample_function)(vec3f), uint32_t material, uint32_t node_voxel_level, uint32_t smallest_level, vec3f node_position){

    #ifdef WL_DEBUG
    debug_voxel_counter++;
    #endif

    uint32_t* root = SVO_manager.SVO_array + node_offset;
    float full_node_length = SMALLEST_VOXEL_LENGTH * (1<<node_voxel_level);

    if(node_voxel_level==smallest_level){
        #ifndef WL_GENERATE_NOISE
        *root = material;
        #else 
        *root = (rand() & (UINT32_MAX-1));
        #endif
        return;
    }

/////////////////////////////////////////////////////
            //      sampling        //

    vec3f sample_position{
    node_position.x,
    node_position.y,
    node_position.z
    };

    
    uint32_t child_node_mask = 0;
    float half_node_length = full_node_length / 2;
    uint32_t node_is_full_sample_mask = 0;


    for (size_t Z = 0; Z < 2; Z++){
        for (size_t Y = 0; Y < 2; Y++){
            for (size_t X = 0; X < 2; X++){
                
                uint32_t temp_child_node_mask = 0;
                temp_child_node_mask = sample_8_corners_of_node(
                    vec3f { sample_position.x + X * half_node_length,
                            sample_position.y + Y * half_node_length,
                            sample_position.z + Z * half_node_length },
                    half_node_length,
                    sample_function
                );
                #ifndef WL_GENERATE_NOISE
                // if the node is part of the tree (not air) store it to be iterated over
                if(temp_child_node_mask!=0){
                    child_node_mask |= 1<<(X + 2*Y + 4*Z);
                }

                // if all 8 corners of the potential node are full, write it to the full nodes mask
                if(temp_child_node_mask==((1<<8)-1)){
                    node_is_full_sample_mask |= 1<<(X + 2*Y + 4*Z);
                }
                #else // WL_GENERATE_NOISE
                // only generates parent and air nodes and min size voxels
                if(RANDOM_BOOL){
                    child_node_mask |= 1<<(X + 2*Y + 4*Z);
                }
                #endif // WL_GENERATE_NOISE
            }
        }
    }

    *root=0;
    uint32_t child_node_count = get_child_node_count(child_node_mask);

    #ifdef WL_DEBUG_no
    printf("%u: the child node mask:", debug_voxel_counter);
    print_binary(child_node_mask);
    printf("%u: the child node count: %u\n", debug_voxel_counter, child_node_count);
    printf("%u: the full nodes mask:", debug_voxel_counter);
    print_binary(node_is_full_sample_mask);
    #endif

    
    
    if(node_is_full_sample_mask == ((1<<8) - 1)){
        *root = material;
        return;
    }
    
    

    *root = SVO_alloc_children_nodes(child_node_count);
    uint32_t children_index = *root;
    *root |= child_node_mask<<23; //storing the mask
    *root |= 1<<31; //setting the flag as branch

    // getting the order of the child nodes from the mask
    // this will assign which nth bit the child belongs to
    // this is so we can use a lookup table to get the position offset of the child node
    uint32_t counter = 0;
    uint32_t child_orders[child_node_count] = {};
    for (size_t i = 0; i < 8; i++){
        if(((child_node_mask>>i)&1)==1){
            child_orders[counter] = i;
            counter++;
        }
    }

    // this big boy will take the majority of the work
    for (size_t i = 0; i < child_node_count; i++){
        vec3f child_node_pos {
            node_position.x + child_offsets[child_orders[i]].x * half_node_length,
            node_position.y + child_offsets[child_orders[i]].y * half_node_length,
            node_position.z + child_offsets[child_orders[i]].z * half_node_length
        };
        generate_SVO_node_recursive(children_index+i, sample_function, material, node_voxel_level-1, smallest_level, child_node_pos);
    }
}

// fills a region of voxels with a material of the function which returns true when it is sampled at a point in 3d space
void wlGenerateSVOWithRegion(SVOInstance SVO_root_offset, bool (sample_function)(vec3f), uint32_t material){

    // converting the index to the pointer from the array
    uint32_t* root = SVO_manager.SVO_array + SVO_root_offset;

    WL_LOG(WL_LOG_TRACE, "the root point set");

    // extracting data from the root for traversing
    uint32_t root_voxel_level = root[ROOT_OFFSET_LEVEL];
    uint32_t smallest_level = root[ROOT_OFFSET_SMALLEST_LEVEL];
    vec3f root_position = {};
    root_position.x = *(float*)(root + 2);
    root_position.y = *(float*)(root + 3);
    root_position.z = *(float*)(root + 4);

    float full_node_length = SMALLEST_VOXEL_LENGTH * (1<<root_voxel_level);

    //for reading children nodes
    //uint32_t mask = (root_data>>23) & ((1<<8)-1);


/////////////////////////////////////////////////////
            //      sampling        //

    //
    //  the sampling will loop over the 8 possible child nodes
    //  and it will make a temporary mask which will be a sample for each corner of the child node
    //  if the temproray mask is 0 then the node is empty
    //  if not call recursive on it
    //

    WL_LOG(WL_LOG_TRACE, "sampling 8 child nodes...");

    vec3f sample_position{
    root_position.x,
    root_position.y,
    root_position.z
    };

    
    uint32_t childnode_mask = 0;
    float half_node_length = full_node_length / 2;
    uint32_t node_is_full_sample_mask = 0;


    for (size_t Z = 0; Z < 2; Z++){
        for (size_t Y = 0; Y < 2; Y++){
            for (size_t X = 0; X < 2; X++){
                uint32_t temp_child_node_index = 0;
                temp_child_node_index = sample_8_corners_of_node(
                    vec3f { sample_position.x + X * half_node_length,
                            sample_position.y + Y * half_node_length,
                            sample_position.z + Z * half_node_length },
                    half_node_length,
                    sample_function
                );
                
                // if the node is part of the tree (not air) store it to be iterated over
                if(temp_child_node_index!=0){
                    childnode_mask |= 1<<(X + 2*Y + 4*Z);
                }

                // if all 8 corners of the potential node are full, write it to the full nodes mask
                if(temp_child_node_index==((1<<8)-1)){
                    node_is_full_sample_mask |= 1<<(X + 2*Y + 4*Z);
                }
            }
        }
    }

    #ifdef GENERATE_NOISE
    child_node_mask = RANDOM_BOOL;
    node_is_full_sample_mask = RANDOM_BOOL;
    #endif
    

    WL_LOG(WL_LOG_TRACE, "8 child nodes sampled!");

    #ifdef WL_DEBUG
    printf("the child node mask:");
    print_binary(childnode_mask);
    printf("the full nodes mask:");
    print_binary(node_is_full_sample_mask);
    #endif

    // we decide weather the node is a leaf of branch based on the mask results
    root[ROOT_OFFSET_DATA] = 0;
    uint32_t child_node_count = get_child_node_count(childnode_mask);
    if(child_node_count==0){
        root[ROOT_OFFSET_DATA] = 0;
        WL_LOG(WL_LOG_TRACE, "root is an empty leaf");
        return;
        
    }
    if(node_is_full_sample_mask == ((1<<8) - 1)) {
        root[ROOT_OFFSET_DATA] = material;
        WL_LOG(WL_LOG_TRACE, "root is a full leaf");
        return;
    }

///////////////////////////////////////////////////////////
        //      creating the child nodes        //

    WL_LOG(WL_LOG_TRACE, "proceeding with child node generation...");

    root[ROOT_OFFSET_DATA] = SVO_alloc_children_nodes(child_node_count);
    uint32_t children_index = root[ROOT_OFFSET_DATA];
    root[ROOT_OFFSET_DATA] |= childnode_mask<<23; //storing the mask
    root[ROOT_OFFSET_DATA] |= 1<<31; //setting the flag as branch

    // getting the order of the child nodes from the mask
    // this will assign which nth bit the child belongs to
    // this is so we can use a lookup table to get the position offset of the child node
    uint32_t counter = 0;
    uint32_t child_orders[child_node_count] = {};
    for (size_t i = 0; i < 8; i++){
        if(((childnode_mask>>i)&1)==1){
            child_orders[counter] = i;
            counter++;
        }
    }

    WL_LOG(WL_LOG_TRACE, "entering the black hole (recusive generation)...");

    //sending out the recursive algorithem
    for (size_t i = 0; i < child_node_count; i++){
        vec3f child_node_pos {
            root_position.x + child_offsets[child_orders[i]].x * half_node_length,
            root_position.y + child_offsets[child_orders[i]].y * half_node_length,
            root_position.z + child_offsets[child_orders[i]].z * half_node_length
        };
        generate_SVO_node_recursive(children_index+i, sample_function, material, root_voxel_level-1, smallest_level, child_node_pos);
    }

    #ifdef WL_DEBUG
    printf("created %u voxels.\n toatl bytes used: %u\n", debug_voxel_counter, SVO_manager.allocated_ammount*sizeof(uint_least32_t));
    #endif
    WL_LOG(WL_LOG_TRACE, " recursive Tree generated successfully!");
}   

/////////////////////////////////////////////////
        //       reading SVO      //

void read_SVO_node_recursive(
    uint32_t node_offset, 
    uint32_t node_level,
    uint32_t smallest_level,
    vec3f node_position,
    void (read_callback)(vec3f/*voxel position*/, float/*voxel side length*/, uint32_t/*neighbouring voxels mask*/, uint32_t/*material*/)
){
    //converting the index to the pointer from the array
    uint32_t* node = SVO_manager.SVO_array + node_offset;

    // the SVO node data
    uint32_t node_data = node[ROOT_OFFSET_DATA];
    
    // getting node meta data
    float full_node_length = SMALLEST_VOXEL_LENGTH * (1<<node_level);

    if(((node_data>>31) == 0) || (node_level == smallest_level)){
        read_callback(node_position, full_node_length, 0, node_data);
        return;
    }

    //for reading children nodes
    uint32_t childnode_mask = (node_data>>23) & ((1<<8)-1);

    // we read wether the node is a leaf of branch based on the mask results
    uint32_t child_node_count = get_child_node_count(childnode_mask);
    if(child_node_count==0){
        WL_LOG(WL_LOG_TRACE, "node is an empty leaf and even tho it said its not in the flag");
        return;
    }

///////////////////////////////////////////////////////////
        //      reading the child nodes        //

    WL_LOG(WL_LOG_TRACE, "proceeding with child nodes...");

    // getting the order of the child nodes from the mask
    // this will assign which nth bit the child belongs to
    // this is so we can use a lookup table to get the position offset of the child node
    uint32_t counter = 0;
    uint32_t child_orders[child_node_count] = {};
    for (size_t i = 0; i < 8; i++){
        if(((childnode_mask>>i)&1)==1){
            child_orders[counter] = i;
            counter++;
        }
    }

    WL_LOG(WL_LOG_TRACE, "entering the black hole (recusive generation)...");

    float half_node_length = full_node_length / 2;

    //sending out the recursive algorithem
    for (size_t i = 0; i < child_node_count; i++){
        vec3f child_node_pos {
            node_position.x + child_offsets[child_orders[i]].x * half_node_length,
            node_position.y + child_offsets[child_orders[i]].y * half_node_length,
            node_position.z + child_offsets[child_orders[i]].z * half_node_length
        };
        read_SVO_node_recursive(node_data+i, node_level-1, smallest_level, child_node_pos , read_callback);
    }

    #ifdef WL_DEBUG
    printf("created %u voxels.\n toatl bytes used: %u\n", debug_voxel_counter, SVO_manager.allocated_ammount*sizeof(uint_least32_t));
    #endif
    WL_LOG(WL_LOG_TRACE, " recursive Tree generated successfully!");
    
}
void wlReadSVO(SVOInstance SVO_root_offset, void (read_callback)(vec3f/*voxel position*/, float/*voxel side length*/, uint32_t/*neighbouring voxels mask*/, uint32_t/*material*/)){
    // converting the index to the pointer from the array
    uint32_t* root = SVO_manager.SVO_array + SVO_root_offset;
    WL_LOG(WL_LOG_TRACE, "the root point set");

    // the SVO node data
    uint32_t root_data = root[ROOT_OFFSET_DATA];
    
    // getting root meta data
    uint32_t root_voxel_level = root[ROOT_OFFSET_LEVEL];
    float full_node_length = SMALLEST_VOXEL_LENGTH * (1<<root_voxel_level);
    uint32_t smallest_level = root[ROOT_OFFSET_SMALLEST_LEVEL];
    vec3f root_position = {};
    root_position.x = *(float*)(root + 2);
    root_position.y = *(float*)(root + 3);
    root_position.z = *(float*)(root + 4);

    if(root_data>>31 == 0){
        read_callback(root_position, full_node_length, 0, root_data);
        return;
    }

    //for reading children nodes
    uint32_t childnode_mask = (root_data>>23) & ((1<<8)-1);

    // we read wether the node is a leaf of branch based on the mask results
    uint32_t child_node_count = get_child_node_count(childnode_mask);
    if(child_node_count==0){
        WL_LOG(WL_LOG_TRACE, "root is an empty leaf and even tho it said its not in the flag");
        return;
    }

///////////////////////////////////////////////////////////
        //      reading the child nodes        //

    WL_LOG(WL_LOG_TRACE, "proceeding with child node...");

    // getting the order of the child nodes from the mask
    // this will assign which nth bit the child belongs to
    // this is so we can use a lookup table to get the position offset of the child node
    uint32_t counter = 0;
    uint32_t child_orders[child_node_count] = {};
    for (size_t i = 0; i < 8; i++){
        if(((childnode_mask>>i)&1)==1){
            child_orders[counter] = i;
            counter++;
        }
    }

    WL_LOG(WL_LOG_TRACE, "entering the black hole (recusive generation)...");

    float half_node_length = full_node_length / 2;

    //sending out the recursive algorithem
    for (size_t i = 0; i < child_node_count; i++){
        vec3f child_node_pos {
            root_position.x + child_offsets[child_orders[i]].x * half_node_length,
            root_position.y + child_offsets[child_orders[i]].y * half_node_length,
            root_position.z + child_offsets[child_orders[i]].z * half_node_length
        };
        read_SVO_node_recursive(root_data+i, root_voxel_level-1, smallest_level, child_node_pos , read_callback);
    }

    #ifdef WL_DEBUG
    printf("created %u voxels.\n toatl bytes used: %u\n", debug_voxel_counter, SVO_manager.allocated_ammount*sizeof(uint_least32_t));
    #endif
    WL_LOG(WL_LOG_TRACE, " recursive Tree generated successfully!");
    
}   


