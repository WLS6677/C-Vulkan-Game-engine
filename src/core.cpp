#include <core.h>

void wlLog(WLResult result){
    switch(result){

        case WL_TEST: printf("this is a test and the LOG fucntion is working!\n");break;
        case WL_NAME_IS_NULL: printf("a NULL pointer was passed as an arg to a function\n");break;
        case WL_FAILED_TO_CREATE_WINDOW: printf("failed to create glfw windown\n");break;
        case WL_FAILED_TO_ALLOCATE_WINDOW: printf("failed to allocate window\n");break;
        case WL_SUCCESS:break;
        
        defualt:
        case WL_FAIL:
        printf("Unknown Error\ns");
    }

}

WLResult wlInitEngine(WLEngine* engine) {

    return WL_SUCCESS;
}




