#ifndef WL_CORE
#define WL_CORE

#include <stdio.h>
#include <time.h>
#include <wlmath.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

//this is the debugging logging macro, it will either lead to a debug manager or do nothing on release
#ifdef WL_DEBUG
    #define WL_LOG(msg) wlLog(msg)
#else
    #define WL_LOG(msg) ((void)0)
#endif

// debug flags
typedef enum {
    WL_SUCCESS,
    WL_FAIL,
    WL_TEST,
    WL_NAME_IS_NULL, // a const char* had NULL as input 
    WL_FAILED_TO_ALLOCATE_WINDOW, // when calling malloc for WLWindow
    WL_FAILED_TO_CREATE_WINDOW, // glfwCreateWIndow();
} WLResult;

// in debug mode, this will be used to log any errors in all moduless
void wlLog(WLResult); 

// this struct will be responsible for storing the data of the Engine
typedef struct {} WLEngine;

#ifdef __cplusplus
} //extern "C"
#endif

//these are the modules, i need to include them after the debugging difintions to prevent errors 
#include <renderer.h>
#include <window.h>

#endif // #ifndef WL_CORE
