#ifndef WL_WINDOW
#define WL_WINDOW

#include <core.h>

// this will hold the data of the window, you just need to store it.
typedef struct WLWindow WLWindow;
typedef enum  {
    WL_WINDOWED,
    WL_FULLSCREEN_BORDERLESS,
    WL_FULLSCREEN_BORDERED
} WLWindowType; //defines the window-moniter behaviour ,when creating the window the app will specify this.


// the window will be wrapped in its own API so it can be either handled by the Renderer or be ran by the Main logic.
// you will need to make a WLwindow variable, and then create the Window, then update the window, and close the widnow when the app closes.
WLWindow* wlCreateWindow(const char* name, WLWindowType type);

// this is for when you change the settings of the window (type).
void wlReCreateWindow(WLWindow* window, const char* name, WLWindowType type);

// polls events for the window and updates it.
void wlUpdateWindow(WLWindow* window);

// optional if you want to close the window but keep the app running.
void wlDestroyWindow(WLWindow* window);

// this will determine the while() loop of the app.
bool wlWindowShouldClose(WLWindow* window);

// the X is width, the Y is height.
WL32uVec2 wlGetWindowSize(WLWindow* window);

// this is for the renderer to resize the frame buffer when the window is resized
//WLResult wlSetWindowResizeCallback(WLWindow* window, void (*callback)(GLFWwindow*, int /*width*/, int /*height*/));

// wrapper for glfwGetRequiredInstanceExtensions
const char** wlGetRequiredWindowInstanceExtensions(uint32_t* size);
// for renderer/Vulkan specific functions
void* wlGetRawWindowHandle(WLWindow* window);

#endif