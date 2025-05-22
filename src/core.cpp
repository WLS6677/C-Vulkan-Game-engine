#include <core.h>

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
        WL_LOG(WL_FAILED_TO_ALLOCATE);
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
        printf("requested allocation surpasses max allocation amount\n");
        printf("Max size: %u\n", allocator.max_allocation_size);
        printf("current allocated amount: %u\n", allocator.allocated_amount);
        printf("requested amount: %u\n", request_size);
        return NULL;
    }

    // gives the address thats after the currently already allocated offset
    return_pointer = allocator.rootPointer + allocator.allocated_amount;

    // changes the allocated ammount to the actual allocated ammount which is +1 from the 
    allocator.allocated_amount += request_size;
    wlPrintAllocatorInfo();
    return return_pointer;
}
void wlPrintAllocatorInfo(){
    printf("Max size:                 %u\n", allocator.max_allocation_size);
    printf("current allocated amount: %u\n\n", allocator.allocated_amount);
}



void wlLog(WLResult result){
    switch(result){

        case WL_TEST: printf("this is a test and the LOG fucntion is working!\n");break;
        case WL_NAME_IS_NULL: printf("a NULL pointer was passed as an arg to a function\n");break;
        case WL_FAILED_TO_ALLOCATE: printf("malloc failed somewhere\n");
        case WL_FAILED_TO_ALLOCATE_WINDOW: printf("failed to allocate window\n");break;
        case WL_FAILED_TO_CREATE_GLFW_WINDOW: printf("failed to create glfw windown\n");break;
        case WL_FAILED_TO_CREATE_WINDOW: printf("failed to create window\n");break;
        case WL_FAILED_TO_CREATE_RENDERER: printf("failed to create Renderer\n");break;
        case WL_SUCCESS:break;
        
        defualt:
        case WL_FAIL:
        printf("Unknown Error\n");
    }

}

//////////////////////////////////////
//                                  //
//           E N G I N E            //
//                                  //
//////////////////////////////////////

struct WLEngine {
    WLRenderer* pRenderer;
    WLWindow* pWindow;
    const char* engine_name;
};
WLEngine* wlCreateEngine(){
    WLEngine* engine;

    wlSetupAllocator(1024*1024*8);
    engine = (WLEngine*)wlAlloc(sizeof(WLEngine));
    if(engine==NULL){
        WL_LOG(WL_FAILED_TO_ALLOCATE);
        wlReleaseAlloc();
        return NULL;
    }

    //engine->pRenderer = wlCreateRenderer();

    engine->pWindow = wlCreateWindow("yo", WL_WINDOWED);
    if(engine->pWindow==NULL){
        WL_LOG(WL_FAILED_TO_CREATE_WINDOW);
        wlReleaseAlloc();
        return NULL;
    }

    return engine;

}
void wlDestroyEngine(WLEngine* engine){
    wlDestroyRenderer(engine->pRenderer);
    wlDestroyWindow(engine->pWindow);
    wlReleaseAlloc();
}
void wlRunEngine(WLEngine* engine){
    while(!wlWindowShouldClose(engine->pWindow)){
        wlUpdateWindow(engine->pWindow);
    }
}




