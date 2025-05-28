#include <window.h>
#include <glfw3.h>
#include <glfw3native.h>

struct WLWindow{
    GLFWwindow* pWindow;
    GLFWmonitor* pMonitor;
    WL32uVec2 size;
    bool is_resizable;
};

static WLWindow window;

void wlCreateWindow(const char* name, WLWindowType type){

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if(name == NULL){
        WL_LOG(WL_LOG_FATAL, "window name is NULL");
        return;
    }

    //temporary defualt size
    window.size.x = 800;
    window.size.y = 600;

    window.pMonitor = NULL;
    if(type!=WL_WINDOWED) {
        window.pMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        window.size.x = mode->width;
        window.size.y = mode->height;
    }

    // actaully creating the window
    window.pWindow = glfwCreateWindow(window.size.x, window.size.y, name, window.pMonitor, NULL);

    if(!window.pWindow) {
        WL_LOG(WL_LOG_FATAL, "failed to create glfw window Handle");
        return;
    }

    glfwSetInputMode(window.pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
void wlReCreateWindow(const char* name, WLWindowType type){
}
void wlUpdateWindow(){
    glfwPollEvents();
}
void wlDestroyWindow(){
    glfwDestroyWindow(window.pWindow);
    glfwTerminate();
}
bool wlWindowShouldClose(){
    if(window.pWindow==NULL){
        WL_LOG(WL_LOG_WARNING, "window doesnt exist"); 
        return false;
    }
    return glfwWindowShouldClose(window.pWindow);
}
WL32uVec2 getWindowSize(){
    return WL32uVec2 {
        window.size.x,
        window.size.y
    };
}

WL32fVec2 wlGetCursorPositon(){

    // getting the cursor position relative to the upper left corner in monitor pixels
    double cursor_monitor_pos[2] = {0.0f, 0.0f};
    glfwGetCursorPos(window.pWindow, &cursor_monitor_pos[0], &cursor_monitor_pos[1]);

    // converting the relative scale to (2.0 units = full window size)
    WL32fVec2 new_cursor_pos = {
        (cursor_monitor_pos[0] - (window.size.x/2))/window.size.x,
        (cursor_monitor_pos[1] - (window.size.y/2))/window.size.y
    };

    return new_cursor_pos;
}
WL32fVec3 wlGetMovementDirectionFreeMove(){
    float forward = 0.0f, back = 0.0f, right = 0.0f, left = 0.0f, up = 0.0f, down = 0.0f;

    if(glfwGetKey(window.pWindow, GLFW_KEY_W)==GLFW_PRESS){ forward = 1.0f; }
    if(glfwGetKey(window.pWindow, GLFW_KEY_S)==GLFW_PRESS){ back = 1.0f; }
    if(glfwGetKey(window.pWindow, GLFW_KEY_D)==GLFW_PRESS){ right = 1.0f; }
    if(glfwGetKey(window.pWindow, GLFW_KEY_A)==GLFW_PRESS){ left = 1.0f; }
    if(glfwGetKey(window.pWindow, GLFW_KEY_SPACE)==GLFW_PRESS){ up = 1.0f; }
    if(glfwGetKey(window.pWindow, GLFW_KEY_C)==GLFW_PRESS){ down = 1.0f; }

    #ifdef WL_DEBUG_no
    if (!glfwGetWindowAttrib(window.pWindow, GLFW_FOCUSED)) {
        WL_LOG(WL_LOG_WARNING, "Window not focused");
    }
    vec3f debug_vec = wlget32fVec3Normalized (WL32fVec3 {
        right - left,
        forward - back,
        up - down
    });
    printf("movement direction (%f, %f, %f)\n", debug_vec.x, debug_vec.y, debug_vec.z);
    
    #endif

    return wlget32fVec3Normalized(WL32fVec3 {
        left - right,
        forward - back,
        up - down
    });
}

const char** wlGetRequiredWindowInstanceExtensions(uint32_t* pSize){
    return glfwGetRequiredInstanceExtensions(pSize);
}
void* wlGetRawWindowHandle(){
    return (void*)window.pWindow;
}



