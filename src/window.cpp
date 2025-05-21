#include <window.h>

struct WLWindow{
    GLFWwindow* pWindow;
    GLFWmonitor* pMonitor;
    WLU32Vec2 size;
    bool is_resizable;
};

WLWindow* wlCreateWindow(const char* name, WLWindowType type){
    WLWindow* window;
    // filling in the WLWindow data
    window = (WLWindow*)malloc(sizeof(WLWindow));
    if(!window){
        WL_LOG(WL_FAILED_TO_ALLOCATE_WINDOW);
        return NULL;
    }

    if(name == NULL){
        WL_LOG(WL_NAME_IS_NULL);
        free(window);
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

        WL_LOG(WL_FAILED_TO_CREATE_GLFW_WINDOW);
        free(window);
        return NULL;
    }

    return window;
}
WLResult wlReCreateWindow(WLWindow* window, const char* name, WLWindowType type){
    return WL_SUCCESS;
}
WLResult wlUpdateWindow(WLWindow* window){
    glfwPollEvents();
    return WL_SUCCESS;
}
WLResult wlPollWindow(WLWindow* window){
    glfwPollEvents();
    return WL_SUCCESS;
}
void wlDestroyWindow(WLWindow* window){
    glfwDestroyWindow(window->pWindow);

    free(window);
}
bool wlWindowShouldClose(WLWindow* window){
    return glfwWindowShouldClose(window->pWindow);
}
WLU32Vec2 getWindowSize(WLWindow* window){
    WLU32Vec2 size{0,0};
    return size;
}
