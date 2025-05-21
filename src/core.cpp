#include <core.h>

struct WLEngine {
    WLRenderer* pRenderer;
    WLWindow* pWindow;
    const char* engine_name;
};

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

WLEngine* wlCreateEngine(){
    WLEngine* engine;
    
    engine = (WLEngine*)malloc(sizeof(WLEngine));
    if(engine==NULL){
        WL_LOG(WL_FAILED_TO_ALLOCATE);
        free(engine);
        return NULL;
    }

    glfwInit();

    //engine->pRenderer = wlCreateRenderer();

    engine->pWindow = wlCreateWindow("yo", WL_WINDOWED);
    if(engine->pRenderer==NULL){
        WL_LOG(WL_FAILED_TO_CREATE_WINDOW);
        free(engine);
        return NULL;
    }

    return engine;

}

void wlDestroyEngine(WLEngine* engine){
    wlDestroyRenderer(engine->pRenderer);
    wlDestroyWindow(engine->pWindow);
    free(engine);
}

void wlRunEngine(WLEngine* engine){
    while(!wlWindowShouldClose(engine->pWindow)){
        wlUpdateWindow(engine->pWindow);
    }
    
}




