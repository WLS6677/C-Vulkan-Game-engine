#include <window.h>
#include <glfw3.h>
#include <glfw3native.h>

struct WLWindow{
    GLFWwindow* pWindow;
    GLFWmonitor* pMonitor;
    WLU32Vec2 size;
    bool is_resizable;
};

WLWindow* wlCreateWindow(const char* name, WLWindowType type){
    WLWindow* window;

    glfwInit();

    // filling in the WLWindow data
    window = (WLWindow*)wlAlloc(sizeof(WLWindow));
    if(!window){
        WL_LOG(WL_FATAL, "failed to allocate window");
        return NULL;
    }

    if(name == NULL){
        WL_LOG(WL_FATAL, "window name is NULL");
        return NULL;
    }

    //temporary defualt size
    window->size.x = 800;
    window->size.y = 600;

    window->pMonitor = NULL;
    if(type!=WL_WINDOWED) {
        window->pMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        window->size.x = mode->width;
        window->size.y = mode->height;
    }

    // actaully creating the window
    window->pWindow = glfwCreateWindow(window->size.x, window->size.y, name, window->pMonitor, NULL);

    if(!window->pWindow) {
        WL_LOG(WL_FATAL, "failed to create glfw window Handle");
        return NULL;
    }

    return window;
}
void wlReCreateWindow(WLWindow* window, const char* name, WLWindowType type){
}
void wlUpdateWindow(WLWindow* window){
    glfwPollEvents();
}
void wlDestroyWindow(WLWindow* window){
    glfwDestroyWindow(window->pWindow);
    glfwTerminate();
}
bool wlWindowShouldClose(WLWindow* window){
    if(window==NULL){
        WL_LOG(WL_WARNING, "window doesnt exist"); 
        return false;
    }
    return glfwWindowShouldClose(window->pWindow);
}
WLU32Vec2 getWindowSize(WLWindow* window){
    WLU32Vec2 size{0,0};
    return size;
}
const char** wlGetRequiredWindowInstanceExtensions(uint32_t* pSize){
    return glfwGetRequiredInstanceExtensions(pSize);
}