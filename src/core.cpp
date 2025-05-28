#include <core.h>
#include <stdarg.h>
#include <time.h>

struct WLAllocator {
    uint8_t* rootPointer;
    uint32_t allocated_amount;
    uint32_t max_allocation_size;
};
static WLAllocator allocator;
void wlSetupAllocator(uint32_t size){
    allocator.allocated_amount = 0;
    allocator.max_allocation_size = size;
    allocator.rootPointer = (uint8_t*)malloc(size);
    if(allocator.rootPointer==NULL){
        WL_LOG(WL_LOG_WARNING, "allocater failed to malloc memory block");
    }
}
void wlReleaseAlloc(){
    if(allocator.rootPointer==NULL){
        return;
    }
    free(allocator.rootPointer);
}
uint8_t* wlAlloc(uint32_t request_size){
    uint8_t* return_pointer;

    // check if there is capacity to allocate
    if(request_size > allocator.max_allocation_size-allocator.allocated_amount){
        WL_LOG(WL_LOG_FATAL,"requested allocation surpasses max allocation amount");
        printf("Max size: %u\n", allocator.max_allocation_size);
        printf("current allocated amount: %u\n", allocator.allocated_amount);
        printf("requested amount: %u\n", request_size);
        return NULL;
    }

    // gives the address thats after the currently already allocated offset
    return_pointer = allocator.rootPointer + allocator.allocated_amount;

    // changes the allocated ammount to the actual allocated ammount which is +1 from the 
    allocator.allocated_amount += request_size;


    #ifdef WL_DEBUG
    printf("allocated %u bytes.     total allocated ammount: %u bytes\n", request_size, allocator.allocated_amount);
    #endif

    return return_pointer;
}
void wlPrintAllocatorInfo(){
    printf("Max size:                 %u\n", allocator.max_allocation_size);
    printf("current allocated amount: %u\n\n", allocator.allocated_amount);
}



uint8_t* wlReadFile(const char* file_name, uint32_t* file_size){
    FILE* file = fopen(file_name, "rb");
    if (!file) return NULL;

    fseek(file, 0, SEEK_END);
    *file_size = ftell(file);
    rewind(file);

    uint8_t* buffer = (uint8_t*)malloc(*file_size);
    fread(buffer, 1, *file_size, file);
    fclose(file);

    return buffer;
}


void wlLog(WLResult result, const char* file, uint32_t line, const char* func, const char* msg){
    switch(result){
        case WL_LOG_PRINT:
        printf("%s", msg);
        break;

        default:
        case WL_LOG_TRACE:
        printf("%s: %u: %s(): %s\n", file + 4, line, func,  msg);
        break;

        case WL_LOG_WARNING:
        printf("[warning] %s: %u: %s(): %s\n", file + 4, line, func,  msg);
        fflush(stdout);
        break;
        
        case WL_LOG_FATAL:
        printf("[FATAL] %s: %u: %s(): %s\n", file + 4, line, func,  msg);
        fflush(stdout); //prints immediately
        break;
    }

}

//////////////////////////////////////
//                                  //
//           E N G I N E            //
//                                  //
//////////////////////////////////////

