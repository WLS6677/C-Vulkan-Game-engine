#include <core.h>
#include <stdarg.h>

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
    //wlPrintAllocatorInfo();
    //WL_LOG(WL_TRACE, "hello");
    return return_pointer;
}
void wlPrintAllocatorInfo(){
    printf("Max size:                 %u\n", allocator.max_allocation_size);
    printf("current allocated amount: %u\n\n", allocator.allocated_amount);
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
    WLRenderer* pRenderer;
    WLWindow* pWindow;
    const char* engine_name;
};
WLEngine* wlCreateEngine(){
    WLEngine* engine;

    wlSetupAllocator(1024*1024*8);
    engine = (WLEngine*)wlAlloc(sizeof(WLEngine));
    if(engine==NULL){
        WL_LOG(WL_LOG_FATAL, "failed to allocate memory for engine");
        wlReleaseAlloc();
        return NULL;
    }

    engine->pWindow = wlCreateWindow("yo", WL_WINDOWED);
    if(engine->pWindow==NULL){
        WL_LOG(WL_LOG_FATAL, "failed to create window");
        wlReleaseAlloc();
        return NULL;
    }

    void* window_wandle = wlGetRawWindowHandle(engine->pWindow);
    
    engine->pRenderer = wlCreateRenderer(window_wandle);
    if(engine->pRenderer==NULL)
    {
        WL_LOG(WL_LOG_FATAL, "failed to create renderer");
        //wlReleaseAlloc();
        //return NULL;
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