struct WLEngine {
    const char* engine_name;
    WLScene main_scene;
};
WLEngine* wlCreateEngine(){
    WLEngine* engine;

    wlSetupAllocator(1024*1024*128);
    engine = (WLEngine*)wlAlloc(sizeof(WLEngine));
    if(engine==NULL){
        WL_LOG(WL_LOG_FATAL, "failed to allocate memory for engine");
        wlReleaseAlloc();
        return NULL;
    }

    wlCreateWindow("yo", WL_WINDOWED);
    

    void* window_wandle = wlGetRawWindowHandle();
    
    wlCreateRenderer(window_wandle);
    if(false)
    {
        WL_LOG(WL_LOG_FATAL, "failed to create renderer");
        wlReleaseAlloc();
        return NULL;
    }

    WLRenderObject HELLO_TRIANGLE = {};

    WLVertex HELLO_VERTEX[] = {
        {{0.5f,0.0f,0.0f},{1.0f,0.0f,0.0f}},
        {{-0.5f,0.0f,0.0f},{0.0f,1.0f,0.0f}},
        {{0.0f,0.5f,0.0f},{0.0f,0.0f,1.0f}},

        {{0.5f,0.5f,0.0f},{1.0f,0.0f,0.0f}},
        {{-0.5f,0.0f,0.0f},{0.0f,1.0f,0.0f}},
        {{0.0f,0.5f,0.0f},{0.0f,0.0f,1.0f}},

        {{-0.5f,0.0f,0.0f},{1.0f,0.0f,0.0f}},
        {{0.5f,0.0f,0.0f},{0.0f,1.0f,0.0f}},
        {{0.0f,-0.5f,0.0f},{0.0f,0.0f,1.0f}},

        {{0.0f,-0.5f,0.0f},{1.0f,0.0f,0.0f}},
        {{0.5f,0.5f,0.0f},{0.0f,1.0f,0.0f}},
        {{-0.5f,0.5f,0.0f},{0.0f,0.0f,1.0f}},

        {{5.0f,-5.0f,0.0f},{1.0f,0.0f,0.0f}},
        {{-5.0f,5.0f,0.0f},{1.0f,0.0f,0.0f}},
        {{-5.0f,-5.0f,0.0f},{1.0f,0.0f,0.0f}},

        {{10.5f,0.0f,0.0f},{1.0f,0.0f,0.0f}},
        {{-10.5f,0.0f,0.0f},{0.0f,1.0f,0.0f}},
        {{10.0f,0.5f,0.0f},{0.0f,0.0f,1.0f}},

        {{10.5f,0.5f,0.0f},{1.0f,0.0f,0.0f}},
        {{-0.5f,0.0f,0.0f},{0.0f,1.0f,0.0f}},
        {{10.0f,0.5f,0.0f},{0.0f,0.0f,1.0f}},

        {{-0.5f,0.0f,0.0f},{1.0f,0.0f,0.0f}},
        {{10.5f,0.0f,0.0f},{0.0f,1.0f,0.0f}},
        {{0.0f,-0.5f,0.0f},{0.0f,0.0f,1.0f}},

        {{10.0f,-0.5f,0.0f},{1.0f,0.0f,0.0f}},
        {{10.5f,0.5f,0.0f},{0.0f,1.0f,0.0f}},
        {{-10.5f,0.5f,0.0f},{0.0f,0.0f,1.0f}},
    };

    HELLO_TRIANGLE.pVertex_buffer = HELLO_VERTEX;
    HELLO_TRIANGLE.vertex_count = sizeof(HELLO_VERTEX)/sizeof(HELLO_VERTEX[0]);

    

    engine->main_scene.camera.position.x = 5.4f;
    engine->main_scene.camera.position.y = -5.2f;
    engine->main_scene.camera.position.z = 2.01f;

    wlInitSVO();
    engine->main_scene.chunk_count = 1;
    engine->main_scene.pChunks = (WLChunk*)wlAlloc(sizeof(WLChunk));
    
    engine->main_scene.pChunks[1].SVO = wlCreateSVOInstance(vec3f{-8.0f,-8.0f,-8.0f}, WL_SVO_LEVEL_16_METERS, WL_SVO_LEVEL_4_METERS);
    wlGenerateChunkSVO(&engine->main_scene.pChunks[1]);
    WLRenderObject chunk_mesh = WLGetChunkSVOMesh(engine->main_scene.pChunks[0]);
    
    wlInitVertexBuffer(&chunk_mesh, 1);

    return engine;
}
void wlDestroyEngine(WLEngine* engine){
    wlDestroyWindow();
    wlDestroyRenderer();
    wlReleaseAlloc();
}
void wlRunEngine(WLEngine* engine){
    while(!wlWindowShouldClose()){
        // polling inputs
        wlUpdateWindow();

        // getting the camera aim direction
        WL32fVec2 cursor_pos = wlGetCursorPositon();
        engine->main_scene.camera.yaw = -360*cursor_pos.x;
        engine->main_scene.camera.pitch = 180*cursor_pos.y;

        wlMoveCamera(&engine->main_scene, 0.01f);

        // getting the camera
        glm::mat4 camera_matrix = wlGetCameraMatrix(engine->main_scene);
        wlRender(camera_matrix);
    }
    printf("shutting down...");
}




